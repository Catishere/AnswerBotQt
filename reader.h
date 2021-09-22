#ifndef READER_H
#define READER_H

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <dinput.h>

#include <QDebug>
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

class Reader
{
    Answerer answerer;
    QList<QByteArray> prevLines;
    QMap<const QByteArray, int> comboMap;
    QSettings settings;
    QString nickname;

    HMODULE clientDll;
    HMODULE hwDll;
    HANDLE processHandle;

    DWORD pid;
    DWORD baseAddress;
    UINT_PTR chatAddress;
    UINT_PTR prisonersAddress;
    UINT_PTR hudAddress;

    int GetHLProcessID();
    int GetDlls();
    QByteArray Utf8Encode(const wchar_t * wstr) const;
    int handleCombo(char * combo_cstr);
    int handleComboInstruction(QByteArray &instruction);
    int OpenP();
    int CloseP();
    void SendKey(const int key);
    int Read();
    bool isGameFocused() const;
    int updatePrisoners();
public:
    Reader();
};

#endif // READER_H
