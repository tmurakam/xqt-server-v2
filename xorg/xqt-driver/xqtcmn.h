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

#ifndef NO
#define NO					0
#endif
#ifndef YES
#define YES					1
#endif

#define	QTDEBUG					NO

typedef	unsigned long	DWORD;

/*
 * Qt Pointer
 */
typedef void	*QWidgetP;
typedef	void	*QPainterP;
typedef	void	*QImageP;
typedef	void	*QRegionP;

/*
 * global variables
 */
#ifdef	__cplusplus
typedef	bool	Bool;
#endif

extern Bool			g_fRootless;
extern Bool			g_fMultiWindow;
extern Bool			g_fFullScreen;

/*
 * C linkage functions
 */
#ifdef	__cplusplus
extern "C" {
#endif

	/*
	 * Qt side
	 */
	void qtApInit(int argc, char *argv[]);
	void qtApProcessEvents(void);
	void qtShowFatalError(const char *msg);

	QImageP qtCreateShadowImage(DWORD *w, DWORD *h, int *depth, 
				    void **bits, DWORD *stride);
	void qtRedrawScreen(int x, int y, int w, int h, void *base, int stride, int bpp, int randr);

	void qtSetWindowRgn(QRegionP rgn);
	
	void *qtBufPos(int row, int offset, int *size);

	QRegionP qtCreateRectRgn(int x, int y, int w, int h);
	void qtCombineRgnAnd(QRegionP r, QRegionP r2);
	void qtCombineRgnOr(QRegionP r, QRegionP r2);
	void qtOffsetRgn(QRegionP rgn, int dx, int dy);
	void qtSetWindowRegion(QRegionP rgn);
	void qtDeleteRgn(QRegionP rgn);

	/*
	 * X side
	 */
	void qtStartAp(int argc, char *argv[]);
	void qtMouseMotionSendEvent(int x, int y);
	void qtMouseButtonsSendEvent (int iEventType, int iButton);
	void qtPushMouseKeyEvent(void);
	void qtSendKeyEvent (DWORD dwKey, int fDown);
	void qtSendKeyMbText(const char *text);
	void qtRandRRotate(int rot);

#ifdef __cplusplus
}
#endif

