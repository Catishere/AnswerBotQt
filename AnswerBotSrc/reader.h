#ifndef READER_H
#define READER_H

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <dinput.h>

#include <QDebug>
#include <QTimer>
#include <QSettings>
#include <QMap>
#include <QList>
#include <QByteArray>
#include <QConsoleListener>

#include "answerer.h"
#include "bot.h"
#include "structs.h"

#define CHATMARK_OFFSET 487
#define COMBO_KEY_DELAY 100
#define SAMPLE_RATE 10

#define REQ_CMD "!m"
#define TRANS_CMD "!t"

class Reader : public QObject
{
    Q_OBJECT
    Answerer answerer;
    Bot bot;
    QList<QByteArray> prevLines;
    QMap<const QByteArray, int> comboMap;
    QSettings settings;
    QByteArray nickname;
    QByteArray delimiter;
    QByteArray nickdel;
    QTimer *timer;

    HMODULE clientDll;
    HMODULE hwDll;
    HANDLE processHandle;

    DWORD pid;
    DWORD baseAddress;
    UINT_PTR chatAddress;
    UINT_PTR prisonersAddress;
    UINT_PTR hudAddress;

    QByteArray Utf8Encode(const wchar_t * wstr) const;
    int GetHLProcessID();
    int GetDlls();
    int OpenP();
    int CloseP();
    void read();
    void eventLoop();
    int handleCombo(char * combo_cstr);
    int handleComboInstruction(QByteArray &instruction);
    int updatePrisoners();
public slots:
    void processGameMemory();
public:
    explicit Reader(QObject *parent = nullptr);
    ~Reader();
};

#endif // READER_H
