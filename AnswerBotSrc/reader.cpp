#include "reader.h"

QByteArray Reader::Utf8Encode(const wchar_t * wstr) const
{
    QByteArray ba;
    int size = (int)wcslen(wstr);
    if (size <= 0) return ba;

    int nsize =
            WideCharToMultiByte(CP_UTF8, 0, wstr, size, NULL, 0, NULL, NULL);

    ba.resize(nsize);
    WideCharToMultiByte(CP_UTF8, 0, wstr, size, ba.data(), nsize, NULL, NULL);
    return ba;
}

int Reader::handleCombo(char *combo_cstr)
{
    auto list = QByteArray(combo_cstr).split('\n');
    if (!list[0].isEmpty() && list[0][0] != '-') return 1;
    list[0].chop(2);
    list[0].remove(0, 2);
    for (auto &ins : list) {
       handleComboInstruction(ins);
    }
    return 0;
}

int Reader::handleComboInstruction(QByteArray &instruction)
{
    auto it = comboMap.find(instruction.trimmed());
    if (it == comboMap.end()) {
        qDebug() << "Error: Combo command " << instruction << " not found";
        return 1;
    }

    Bot::sendKey(it.value());
    Sleep(COMBO_KEY_DELAY);
    return 0;
}

int Reader::GetHLProcessID()
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (_tcscmp(entry.szExeFile, L"hl.exe") == 0)
            {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,
                                              entry.th32ProcessID);
                pid = entry.th32ProcessID;
                CloseHandle(hProcess);
            }
        }
    }
    return 0;
}

int Reader::GetDlls()
{
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;

    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                            PROCESS_VM_READ,
                            FALSE, pid );
    if (NULL == hProcess)
        return 1;

    if( EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded,
                             LIST_MODULES_ALL))
    {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH];

            if ( GetModuleBaseName(hProcess, hMods[i], szModName,
                                      sizeof(szModName) / sizeof(TCHAR)))
            {
                if (!_tcscmp(szModName, L"client.dll")) {
                    clientDll = hMods[i];
                    chatAddress = (UINT_PTR)clientDll + 0x136E22;
                    prisonersAddress = (UINT_PTR)clientDll + 0x125210;
                    qDebug() << "Found client.dll";
                } else if (!_tcscmp(szModName, L"hw.dll")) {
                    hwDll = hMods[i];
                    hudAddress = (UINT_PTR)hwDll + 0x7ECFA0;
                    chatInputAddress = (UINT_PTR)hwDll + 0xAB4060;
                    qDebug() << "Found hw.dll";
                }
            }
        }
    }

    CloseHandle( hProcess );

    return 0;
}

Reader::Reader(QObject *parent)
    : QObject(parent),
      answerer(settings, this),
      settings(QSettings::IniFormat, QSettings::UserScope,
                           "CatCompany", "AnswerBot")
{
    if (!settings.contains("path"))
        settings.setValue("path", "YOUR_CSTRIKE_PATH");
    settings.sync();

    // listen to console input
    QConsoleListener console;
    connect(&console, &QConsoleListener::newLine,
            parent, [=](const QString &line) {
        if (line.startsWith("normal"))
            delimiter = "\x01 : \x01";
        else if (line.startsWith("danger"))
            delimiter = "\x01: ";
        else {
            delimiter = QString(line)
                    .replace('1', '\x01')
                    .replace('2', '\x02')
                    .replace('3', '\x03')
                    .replace('4', '\x04')
                    .replace('s', ' ')
                    .toUtf8();
        }
        qDebug() << delimiter;
        nickdel = nickname + delimiter;
    });

    nickname = settings.value("user").toString().toUtf8();
    delimiter = "\x01: ";
    nickdel = nickname + delimiter;

    comboMap = {
        {"Nadqsno",         DIK_D       },
        {"Nalqvo",          DIK_A       },
        {"Napred",          DIK_W       },
        {"Nazad",           DIK_S       },
        {"Prezarejdane",    DIK_R       },
        {"Klqkane",         DIK_LCONTROL},
        {"TAB",             DIK_TAB     },
        {"Skok",            DIK_SPACE   },
        {"Ritnik",          DIK_F11     },
        {"Umruk",           DIK_F10     },
        {"Dqsna strelka",   DIK_F8      },
        {"Lqva strelka",    DIK_F9      },
        {"Buton \"E\"",     DIK_E       }
    };

    GetHLProcessID();
    GetDlls();
    OpenP();
    read();
    eventLoop();
    CloseP();
}

Reader::~Reader()
{
    delete timer;
}

int Reader::OpenP()
{

    processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                FALSE, pid);
    return processHandle == NULL;
}

int Reader::CloseP()
{
    CloseHandle(processHandle);
    return 0;
}

void Reader::read()
{
    updatePrisoners();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout,
            this, &Reader::processGameMemory);
    timer->start(SAMPLE_RATE);
}

void Reader::eventLoop()
{
    QEventLoop loop;
    QTimer timer;
    connect(&answerer, &Answerer::interruptLoop, &loop, &QEventLoop::quit);
    while (true) {
        loop.exec();
    }
}

void Reader::processGameMemory()
{
    chat_struct chat;
    hud_struct hud;
    char chatInput[120];
    char lastHud[HUD_SECTOR_LEN];
    bool isCry = false;
    bool isCombo = true;
    size_t bytesRead;

    if (ReadProcessMemory(processHandle,
                          (void *)(chatAddress),
                          (TCHAR *)&chat,
                          sizeof(chat_struct),
                          &bytesRead))
    {
        for (auto &line : chat.lines) {
            auto qbaLine = Utf8Encode(line);
            if (qbaLine.endsWith('\x03')) qbaLine.chop(1); // fix broken servers
            if (prevLines.contains(qbaLine))
                continue;

            bool any = true;

            if (qbaLine.indexOf(nickdel + REQ_CMD " ") >= 0) {
                Bot::sendKey(DIK_NUMPAD6);
                auto qi = qbaLine.lastIndexOf(REQ_CMD);
                if (qi < 0) continue;
                QByteArray ans = answerer
                        .answer(qbaLine
                                .right(qbaLine.size() - qi - sizeof(REQ_CMD)));
                qDebug() << "> " << ans;
                if (!ans.startsWith("Not found"))
                    Bot::sendKey(DIK_NUMPAD4);
            } else if (qbaLine.indexOf(nickdel + "clr") >= 0) {
                isCry = false;
                prevLines.clear();
                qDebug() << "Reset list";
            } else if (qbaLine.indexOf(nickdel + "ok") >= 0) {
                isCombo = false;
                qDebug() << "Reset combo";
            } else if (qbaLine.indexOf(nickdel + "ko") >= 0) {
                isCombo = true;
                qDebug() << "Stop combo";
            } else if (qbaLine.indexOf(nickdel) >= 0) {
                auto index = qbaLine.size() - qbaLine.lastIndexOf(delimiter)
                        - delimiter.size();
                qDebug() << "Chat:" << qbaLine.right(index);
            } else if (!isCry
                       && qbaLine.indexOf(nickname + " to SPECTATOR") >= 0) {
                QByteArray ba("ne sum afk we");
                answerer.makeWrite(ba);
                Bot::sendKey(DIK_NUMPAD4);
                Bot::sendKey(DIK_NUMPAD5);
                qDebug() << "Cried spectator";
                isCry = true;
            } else if (qbaLine.indexOf("[Quest]\x03") >= 0) {
                Bot::sendKey(DIK_NUMPAD6);
                auto qi = qbaLine.lastIndexOf("\x04");
                if (qi < 0) continue;
                auto index = qbaLine.size() - qi - 2;
                updatePrisoners();
                qDebug() << "Quest Answer: "
                         << answerer.answer(qbaLine.right(index));
                Bot::sendKey(DIK_NUMPAD4);
            } else if (qbaLine.indexOf(" " + nickname) >= 0
                       || qbaLine.indexOf(" " + nickname.toLower()) >= 0) {
                qDebug() << qbaLine;
            } else {
                any = false;
            }

            if (any) {
                prevLines.append(qbaLine);
                if (prevLines.size() > 5)
                    prevLines.removeFirst();
            }
        }
    }

    if (ReadProcessMemory(processHandle,
                          (void *)(hudAddress),
                          (TCHAR *)&hud,
                          sizeof(hud_struct),
                          &bytesRead))
    {
        for (auto &sector : hud.sectors) {
            if (!strcmp(lastHud, sector))
                continue;
            QString qSector(sector);
            if (!isCombo && qSector.count('\n') > 5) {
                isCombo = true;
                qDebug() << "Executing Combo";
                handleCombo(sector);
            } else if (qSector.startsWith("JailBreak - Day ")) {
                answerer.setJbDay(QString(qSector.right(2).trimmed()));
            }
        }
    }

    if (ReadProcessMemory(processHandle,
                          (void *)(chatInputAddress),
                          chatInput,
                          sizeof(chatInput),
                          &bytesRead))
    {
        QByteArray line(chatInput);
        if (!prevLines.contains(line)
                && line.endsWith("#")
                && line.indexOf(TRANS_CMD " ") >= 0) {
            Bot::sendKey(DIK_NUMPAD6);
            Bot::sendKey(DIK_ESCAPE);
            line.chop(1);
            answerer.translate(line
                       .right(line.size() - sizeof(TRANS_CMD)));
            prevLines.append(line);
            if (prevLines.size() > 5)
                prevLines.removeFirst();
        }
    }
}

int Reader::updatePrisoners()
{
    char prisoners[32];
    size_t bytesRead;
    if (ReadProcessMemory(processHandle,
                          (void *)(prisonersAddress),
                          prisoners,
                          sizeof(prisoners),
                          &bytesRead))
    {
        QRegularExpression re("Prisoners: ([0-9]+) Alive / ([0-9]+) Total");
        auto match = re.match(QByteArray(prisoners));
        if (match.hasMatch()) {
            answerer.setAlivePrisoners(match.captured(1));
            answerer.setTotalPrisoners(match.captured(2));
            return 0;
        }
    }
    return 1;
}
