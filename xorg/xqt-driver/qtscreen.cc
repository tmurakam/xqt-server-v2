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
#include <qpe/qpeapplication.h>
#include <qdirectpainter_qws.h>
#endif
#include <qimage.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qtextcodec.h>

#include <unistd.h>

#include "xqtcpp.h"
#include "qtscreen.h"
#include "qtapplication.h"
#include "qtkeytrans.h"

qtScreen *g_screen;

#ifndef QWS
#define DEBUG_SCR	/* screen debug mode for PC... */
#endif

//
// Constructor
//
qtScreen::qtScreen(QWidget *parent, const char *name, WFlags f)
	: QWidget(parent, name, f)
{
#ifndef QWS
	m_img = NULL;
#endif
	ctrlOn = false;
	shiftOn = false;
	isFullScreen = false;

	setCaption( tr("X/Qt Server") );
	setBackgroundMode( NoBackground );
	setFocusPolicy(QWidget::StrongFocus);
	setMouseTracking(true);
	setCursor(QCursor(BlankCursor));

#ifdef QWS
	QPEApplication::grabKeyboard();
#endif

	checkScreenSizeRotation();

	// create popup menu
	menu = new QPopupMenu;
#ifdef QWS
	if (g_fFullScreen) {
		menu->insertItem("&FullScreen", this, SLOT( toggleFullScreen() ));
	}
	menu->insertItem("&Help", g_ap, SLOT( showHelp() ));
#endif
	menu->insertItem("&Shutdown", this, SLOT( quit() ));
}

void qtScreen::quit(void)
{
	exit(0);
}

void qtScreen::closeEvent(QCloseEvent *e)
{
	quit();
}


void qtScreen::checkScreenSizeRotation(void)
{
	QSize newSize = QApplication::desktop()->size();
	if (newSize == desktopSize) {
		return;	// do nothing
	}

	desktopSize = newSize;
	fullScrSize = desktopSize;
	normalScrSize = fullScrSize;
	if (normalScrSize.height() >= 400) {
		normalScrSize.setHeight( normalScrSize.height() - 38);
	} else {
		normalScrSize.setHeight( normalScrSize.height() - 20);
	}

	if (!g_fFullScreen) {
		fullScrSize = normalScrSize;
	}

#ifdef DEBUG_SCR
	normalScrSize.setHeight(640);
	normalScrSize.setWidth(480);
	fullScrSize = normalScrSize;
#endif

	setDisplaySize();

#ifdef QWS
	// zaurus hack
	if (desktopSize.width() > desktopSize.height()) {
		shadow.setScrRandr(270);
		qtRandRRotate(270);
	} else {
		shadow.setScrRandr(0);
		qtRandRRotate(0);
	}
#endif
}	

//
// Create shadow buffer
//
QImage *qtScreen::CreateShadowImage(DWORD *w, DWORD *h, int *depth, void **bits, DWORD *stride)
{
	*w = fullScrSize.width();
	*h = fullScrSize.height();

#ifdef QWS
	// zaurus hack
	if (*w > *h) {
		// rotate!
		int sw = *w;
		*w = *h;
		*h = sw;
	}

	*depth = 16;	// 16bpp 固定
	*bits = NULL;
	*stride = *w;

	qDebug("create shadow image:%dx%d\n", *w, *h);

	shadow.setFbSize(*w, *h);
	return NULL;
#else
	if (m_img == NULL) {	// Server reset しても作るのは一回だけ

	  	m_img = new QImage;

		// 最初は 16bpp で試す (Qt/Embedded のみ)
		qWarning("Try 16bpp");
		*depth = 16;
		if (!m_img->create(*w, *h, *depth)) {
			// 次に 32bpp で試す
			qWarning("Try 32bpp");
			*depth = 32;
			if (!m_img->create(*w, *h, *depth)) {
				qWarning("Could not create shadow image");
				delete m_img;
				return NULL;
			}
		}
	}

	*bits = (void *)m_img->bits();
	if (*bits == NULL) {
		qWarning("Could not create shadow image");
		delete m_img;
		return NULL;
	}

	*stride = m_img->bytesPerLine() * 8 / *depth;
	
	shadow.setFbParam(m_img->bits(), m_img->bytesPerLine());
	shadow.setFbSize(*w, *h);
	return m_img;
#endif
}

extern "C" 
QImageP qtCreateShadowImage(DWORD *w, DWORD *h, int *depth, void **bits, DWORD *stride)
{
	return (QImageP)g_screen->CreateShadowImage(w, h, depth, bits, stride);
}

//
// Handle paint event
//
void qtScreen::paintEvent(QPaintEvent *ev)
{
#ifdef QWS
	QDirectPainter p(this);
	shadow.transfer(&p);
#else

	if (!m_img) return;
//	if (!hasFocus()) return;

	int x = ev->rect().left();
	int y = ev->rect().top();
	int w = ev->rect().width();
	int h = ev->rect().height();

	shadow.transfer(x, y, w, h);

	QPainter p(this);
	p.drawImage(x, y, *m_img, x, y, w, h);
#endif
}

void qtScreen::RedrawScreen(int x, int y, int w, int h, 
			    void *base, int stride, int bpp, int randr)
{
	/* save shadow buffer paramaeters */
	shadow.setShaParam(base, stride, bpp, randr);

	/* update */
	shadow.sha2fb(&x, &y, &w, &h);
	shadow.fb2scr(&x, &y, &w, &h);
	update(x, y, w, h);
	//repaint(x, y, w, h);
}

extern "C" void qtRedrawScreen(int x, int y, int w, int h, 
			       void *base, int stride, int bpp, int randr)
{
	if (!g_screen) return;
	g_screen->RedrawScreen(x, y, w, h, base, stride, bpp, randr);
}

//
// Handle mask
//
extern "C"
void qtSetWindowRgn(QRegionP rgn)
{
	QRegion *r = (QRegion *)rgn;
	g_screen->setMask(*r);
}

//
// toggle screen size
//
void qtScreen::toggleFullScreen(void)
{
	isFullScreen = !isFullScreen;
	setDisplaySize();
}

//
// set screen size
//
void qtScreen::setDisplaySize(void)
{
	if (isFullScreen) {
		scrSize = fullScrSize;
	} else {
		scrSize = normalScrSize;
	}
	resetScreenPos();
}

//
// reset screen position
//
void qtScreen::resetScreenPos(void)
{
	//qDebug("screen size = %d x %d", scrSize.width(), scrSize.height());
	setFixedSize(scrSize);

#ifndef DEBUG_SCR
	reparent(0, WType_TopLevel | WStyle_Customize | WStyle_NoBorderEx,
		 QPoint(0,0));
#endif
	if (isFullScreen) {
		showFullScreen();
	} else {
		showNormal();
	}
}

void qtScreen::moveEvent(QMoveEvent *ev)
{
#ifdef QWS
	checkScreenSizeRotation();
#endif
	if (ev->pos().x() != 0 && ev->pos().y() != 0) {
		resetScreenPos();
	}
}

void qtScreen::resizeEvent(QResizeEvent *ev)
{
	if (size() != scrSize) {
		resetScreenPos();
	}
}

void qtScreen::focusInEvent(QFocusEvent *ev)
{
	resetScreenPos();
}


//
// Handle mouse Events
//
void qtScreen::mouseButtonEvent(QMouseEvent *ev, int isPress)
{
	int button;

	setFocus();

	int x = ev->x();
	int y = ev->y();

	shadow.scr2fb(&x, &y);
	shadow.fb2sha(&x, &y);
	//qDebug("mouse=(%d,%d)", x, y);

	switch (ev->button()) {
	case LeftButton:
		button = 0;
		break;
	case MidButton:
		button = 1;
		break;
	case RightButton:
		button = 2;
		break;
	default:
		return;
	}

#ifdef	QWS
	if (button == 0 && ctrlOn) {
		if (shiftOn) {
			button = 1;	// ctrl + shift -> mid button
		} else {
			button = 2;	// ctrl -> right button
		}
	}
#endif

	qtMouseMotionSendEvent(x, y);
	qtMouseButtonsSendEvent(isPress, button);

	qtPushMouseKeyEvent();
}

void qtScreen::mousePressEvent(QMouseEvent *ev)
{
	mouseButtonEvent(ev, 1);
}

void qtScreen::mouseReleaseEvent(QMouseEvent *ev)
{
	mouseButtonEvent(ev, 0);
}

void qtScreen::mouseMoveEvent(QMouseEvent *ev)
{
	int x = ev->x();
	int y = ev->y();

	shadow.scr2fb(&x, &y);
	shadow.fb2sha(&x, &y);
	//qDebug("mouse=(%d,%d)", x, y);

	qtMouseMotionSendEvent(x, y);
	qtPushMouseKeyEvent();
}

//
// Handle keyboard Events
//
void qtScreen::keyPressEvent(QKeyEvent *ev)
{
	static QTextCodec *codec = QTextCodec::codecForName("eucJP");

#ifdef QWS
	QCString euc;

	switch (ev->key()) {
	case 0:
		euc = codec->fromUnicode(ev->text());
		qtSendKeyMbText(euc.data());
		return;

	case Qt::Key_F11:	// menu
		// ignore
		return;

	case Qt::Key_F22:
		//qDebug("ctrl = on");
		ctrlOn = true;
		return;

	case Qt::Key_Shift:
		shiftOn = true;
		break;
	}
#endif

	qtTranslateKey(ev, TRUE);
}

void qtScreen::keyReleaseEvent(QKeyEvent *ev)
{
#ifdef QWS
	switch (ev->key()) {
	case Qt::Key_F11:	// menu
		if (isFullScreen) {
			toggleFullScreen();
		} else {
			menu->exec();
		}
		return;

	case Qt::Key_F22:
		//qDebug("ctrl = off");
		ctrlOn = false;
		return;

	case Qt::Key_Shift:
		shiftOn = false;
		break;
	}
#endif

	//qDebug("keyRelease:%x", ev->key());
	qtTranslateKey(ev, FALSE);
}
