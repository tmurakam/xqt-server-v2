/* $TOG: InitInput.c /main/12 1998/02/10 13:23:52 kaleb $ */
/*

  Copyright 1993, 1998  The Open Group

  Permission to use, copy, modify, distribute, and sell this software and its
  documentation for any purpose is hereby granted without fee, provided that
  the above copyright notice appear in all copies and that both that
  copyright notice and this permission notice appear in supporting
  documentation.

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.

  Except as contained in this notice, the name of The Open Group shall
  not be used in advertising or otherwise to promote the sale, use or
  other dealings in this Software without prior written authorization
  from The Open Group.

*/
/*
 * Xqt version : Takuya Murakami
 */

/* $XFree86: xc/programs/Xserver/hw/xwin/InitInput.c,v 1.12 2003/02/12 15:01:38 alanh Exp $ */

#include "xqt.h"

#include <sys/socket.h>
#include <sys/ioctl.h>

CARD32		g_c32LastInputEventTime = 0;

static int	g_fdMouseKeyEvents[2];


/* Called from dix/devices.c */
/*
 * All of our keys generate up and down transition notifications,
 * so all of our keys can be used as modifiers.
 * 
 * An example of a modifier is mapping the A key to the Control key.
 * A has to be a legal modifier.  I think.
 */

Bool
LegalModifier (unsigned int uiKey, DevicePtr pDevice)
{
	return TRUE;
}


/* Called from dix/dispatch.c */
/*
 * Run through the Windows message queue(s) one more time.
 * Tell mi to dequeue the events that we have sent it.
 */
void
ProcessInputEvents (void)
{
#if 0
	ErrorF ("ProcessInputEvents\n");
#endif

	mieqProcessInputEvents ();
	miPointerUpdate ();

#if 0
	ErrorF ("ProcessInputEvents - returning\n");
#endif
}


#if 0
int
TimeSinceLastInputEvent ()
{
	if (g_c32LastInputEventTime == 0)
		g_c32LastInputEventTime = GetTickCount ();
	return GetTickCount () - g_c32LastInputEventTime;
}
#endif

void
qtPushMouseKeyEvent(void)
{
	char dummy = 0xff;

	write(g_fdMouseKeyEvents[0], &dummy, 1);
}

void
qtPopMouseKeyEvent(void)
{
	char buf[64];

	read(g_fdMouseKeyEvents[1], buf, sizeof(buf));
}

/* See Porting Layer Definition - p. 17 */
void
InitInput (int argc, char *argv[])
{
	DeviceIntPtr		pMouse, pKeyboard;
	int on;

#if QTDEBUG
	ErrorF ("InitInput\n");
#endif

	pMouse = AddInputDevice (qtMouseProc, TRUE);
	pKeyboard = AddInputDevice (qtKeybdProc, TRUE);
  
	RegisterPointerDevice (pMouse);
	RegisterKeyboardDevice (pKeyboard);

	miRegisterPointerDevice (screenInfo.screens[0], pMouse);
	mieqInit ((DevicePtr)pKeyboard, (DevicePtr)pMouse);

	/* Initialize the mode key states */
	qtInitializeModeKeyStates ();


	/* ### マウス/キーボードイベント通知用のソケットを作成する */
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, g_fdMouseKeyEvents) < 0) {
		FatalError ("InitInput - Failed socketpair\n");
	}
	AddEnabledDevice(g_fdMouseKeyEvents[1]);

	/* ### non blocking */
	on = 1;
	ioctl(g_fdMouseKeyEvents[1], FIONBIO, &on);


#if QTDEBUG
	ErrorF ("InitInput - returning\n");
#endif
}


#ifdef XTESTEXT1
void
XTestGenerateEvent (int dev_type, int keycode, int keystate,
		    int mousex, int mousey)
{
	ErrorF ("XTestGenerateEvent\n");
}


void
XTestGetPointerPos (short *fmousex, short *fmousey)
{
	ErrorF ("XTestGetPointerPos\n");
}


void
XTestJumpPointer (int jx, int jy, int dev_type)
{
	ErrorF ("XTestJumpPointer\n");
}
#endif

