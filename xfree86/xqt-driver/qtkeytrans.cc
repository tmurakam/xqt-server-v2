/*
 *Copyright (C) 2004 The X/Qt Server Project. All Rights Reserved.
 *
 *Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 *"Software"), to deal in the Software without restriction, including
 *without limitation the rights to use, copy, modify, merge, publish,
 *distribute, sublicense, and/or sell copies of the Software, and to
 *permit persons to whom the Software is furnished to do so, subject to
 *the following conditions:
 *
 *The above copyright notice and this permission notice shall be
 *included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE XFREE86 PROJECT BE LIABLE FOR
 *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 *CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *Except as contained in this notice, the name of the X/Qt Server Project
 *shall not be used in advertising or otherwise to promote the sale, use
 *or other dealings in this Software without prior written authorization
 *from the XFree86 Project.
 *
 * Authors:	Takuya Murakami
 */
/*
 * Qt::Key code to scancode converter
 */

#include <qnamespace.h>
#include <qevent.h>

#include "xqtcmn.h"
#include "qtkeytrans.h"

#include "keysym.h"

/* Qt::Key to scancode map structure */
typedef struct
{
	int		qtkey;
	int		scancode;
	int		modifier;
} qtKeyScanMapRec;

#define	SHIFT		0x1
#define	NOSHIFT		0x2
#define	CTRL		0x4
#define	NOCTRL		0x8

#define	MIN_KEYCODE	0

#define	SCAN_CTRL	0x1d + MIN_KEYCODE
#define	SCAN_ALT	0x38 + MIN_KEYCODE
#define	SCAN_SHIFT	0x2a + MIN_KEYCODE

/* 
 * scancode map (emulates jp106 keyboard). see qtkeymap.h also.
 */
static const qtKeyScanMapRec
qtKeyScanMap[] = {
/* keyname              Qt::Key                 scan	modifier */
/* Escape     */	{Qt::Key_Escape,	0x01,	0},
/* 1          */	{Qt::Key_1,		0x02,	0},
/* 2          */	{Qt::Key_2,		0x03,	0},
/* 3          */	{Qt::Key_3,		0x04,	0},
/* 4          */	{Qt::Key_4,		0x05,	0},
/* 5          */	{Qt::Key_5,		0x06,	0},
/* 6          */	{Qt::Key_6,		0x07,	0},
/* 7          */	{Qt::Key_7,		0x08,	0},
/* 8          */	{Qt::Key_8,		0x09,	0},
/* 9          */	{Qt::Key_9,		0x0a,	0},
/* 0          */	{Qt::Key_0,		0x0b,	0},
/* !          */	{Qt::Key_Exclam,	0x02,	0},
/* "          */	{Qt::Key_QuoteDbl,	0x03,	0},
/* #          */	{Qt::Key_NumberSign,	0x04,	0},
/* $          */	{Qt::Key_Dollar,	0x05,	0},
/* %          */	{Qt::Key_Percent,	0x06,	0},
/* &          */	{Qt::Key_Ampersand,	0x07,	0},
/* '          */	{Qt::Key_Apostrophe,	0x08,	0},
/* (          */	{Qt::Key_ParenLeft,	0x09,	0},
/* )          */	{Qt::Key_ParenRight,	0x0a,	0},
/* -          */	{Qt::Key_Minus,		0x0c,	0},
/* =          */	{Qt::Key_Equal,		0x0c,	SHIFT|NOCTRL},
/* ^          */	{Qt::Key_AsciiCircum,	0x0d,	NOCTRL},
/* ~          */	{Qt::Key_AsciiTilde,	0x0d,	SHIFT|NOCTRL},
/* BackSpace  */	{Qt::Key_BackSpace,	0x0e,	0},
/* Tab        */	{Qt::Key_Tab,		0x0f,	0},
/* Q          */	{Qt::Key_Q,		0x10,	0},
/* W          */	{Qt::Key_W,		0x11,	0},
/* E          */	{Qt::Key_E,		0x12,	0},
/* R          */	{Qt::Key_R,		0x13,	0},
/* T          */	{Qt::Key_T,		0x14,	0},
/* Y          */	{Qt::Key_Y,		0x15,	0},
/* U          */	{Qt::Key_U,		0x16,	0},
/* I          */	{Qt::Key_I,		0x17,	0},
/* O          */	{Qt::Key_O,		0x18,	0},
/* P          */	{Qt::Key_P,		0x19,	0},
/* @          */	{Qt::Key_At,		0x1a,	NOCTRL},
/* `          */	{Qt::Key_QuoteLeft,	0x1a,	SHIFT|NOCTRL},
/* [          */	{Qt::Key_BracketLeft,	0x1b,	NOCTRL},
/* {          */	{Qt::Key_BraceLeft,	0x1b,	SHIFT|NOCTRL},
/* Return     */	{Qt::Key_Return,	0x1c,	0},
/* OK         */	{Qt::Key_F33,		0x1c,	0},
/* Control    */	{Qt::Key_Control,	0x1d,	0},
/* A          */	{Qt::Key_A,		0x1e,	0},
/* S          */	{Qt::Key_S,		0x1f,	0},
/* D          */	{Qt::Key_D,		0x20,	0},
/* F          */	{Qt::Key_F,		0x21,	0},
/* G          */	{Qt::Key_G,		0x22,	0},
/* H          */	{Qt::Key_H,		0x23,	0},
/* J          */	{Qt::Key_J,		0x24,	0},
/* K          */	{Qt::Key_K,		0x25,	0},
/* L          */	{Qt::Key_L,		0x26,	0},
/* ;          */	{Qt::Key_Semicolon,	0x27,	NOCTRL},
/* +          */	{Qt::Key_Plus,		0x27,	SHIFT|NOCTRL},
/* :          */	{Qt::Key_Colon,		0x28,	NOCTRL},
/* *          */	{Qt::Key_Asterisk,	0x28,	SHIFT|NOCTRL},
/* ZenHan     */	{Qt::Key_F21,		0x29,	0},
/* Shift      */	{Qt::Key_Shift,		0x2a,	0},
/* ]          */	{Qt::Key_BracketRight,	0x2b,	NOCTRL},
/* },          */	{Qt::Key_BraceRight,	0x2b,	SHIFT|NOCTRL},
/* Z          */	{Qt::Key_Z,		0x2c,	0},
/* X          */	{Qt::Key_X,		0x2d,	0},
/* C          */	{Qt::Key_C,		0x2e,	0},
/* V          */	{Qt::Key_V,		0x2f,	0},
/* B          */	{Qt::Key_B,		0x30,	0},
/* N          */	{Qt::Key_N,		0x31,	0},
/* M          */	{Qt::Key_M,		0x32,	0},
/* comma      */	{Qt::Key_Comma,		0x33,	NOSHIFT},
/* <          */	{Qt::Key_Less,		0x33,	SHIFT},
/* .          */	{Qt::Key_Period,	0x34,	NOSHIFT},
/* >          */	{Qt::Key_Greater,	0x34,	SHIFT},
/* /          */	{Qt::Key_Slash,		0x35,	NOSHIFT},
/* ?          */	{Qt::Key_Question,	0x35,	SHIFT},
/* Alt        */	{Qt::Key_Alt,		0x38,	0},
/* Space      */	{Qt::Key_Space,		0x39,	0},
/* Caps_Lock  */	{Qt::Key_CapsLock,	0x3a,	NOCTRL},
/* F1         */	{Qt::Key_F1,		0x3b,	0},
/* F2         */	{Qt::Key_F2,		0x3c,	0},
/* F3         */	{Qt::Key_F3,		0x3d,	0},
/* F4         */	{Qt::Key_F4,		0x3e,	0},
/* F5         */	{Qt::Key_F5,		0x3f,	0},
/* F6         */	{Qt::Key_F6,		0x40,	0},
/* F7         */	{Qt::Key_F7,		0x41,	0},
/* F8         */	{Qt::Key_F8,		0x42,	0},
/* F9         */	{Qt::Key_F9,		0x43,	0},
/* NumLock    */	{Qt::Key_NumLock,	0x45,	0},
/* ScrollLock */	{Qt::Key_ScrollLock,	0x46,	0},
/* F11        */	{Qt::Key_F11,		0x57,	0},
/* Up         */	{Qt::Key_Up,		0x5a,	0},
/* Prior      */	{Qt::Key_Prior,		0x5b,	0},
/* Left       */	{Qt::Key_Left,		0x5c,	0},
/* Begin      */	/*{Qt::Key_Begin,		0x5d,	0},*/
/* Right      */	{Qt::Key_Right,		0x5e,	0},
/* End        */	{Qt::Key_End,		0x5f,	0},
/* Down       */	{Qt::Key_Down,		0x60,	0},
/* Next       */	{Qt::Key_Next,		0x61,	0},
/* Insert     */	{Qt::Key_Insert,	0x62,	0},
/* Delete     */	{Qt::Key_Delete,	0x63,	NOCTRL},
/* Pause      */	{Qt::Key_Pause,		0x66,	0},
/* Print      */	{Qt::Key_Print,		0x67,	0},
/* Break      */	/*{Qt::Key_Break,		0x6a,	0},*/
/* Meta       */	{Qt::Key_Meta,		0x6b,	0},
/* F14        */	{Qt::Key_F14,		0x6f,	0},
/* HiraKata   */	{Qt::Key_F15,		0x70,	0},
/* F16        */	{Qt::Key_F16,		0x71,	0},
/* F17        */	{Qt::Key_F17,		0x72,	0},
/* \          */	{Qt::Key_Backslash,	0x73,	NOCTRL},
/* _          */	{Qt::Key_Underscore,	0x73,	SHIFT|NOCTRL},
/* \          */	/*{Qt::Key_Backslash,	0x7d,	NOCTRL},*/
/* |          */	{Qt::Key_Bar,		0x7d,	SHIFT|NOCTRL},

// --- Sharp SL-Cxxx series ---
/* F10(Addr.) */	{Qt::Key_F10,		0x1d,	0},	// ctrl
/* F12(Home)  */	{Qt::Key_F12,		0x38,	0},	// alt
/* F13(Mail)  */	{Qt::Key_F13,		0x38,	0},	// alt (does not work...)
/* \          */	{0x2022,		0x73,	NOCTRL},
/* Ctrl+1     */	{0x200f,		0x02,	CTRL},	
/* Ctrl+2     */	{0x2010,		0x03,	CTRL},	
/* Ctrl+3     */	{0x2012,		0x04,	CTRL},	
/* Ctrl+4     */	{0x2011,		0x05,	CTRL},	
/* Ctrl+5     */	{0x201b,		0x06,	CTRL},	
/* Ctrl+6     */	{0x200a,		0x07,	CTRL},	
/* Ctrl+7     */	{0x200b,		0x08,	CTRL},	
/* Ctrl+8     */	{0x200c,		0x09,	CTRL},	
/* Ctrl+9     */	{0x2007,		0x0a,	CTRL},	
/* Ctrl+0     */	{0x2008,		0x0b,	CTRL},	
/* Ctrl+Q     */	{0x2014,		0x10,	CTRL},	
/* Ctrl+O     */	{0x2015,		0x18,	CTRL},	
/* Ctrl+P     */	{0x2016,		0x19,	CTRL},	
/* Ctrl+A     */	{0x2017,		0x1e,	CTRL},	
/* Ctrl+S     */	{0x2018,		0x1f,	CTRL},	
/* Ctrl+D     */	{0x00a5,		0x20,	CTRL},	
/* Ctrl+N     */	{0x2019,		0x31,	CTRL},	
/* Ctrl+M     */	{0x201a,		0x32,	CTRL},	
};

void qtTranslateKey(QKeyEvent *ev, int fPress)
{
	int qtkey = ev->key();
	int scancode = -1;
	int modifier;

	for (int i = 0; i < sizeof(qtKeyScanMap)/sizeof(qtKeyScanMap[0]); i++) {
		if (qtKeyScanMap[i].qtkey == qtkey) {
			scancode = qtKeyScanMap[i].scancode;
			modifier = qtKeyScanMap[i].modifier;
			break;
		}
	}
//	qDebug("key:%x, mod:%x -> key:%x, mod:%x\n", ev->key(), ev->state(),
//	       scancode, modifier);

	if (scancode < 0) return;

	int bs = ev->state();

	// Shift key handling
	if ((modifier & SHIFT) && !(bs & Qt::ShiftButton)) {
		qtSendKeyEvent(SCAN_SHIFT, TRUE);
	}
	else if ((modifier & NOSHIFT) && (bs & Qt::ShiftButton)) {
		qtSendKeyEvent(SCAN_SHIFT, FALSE);
	}

#ifdef	Zaurus
	if (modifier & NOCTRL) {
		bs &= ~Qt::ControlButton;
	}
#endif
	if ((bs & Qt::ControlButton) || (modifier & CTRL)) {
		qtSendKeyEvent(SCAN_CTRL, TRUE);
	}
	if (bs & Qt::AltButton) {
		qtSendKeyEvent(SCAN_ALT, TRUE);
	}

	qtSendKeyEvent(scancode + MIN_KEYCODE, fPress);

	if ((bs & Qt::ControlButton) || (modifier & CTRL)) {
		qtSendKeyEvent(SCAN_CTRL, FALSE);
	}
	if (bs & Qt::AltButton) {
		qtSendKeyEvent(SCAN_ALT, FALSE);
	}

	if ((modifier & SHIFT) && !(bs & Qt::ShiftButton)) {
		qtSendKeyEvent(SCAN_SHIFT, FALSE);
	}
	else if ((modifier & NOSHIFT) && (bs & Qt::ShiftButton)) {
		qtSendKeyEvent(SCAN_SHIFT, TRUE);
	}

	qtPushMouseKeyEvent();
}

