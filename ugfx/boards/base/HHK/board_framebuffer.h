/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.io/license.html
 */

// Set this to your frame buffer pixel format.
#ifndef GDISP_LLD_PIXELFORMAT
	#define GDISP_LLD_PIXELFORMAT		GDISP_PIXELFORMAT_RGB565
#endif

// Uncomment this if your frame buffer device requires flushing
#define GDISP_HARDWARE_FLUSH		GFXON

#ifdef GDISP_DRIVER_VMT

	#include <sdk/calc/calc.h>
	#include <sdk/os/lcd.h>

	static void board_init(GDisplay *g, fbInfo *fbi) {
		// TODO: Initialize your frame buffer device here

		// TODO: Set the details of the frame buffer
		g->g.Width = width;
		g->g.Height = height;
		g->g.Backlight = 100;
		g->g.Contrast = 50;
		fbi->linelen = g->g.Width * sizeof(LLDCOLOR_TYPE);				// bytes per row
		fbi->pixels = vram;												// pointer to the memory frame buffer
	}

	#if GDISP_HARDWARE_FLUSH
		static void board_flush(GDisplay *g) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
			LCD_Refresh();
		}
	#endif

	#if GDISP_NEED_CONTROL
		static void board_backlight(GDisplay *g, gU8 percent) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
			(void) percent;
		}

		static void board_contrast(GDisplay *g, gU8 percent) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
			(void) percent;
		}

		static void board_power(GDisplay *g, gPowermode pwr) {
			// TODO: Can be an empty function if your hardware doesn't support this
			(void) g;
			(void) pwr;
		}
	#endif

#endif /* GDISP_LLD_BOARD_IMPLEMENTATION */
