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

#include <qwidget.h>
#include <qpopupmenu.h>
#include <qevent.h>

#include "xqtcmn.h"
#include "qtddraw.h"

/*
 * Screen GUI class
 */
class qtScreen : public QWidget
{
Q_OBJECT
private:
#ifndef QWS
	QImage	*m_img;		// shadow image
#endif
	bool	isFullScreen;	// is full screen mode?

	QSize  	scrSize;	// current screen size
	QSize  	fullScrSize;	// full screen size
	QSize  	normalScrSize;	// normal screen size (without task bar)
	QSize  	desktopSize;	// physical desktop size

	qtShadow shadow;
	
	QPopupMenu *menu;

	bool	ctrlOn, shiftOn;	// key state

	void setDisplaySize(void);
	void resetScreenPos(void);
	void checkScreenSizeRotation(void);
	
public:
	qtScreen(QWidget *parent = 0, const char *name = 0, WFlags f = 0);
	QImage *CreateShadowImage(DWORD *w, DWORD *h, int *depth, void **bits, DWORD *stride);
	void *WindowLinear(int row, int offset, int *size);

	void RedrawScreen(int x, int y, int w, int h, 
			  void *base, int stride, int bpp, int randr);


	//inline int depth(void) { return m_pixmap->depth(); }
	//inlien QPixmap *pixmap(void) { return m_pixmap; }

public slots:
	void quit(void);
	void toggleFullScreen(void);

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void paintEvent(QPaintEvent *);
	void mouseButtonEvent(QMouseEvent *ev, int type);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void keyPressEvent(QKeyEvent *ev);
	virtual void keyReleaseEvent(QKeyEvent *ev);
	virtual void moveEvent(QMoveEvent *ev);
	virtual void resizeEvent(QResizeEvent *ev);
	virtual void focusInEvent(QFocusEvent *ev);
};

extern qtScreen *g_screen;
