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

/*
 * Define Qt:Key_ to internal scan code map
 */
#define	K(qtkey, ks1, ks2)	Qt::Key_ ## qtkey,
#define	KD(qtkey, ks1, ks2)	qtkey,

static const int qtKeyScanMap[] = {
#include "KeyMap.h"
};

static int qtkey2scan(int qtkey)
{
	int scancode = -1;
	int i;

	for (int i = 0; i < sizeof(qtKeyScanMap)/sizeof(int); i++) {
		if (qtKeyScanMap[i] == qtkey) {
			scancode = i + MIN_KEYCODE;
			break;
		}
	}
	return scancode;
}

void qtTranslateKey(QKeyEvent *ev, int fPress)
{
	static const int scan_control = qtkey2scan(Qt::Key_Control);
	static const int scan_alt     = qtkey2scan(Qt::Key_Alt);

	int qtkey = ev->key();
	int bs = ev->state();

	int scancode = qtkey2scan(qtkey);
	qDebug("key:%x, mod:%x -> key:%x\n", qtkey, bs, scancode);

	if (scancode < 0) return;

	//
	// KeyHelper 対応
	//
	// Control / Alt modifier を処理する
	//
	if (fPress) {
		if (bs & Qt::ControlButton) qtSendKeyEvent(scan_control, true);
		if (bs & Qt::AltButton) qtSendKeyEvent(scan_alt, true);
	}

	qtSendKeyEvent(scancode, fPress);

	if (!fPress) {
		if (bs & Qt::AltButton) qtSendKeyEvent(scan_alt, false);
		if (bs & Qt::ControlButton) qtSendKeyEvent(scan_control, false);
	}

	qtPushMouseKeyEvent();
}
