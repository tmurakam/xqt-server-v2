/****************************************************************************
** XEventWatcher meta object code from reading C++ file 'xqtclip.h'
**
** Created: Sun Oct 3 19:43:29 2004
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "xqtclip.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *XEventWatcher::className() const
{
    return "XEventWatcher";
}

QMetaObject *XEventWatcher::metaObj = 0;

void XEventWatcher::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QWidget::className(), "QWidget") != 0 )
	badSuperclassWarning("XEventWatcher","QWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString XEventWatcher::tr(const char* s)
{
    return qApp->translate( "XEventWatcher", s, 0 );
}

QString XEventWatcher::tr(const char* s, const char * c)
{
    return qApp->translate( "XEventWatcher", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* XEventWatcher::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (XEventWatcher::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    m1_t0 v1_0 = &XEventWatcher::dataReceived;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    QMetaData *slot_tbl = QMetaObject::new_metadata(1);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(1);
    slot_tbl[0].name = "dataReceived()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"XEventWatcher", "QWidget",
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}
