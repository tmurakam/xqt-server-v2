/*
 *Copyright (C) 1994-2000 The XFree86 Project, Inc. All Rights Reserved.
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
 *Except as contained in this notice, the name of the XFree86 Project
 *shall not be used in advertising or otherwise to promote the sale, use
 *or other dealings in this Software without prior written authorization
 *from the XFree86 Project.
 *
 * Authors:	Dakshinamurthy Karra
 *		Suhaib M Siddiqi
 *		Peter Busch
 *		Harold L Hunt II
 * Qt Version:	Takuya Murakami
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/winmouse.c,v 1.4 2001/10/29 21:10:24 alanh Exp $ */

#include "xqt.h"

void
qtMouseCtrl (DeviceIntPtr pDevice, PtrCtrl *pCtrl)
{
}


/*
 * See Porting Layer Definition - p. 18
 * This is known as a DeviceProc
 */

int
qtMouseProc (DeviceIntPtr pDeviceInt, int iState)
{
	CARD8			map[6];
	DevicePtr		pDevice = (DevicePtr) pDeviceInt;

	switch (iState)
	{
	case DEVICE_INIT:
		map[1] = 1;
		map[2] = 2;
		map[3] = 3;
		map[4] = 4;
		map[5] = 5;
		InitPointerDeviceStruct (pDevice,
					 map,
					 5, /* Buttons 4 and 5 are mouse wheel events */
					 miPointerGetMotionEvents,
					 qtMouseCtrl,
					 miPointerGetMotionBufferSize ());
		break;

	case DEVICE_ON:
		pDevice->on = TRUE;
		break;

	case DEVICE_CLOSE:
	case DEVICE_OFF:
		pDevice->on = FALSE;
		break;
	}
	return Success;
}


/* Handle the mouse wheel */
int
qtMouseWheel (ScreenPtr pScreen, int iDeltaZ)
{
	return 0;
}


/*
 * Enqueue a mouse button event
 */

void 
qtMouseMotionSendEvent(int x, int y)
{
#if 0
	xEvent		xCurrentEvent;

	/* Load an xEvent and enqueue the event */
	xCurrentEvent.u.u.type = MotionNotify;
	xCurrentEvent.u.keyButtonPointer.rootX = x;
	xCurrentEvent.u.keyButtonPointer.rootY = y;
	xCurrentEvent.u.keyButtonPointer.time = GetTimeInMillis();

	mieqEnqueue (&xCurrentEvent);
#else
	miPointerAbsoluteCursor (x, y, g_c32LastInputEventTime = GetTimeInMillis());
#endif
}

void
qtMouseButtonsSendEvent (int isPress, int iButton)
{
	const int xbuttons[3] = {Button1, Button2, Button3};
	xEvent		xCurrentEvent;

	/* Load an xEvent and enqueue the event */
	xCurrentEvent.u.u.type = isPress ? ButtonPress : ButtonRelease;
	xCurrentEvent.u.u.detail = xbuttons[iButton];
	xCurrentEvent.u.keyButtonPointer.time
		= g_c32LastInputEventTime = GetTimeInMillis();
	mieqEnqueue (&xCurrentEvent);
}


/*
 * Decide what to do with a Windows mouse message
 */
#if 0	
/*
  ### この関数は Qt のイベントハンドラ側で書く
  上の qtMouseButtonsSendEvent でイベントを送り込めばよい。
*/

int
winMouseButtonsHandle (ScreenPtr pScreen,
		       int iEventType, int iButton,
		       WPARAM wParam)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;

  /* Send button events right away if emulate 3 buttons is off */
  if (pScreenInfo->iE3BTimeout == WIN_E3B_OFF)
    {
      /* Emulate 3 buttons is off, send the button event */
      winMouseButtonsSendEvent (iEventType, iButton);
      return 0;
    }

  /* Emulate 3 buttons is on, let the fun begin */
  if (iEventType == ButtonPress
      && pScreenPriv->iE3BCachedPress == 0
      && !pScreenPriv->fE3BFakeButton2Sent)
    {
      /*
       * Button was pressed, no press is cached,
       * and there is no fake button 2 release pending.
       */

      /* Store button press type */
      pScreenPriv->iE3BCachedPress = iButton;

      /*
       * Set a timer to send this button press if the other button
       * is not pressed within the timeout time.
       */
      SetTimer (pScreenPriv->hwndScreen,
		WIN_E3B_TIMER_ID,
		pScreenInfo->iE3BTimeout,
		NULL);
    }
  else if (iEventType == ButtonPress
	   && pScreenPriv->iE3BCachedPress != 0
	   && pScreenPriv->iE3BCachedPress != iButton
	   && !pScreenPriv->fE3BFakeButton2Sent)
    {
      /*
       * Button press is cached, other button was pressed,
       * and there is no fake button 2 release pending.
       */

      /* Mouse button was cached and other button was pressed */
      KillTimer (pScreenPriv->hwndScreen, WIN_E3B_TIMER_ID);
      pScreenPriv->iE3BCachedPress = 0;

      /* Send fake middle button */
      winMouseButtonsSendEvent (ButtonPress, Button2);

      /* Indicate that a fake middle button event was sent */
      pScreenPriv->fE3BFakeButton2Sent = TRUE;
    }
  else if (iEventType == ButtonRelease
	   && pScreenPriv->iE3BCachedPress == iButton)
    {
      /*
       * Cached button was released before timer ran out,
       * and before the other mouse button was pressed.
       */
      KillTimer (pScreenPriv->hwndScreen, WIN_E3B_TIMER_ID);
      pScreenPriv->iE3BCachedPress = 0;

      /* Send cached press, then send release */
      winMouseButtonsSendEvent (ButtonPress, iButton);
      winMouseButtonsSendEvent (ButtonRelease, iButton);
    }
  else if (iEventType == ButtonRelease
	   && pScreenPriv->fE3BFakeButton2Sent
	   && !(wParam & MK_LBUTTON)
	   && !(wParam & MK_RBUTTON))
    {
      /*
       * Fake button 2 was sent and both mouse buttons have now been released
       */
      pScreenPriv->fE3BFakeButton2Sent = FALSE;
      
      /* Send middle mouse button release */
      winMouseButtonsSendEvent (ButtonRelease, Button2);
    }
  else if (iEventType == ButtonRelease
	   && pScreenPriv->iE3BCachedPress == 0
	   && !pScreenPriv->fE3BFakeButton2Sent)
    {
      /*
       * Button was release, no button is cached,
       * and there is no fake button 2 release is pending.
       */
      winMouseButtonsSendEvent (ButtonRelease, iButton);
    }

  return 0;
}
#endif
