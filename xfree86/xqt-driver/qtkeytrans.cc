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

void qtTranslateKey(QKeyEvent *ev, int fPress)
{
	int qtkey = ev->key();
	int scancode = -1;
	int modifier;

	for (int i = 0; i < sizeof(qtKeyScanMap)/sizeof(int); i++) {
		if (qtKeyScanMap[i] == qtkey) {
			scancode = i + MIN_KEYCODE;
			break;
		}
	}
	qDebug("key:%x, mod:%x -> key:%x, mod:%x\n", ev->key(), ev->state(),
	       scancode, modifier);

	if (scancode < 0) return;

	qtSendKeyEvent(scancode, fPress);
	qtPushMouseKeyEvent();
}
