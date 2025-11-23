#include "capture_events.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sdk/os/input.h>

typeof(touch) touch = { 0, 0, 0 };
typeof(keys_list) keys_list;
typeof(keys_list_length) keys_list_length = 0;

static size_t keys_list_reserved = 0;
static gBool shift = gFalse;
static gBool keyboard = gFalse;

#if GINPUT_NEED_KEYBOARD
#include "../../../src/ginput/ginput_keyboard.h"
void deinit_list() {
    free(keys_list);
}

static void HandleKeyEvent(const struct Input_Event * event) {
    if (event->data.key.keyCode == KEYCODE_SHIFT) {
        shift = event->data.key.direction != KEY_RELEASED ? gTrue : gFalse;
        return;
    }
    if (event->data.key.keyCode == KEYCODE_KEYBOARD) {
        keyboard = event->data.key.direction != KEY_RELEASED ? gTrue : gFalse;
    }

    if (keys_list_reserved < ++keys_list_length) {
        if (keys_list_reserved == 0) {
            keys_list = malloc(sizeof(*keys_list) * 10);
            atexit(deinit_list);
            keys_list_reserved = 10;
        } else {
            keys_list_reserved *= 1.5;
            keys_list = realloc(keys_list, sizeof(*keys_list) * keys_list_reserved);
        }
    }

    typeof(keys_list) entry = keys_list + keys_list_length - 1;
    entry->state = 0;
    entry->chr = 0xFE;

    if (shift == gTrue) entry->state |= GKEYSTATE_SHIFT;
    if (keyboard == gTrue) entry->state |= GKEYSTATE_DRIVER_FIRST;
    switch (event->data.key.direction) {
        case KEY_RELEASED:
            entry->state |= GKEYSTATE_KEYUP;
            break;
        case KEY_HELD:
            entry->state |= GKEYSTATE_REPEAT;
            break;
        case KEY_PRESSED:
            break;
    }
    
    #define C(k, c) case k: entry->chr = c; break;
    switch (event->data.key.keyCode) {
        C(KEYCODE_KEYBOARD, GKEY_FN1)
        C(KEYCODE_SHIFT, 0xFD)
        C(KEYCODE_0, '0')
        C(KEYCODE_1, '1')
        C(KEYCODE_2, '2')
        C(KEYCODE_3, '3')
        C(KEYCODE_4, '4')
        C(KEYCODE_5, '5')
        C(KEYCODE_6, '6')
        C(KEYCODE_7, '7')
        C(KEYCODE_8, '8')
        C(KEYCODE_9, '9')
        C(KEYCODE_BACKSPACE, GKEY_BACKSPACE)
        C(KEYCODE_CLOSE_PARENTHESIS, ')')
        C(KEYCODE_OPEN_PARENTHESIS, '(')
        C(KEYCODE_COMMA, ',')
        C(KEYCODE_DIVIDE, '/')
        C(KEYCODE_DOT, '.')
        C(KEYCODE_DOWN, GKEY_DOWN)
        C(KEYCODE_EQUALS, '=')
        C(KEYCODE_EXE, GKEY_ENTER)
        C(KEYCODE_EXP, GKEY_FN3)
        C(KEYCODE_LEFT, GKEY_LEFT)
        C(KEYCODE_MINUS, '-')
        C(KEYCODE_NEGATIVE, GKEY_FN2)
        C(KEYCODE_PLUS, '+')
        C(KEYCODE_POWER, '^')
        C(KEYCODE_POWER_CLEAR, GKEY_ESC)
        C(KEYCODE_RIGHT, GKEY_RIGHT)
        C(KEYCODE_TIMES, '*')
        C(KEYCODE_UP, GKEY_UP)
        C(KEYCODE_X, 'x')
        C(KEYCODE_Y, 'y')
        C(KEYCODE_Z, 'z')
    }
    #undef C
    if (entry->chr > 0x80) entry->state |= GKEYSTATE_SPECIAL;
}
#else
static void HandleKeyEvent(const struct Input_Event * event) {
    (void)keys_list_reserved;
    (void)shift;
    (void)keyboard;
    (void)event;
}
#endif

void CaptureEvents() {
    struct Input_Event event;
    memset(&event, 0, sizeof(event));
    GetInput(&event, 0, 0);
    switch (event.type) {
        case EVENT_TOUCH:
            touch.z = (event.data.touch_single.direction & (TOUCH_DOWN | TOUCH_HOLD_DRAG)) != 0 ? 1 : 0;
            touch.x = event.data.touch_single.p1_x;
            touch.y = event.data.touch_single.p1_y;
            break;
        case EVENT_KEY:
            HandleKeyEvent(&event);
            break;
        default:
            break;
    }
}
