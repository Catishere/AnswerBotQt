#include "bot.h"

Bot::Bot()
{

}

void Bot::sendKey(const int key)
{
    if (!isGameFocused()) return;

    INPUT inp[2];
    memset(inp, 0, sizeof(INPUT));
    inp[0].type = INPUT_KEYBOARD;
    inp[0].ki.dwFlags = KEYEVENTF_SCANCODE;
    inp[1] = inp[0];
    inp[1].ki.dwFlags |= KEYEVENTF_KEYUP;

    inp[0].ki.wScan = inp[1].ki.wScan = key;
    SendInput(1, inp, sizeof(INPUT));
    Sleep(KEYHOLD_DURATION);
    SendInput(1, inp + 1, sizeof(INPUT));
}

bool Bot::isGameFocused()
{
    TCHAR title[PATH_MAX];
    GetWindowText(GetForegroundWindow(), title, PATH_MAX);
    return !_tcsicmp(L"Counter-strike", title);
}

void Bot::execute(int mode)
{
    if (mode & 1)
        sendKey(DIK_NUMPAD4);
    if (mode & (1 << 1))
        sendKey(DIK_NUMPAD5);
}
