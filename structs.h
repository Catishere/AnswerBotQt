#ifndef CHATSTRUCT_H
#define CHATSTRUCT_H

#include <tchar.h>
#define HUD_SECTOR_LEN 512
#define CHAT_LINE_LEN 270

struct chat_struct {
    TCHAR lines[5][CHAT_LINE_LEN];
};

struct hud_struct {
    char sectors[4][HUD_SECTOR_LEN];
};

#endif // CHATSTRUCT_H
