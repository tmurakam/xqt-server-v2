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
 * show keymap info
 */

#define	Wiki	1

main()
{
#define	K(qtkey, ks1, ks2)	/**/
#define	KD(qtkey, ks1, ks2)	/**/
#include "KeyMap.h"
	int scancode = MIN_KEYCODE;

	printf("scan    Qt::Key              KeySym               KeySym(Shift)\n");
	printf("---------------------------------------------------------------------\n");

#undef K
#undef KD
#if Wiki
#define	K(qtkey, ks1, ks2)	printf("|%02x|%s|%s|%s|\n", scancode, "Qt::Key_" #qtkey, #ks1, #ks2); scancode++;
#define	KD(qtkey, ks1, ks2)	printf("|%02x|%s|%s|%s|\n", scancode, #qtkey, #ks1, #ks2); scancode++;
#else
#define	K(qtkey, ks1, ks2)	printf("%02x      %-20s %-20s %s\n", scancode, "Qt::Key_" #qtkey, #ks1, #ks2); scancode++;
#define	KD(qtkey, ks1, ks2)	printf("%02x      %-20s %-20s %s\n", scancode, #qtkey, #ks1, #ks2); scancode++;
#endif

#include "KeyMap.h"
}

