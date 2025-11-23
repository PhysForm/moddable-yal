#pragma once

#include "gfx.h"

extern struct {
    gCoord x, y, z;
} touch;

extern struct {
    gU8 chr;
    gU32 state;
} *keys_list;

extern size_t keys_list_length;

void CaptureEvents();
