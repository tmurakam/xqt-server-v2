#include <qapplication.h>
#include <qclipboard.h>
#include <qsocketnotifier.h>

#include "xqtclip.h"

int main(int argc, char *argv[])
{
	QApplication	ap(argc, argv);

	XEventWatcher *manager = new XEventWatcher;

	int xfd = initXside(argc, argv);
	QSocketNotifier *sn = new QSocketNotifier(xfd, QSocketNotifier::Read);

	QObject::connect( sn, SIGNAL(activated(int)),
			  manager, SLOT(dataReceived()) );
	ap.setMainWidget(manager);

	ap.exec();
}

void XEventWatcher::dataReceived(void)
{
//	qDebug("XEvents");
	dispatchXEvents();
}

char *qtGetClipboardDataUtf8(void)
{
//	qDebug("GetClipboard");

	QClipboard *cb = QApplication::clipboard();
	QCString str = cb->text().utf8();
	
	return strdup((const char *)str);
}

void qtSetClipboardDataUtf8(const char *utf8)
{
//	qDebug("SetClipboard");

	QClipboard *cb = QApplication::clipboard();
	cb->setText( QString::fromUtf8(utf8) );
}

