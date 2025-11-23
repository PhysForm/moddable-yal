#include "gfx.h"

#if GINPUT_NEED_KEYBOARD

    #include <stddef.h>
    #include <limits.h>
    #include <string.h>
    #include "capture_events.h"

    #define GKEYBOARD_DRIVER_VMT        GKEYBOARDCMT_HHK
    #include "../../../src/ginput/ginput_driver_keyboard.h"

	static gBool HHK_KeyboardInit(GKeyboard *k, unsigned driverinstance);
	static int HHK_KeyboardGetData(GKeyboard *k, gU8 *pch, int sz);

    const GKeyboardVMT GKEYBOARD_DRIVER_VMT[1] = {{
		{
			GDRIVER_TYPE_KEYBOARD,
			0,
			sizeof(GKeyboard),
			_gkeyboardInitDriver, _gkeyboardPostInitDriver, _gkeyboardDeInitDriver
		},
		0,
		HHK_KeyboardInit,			// init
		0,						// deinit
		HHK_KeyboardGetData,        // getdata
		0						// putdata		void	(*putdata)(GKeyboard *k, char ch);		Optional
	}};
    
	static gBool HHK_KeyboardInit(GKeyboard *k, unsigned driverinstance) {
        (void) k;
        (void) driverinstance;

        return gTrue;
    }

	static int HHK_KeyboardGetData(GKeyboard *k, gU8 *pch, int sz) {
        if (sz <= 0) return 0;
        CaptureEvents();
        k->keystate = keys_list[0].state;
        size_t i;
        for (i = 0; i < INT_MAX && (int)i < sz && i < keys_list_length && k->keystate == keys_list[i].state; i++)
            pch[i] = keys_list[i].chr;
        keys_list_length -= i;
        memmove(keys_list, keys_list + i, keys_list_length * sizeof(*keys_list));
        return i;
    }

#endif /* GINPUT_NEED_KEYBOARD */
