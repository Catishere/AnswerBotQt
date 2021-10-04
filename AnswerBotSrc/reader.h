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

#include "answerer.h"
#include "structs.h"

#define CHATMARK_OFFSET 487
#define COMBO_KEY_DELAY 100
#define KEYHOLD_DURATION 20
#define SAMPLE_RATE 10
#define NICK "Cat"

#ifdef SIMON
#define DELIM "\x01:\x04 "
#else
#define DELIM ": "
#endif

#define REQ_CMD "!m"
#define TRANS_CMD "!t"
#define NICK_LOWER "cat"

class Reader : public QObject
{
    Q_OBJECT
    Answerer answerer;
    QList<QByteArray> prevLines;
    QMap<const QByteArray, int> comboMap;
    QSettings settings;
    QString nickname;
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
    void SendKey(const int key);
    void read();
    void eventLoop();
    bool isGameFocused() const;
    int handleCombo(char * combo_cstr);
    int handleComboInstruction(QByteArray &instruction);
    int updatePrisoners();
public slots:
    void processGameMemory();
    void executeSlot(int mode);
public:
    explicit Reader(QObject *parent = nullptr);
    ~Reader();
};

#endif // READER_H
