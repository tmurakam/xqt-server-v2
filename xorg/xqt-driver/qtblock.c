#include "xqt.h"

void
qtBlockHandler (int nScreen,
		pointer pBlockData,
		pointer pTimeout,
		pointer pReadMask)
{
	qtApProcessEvents();
}
