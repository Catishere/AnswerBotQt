#ifndef BOT_H
#define BOT_H

#include <windows.h>
#include <tchar.h>
#include <dinput.h>

#define KEYHOLD_DURATION 20

class Bot
{
public:
    Bot();
    static void sendKey(const int key);
    static bool isGameFocused();
    static void execute(int mode);
};

#endif // BOT_H
