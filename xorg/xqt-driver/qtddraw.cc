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

#include <qapplication.h>
#ifdef	QWS
#include <qdirectpainter_qws.h>
#endif
#include <qimage.h>

#include "xqtcpp.h"
#include "qtddraw.h"

qtShadow::qtShadow(void) 
{
	fbBase = NULL;
	shaBase = NULL;

	shaRandr = 0;
	scrRandr = 0;
}

void qtShadow::setFbSize(int w, int h)
{
//	qDebug("setFbSize(%d:%d)", w, h);

	fbWidth = w;
	fbHeight = h;
}

void qtShadow::setFbParam(void *base, int bpl)
{
	fbBase = (unsigned char *)base;
	fbBpl = bpl;
}

void qtShadow::setShaParam(void *base, int bpl, int bpp, int randr)
{
	shaBase = (unsigned char *)base;
	shaBpl = bpl;
	shaBpp = bpp;

	switch (randr) {
	case RR_0:
		shaRandr = 0;
		break;

	case RR_90:
		shaRandr = 90;
		break;

	case RR_180:
		shaRandr = 180;
		break;

	case RR_270:
		shaRandr = 270;
		break;
	}
}

//
// rotate
//
void qtShadow::rot_x2fb(int rot, int *x, int *y, int *w, int *h)
{
	while (rot < 0) {
		rot += 360;
	}
	while (rot >= 360) {
		rot -= 360;
	}

	int xx, yy, ww, hh;
	int dummy = 1;

	if (!w || !h) {	w = &dummy; h = &dummy; }

	switch (rot) {
	case 0:
		/* do nothing */
		return;

#if 0
	case 90:
		xx = *y;
		yy = fbHeight - *x - *w;
		ww = *h;
		hh = *w;
		break;
#endif

	case 270:
		xx = fbWidth - *y - *h;
		yy = *x;
		ww = *h;
		hh = *w;
		break;

	default:
		qWarning("rot_x2fb failed");
		break;
	}
	*x = xx; *y = yy; *w = ww; *h = hh;
}

void qtShadow::rot_fb2x(int rot, int *x, int *y, int *w, int *h)
{
	while (rot < 0) {
		rot += 360;
	}
	while (rot >= 360) {
		rot -= 360;
	}

	int xx, yy, ww, hh;
	int dummy = 1;

	if (!w || !h) {	w = &dummy; h = &dummy; }

	//qDebug("rot_fb2x(%d:(%d:%d:%d:%d):%d) -> ", rot, *x, *y, *w, *h, fbWidth);

	switch (rot) {
	case 0:
		/* do nothing */
		return;

	case 270:
		xx = *y;
		yy = fbWidth - *x - *w;
		ww = *h;
		hh = *w;
		break;

	default:
		qWarning("rot_fb2x failed");
		break;
	}
	*x = xx; *y = yy; *w = ww; *h = hh;

	//qDebug("  -> (%d:%d:%d:%d)", *x, *y, *w, *h);
}


void qtShadow::sha2fb(int *x, int *y, int *w, int *h)
{
	rot_x2fb(shaRandr, x, y, w, h);
}

void qtShadow::fb2sha(int *x, int *y, int *w, int *h)
{
	rot_fb2x(shaRandr, x, y, w, h);
}

void qtShadow::scr2fb(int *x, int *y, int *w, int *h)
{
	rot_x2fb(scrRandr, x, y, w, h);
}

void qtShadow::fb2scr(int *x, int *y, int *w, int *h)
{
	rot_fb2x(scrRandr, x, y, w, h);
}

#ifdef QWS
void qtShadow::transfer(QDirectPainter *p)
{
	int n = p->numRects();

	setFbParam(p->frameBuffer(), p->lineStep());

	for (int i = 0; i < n; i++) {
		const QRect &clip = p->rect(i);
		transfer(clip.left(), clip.top(), clip.width(), clip.height());
	}
}
#endif

void qtShadow::transfer(int x, int y, int w, int h)
{
	if (!fbBase || !shaBase) return;
	//qDebug("qtShadow::transfer(%d,%d,%d,%d)", x, y, w, h);

	switch (shaRandr) {
	case 0:
		transfer0(x, y, w, h);
		break;
		
	case 270:
		transfer3(x, y, w, h);
		break;

	default:
		/* ### not yet */
		break;
	}
}

void qtShadow::transfer0(int x, int y, int w, int h)
{
	int bypp = shaBpp / 8;

	uchar *fb = fbBase  + x * bypp + y * fbBpl;
	uchar *sb = shaBase + x * bypp + y * shaBpl;

	w *= bypp;
	while (h--) {
		memcpy(fb, sb, w);
		fb += fbBpl;
		sb += shaBpl;
	}
}

// 270 rotation : SL-C7xx, 8xx, 5500 etc.
void qtShadow::transfer3(int x, int y, int w, int h)
{
	int bypp = shaBpp / 8;

	uchar *fb = fbBase + (x + w - 1) * bypp + y * fbBpl;
	uchar *sb = shaBase + y * bypp + (fbWidth - x - w) * shaBpl;

	uchar *fb_next;
	uchar *sb_next;

	while (w--) {
		fb_next = fb - bypp;
		sb_next = sb + shaBpl;

		int hh = h;

/* ad hoc... */
#ifdef QWS
#define	DRAW1	*(ushort *)fb = *(ushort *)sb; fb += fbBpl; sb += bypp
#else
#define	DRAW1	*(ulong *)fb = *(ulong *)sb; fb += fbBpl; sb += bypp
#endif

#define DRAW4	DRAW1; DRAW1; DRAW1; DRAW1;
#define DRAW16	DRAW4; DRAW4; DRAW4; DRAW4;

		while (hh >= 16) {
			DRAW16;
			hh -= 16;
		}
		while (hh--) {
			DRAW1;
		}

		sb = sb_next;
		fb = fb_next;
	}
}
