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
#include <qregion.h>

#include "xqtcmn.h"
#include "xqtcpp.h"

extern "C"
QRegionP qtCreateRectRgn(int x, int y, int width, int height)
{
	QRegion *rgn = new QRegion(x, y, width, height);
	return (QRegionP)rgn;
}

extern "C"
void qtCombineRgnAnd(QRegionP rgnp, QRegionP rgn2p)
{
	QRegion *rgn  = (QRegion *)rgnp;
	QRegion *rgn2 = (QRegion *)rgn2p;

	*rgn &= *rgn2;
}

extern "C"
void qtCombineRgnOr(QRegionP rgnp, QRegionP rgn2p)
{
	QRegion *rgn  = (QRegion *)rgnp;
	QRegion *rgn2 = (QRegion *)rgn2p;

	*rgn |= *rgn2;
}

extern "C"
void qtOffsetRgn(QRegionP rgn, int dx, int dy)
{
	QRegion *r = (QRegion *)rgn;
	r->translate(dx, dy);
}

extern "C"
void qtDeleteRgn(QRegionP rgn)
{
	QRegion *r = (QRegion *)rgn;
	delete(r);
}

