#ifdef __cplusplus
extern "C" {
#endif

/* 
 * X side
 */
int initXside(int argc, char *argv[]);
void dispatchXEvents(void);

/*
 * Qt side
 */
char *qtGetClipboardDataUtf8(void);
void qtSetClipboardDataUtf8(const char *utf8);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
#include <qwidget.h>
class XEventWatcher : public QWidget
{
Q_OBJECT
public slots:
	void dataReceived(void);
};
#endif
