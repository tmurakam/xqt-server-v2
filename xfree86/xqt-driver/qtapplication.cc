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
 * Qt Application/GUI thread
 */

#ifdef QWS
#include <qpe/global.h>
#endif

#include "qtapplication.h"
#include "qmessagebox.h"

#include "xqtcmn.h"
#include "qtscreen.h"

#include <stdio.h>

XqtApplication *g_ap;

XqtApplication::XqtApplication(int &argc, char **argv)
	: QPEApplication(argc, argv)
{
}

#ifdef QWS
bool XqtApplication::qwsEventFilter(QWSEvent *e)
{
	// dirty hack...
	if ( e->type == QWSEvent::Key) {
		QWSKeyEvent *ke = (QWSKeyEvent *)e;
#if 0
		fprintf(stderr, "KEY: win=%x unicode=%x keycode=%x modifier=%x press=%x\n",
		       ke->simpleData.window, ke->simpleData.unicode, ke->simpleData.keycode,
		       ke->simpleData.modifiers, ke->simpleData.is_press);
#endif
		if (ke->simpleData.keycode == Qt::Key_Escape) {
			// avoid QPEApplication::qwsEventFilter
			return QApplication::qwsEventFilter(e);
		}
	}
	return QPEApplication::qwsEventFilter(e);
}
#endif

void XqtApplication::showHelp(void)
{
#ifdef QWS
	Global::execute("helpbrowser", "Xqt.html");
#endif
}


extern "C"
void qtApInit(int argc, char *argv[])
{
	qDebug("qtApInit");
	if (!g_ap) {
		g_ap = new XqtApplication(argc, argv);
		g_screen = new qtScreen;
	}

	g_ap->setMainWidget(g_screen);
	g_ap->processEvents();
}

extern "C"
void qtApProcessEvents(void)
{
	g_ap->processEvents();
}

extern "C"
void qtShowFatalError(const char *msg)
{
	if (!g_ap) return;
	QMessageBox::critical(NULL, "X/Qt Server", QString(msg));
}
