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
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/winkeybd.c,v 1.12 2002/10/17 08:18:22 alanh Exp $ */


#include "xqt.h"
#include "qtkeymap.h"
 
#ifdef XKB
#define XKB_IN_SERVER
#include "XKBsrv.h"
#endif

/*
 * Define keymap
 */
static Bool g_qtKeyState[NUM_KEYCODES];

/*
 * We call this function from qtKeybdProc when we are
 * initializing the keyboard.
 */
void
qtGetKeyMappings (KeySymsPtr pKeySyms, CARD8 *pModMap)
{
	int			i;
	KeySym		*pMap = qtKeyMap;
	KeySym		*pKeySym;

	/*
	 * Initialize all key states to up... which may not be true
	 * but it is close enough.
	 */
	memset(g_qtKeyState, 0, sizeof (g_qtKeyState[0]) * NUM_KEYCODES);

	/* MAP_LENGTH is defined in Xserver/include/input.h to be 256 */
	for (i = 0; i < MAP_LENGTH; i++)
		pModMap[i] = NoSymbol;  /* make sure it is restored */

	/* Loop through all valid entries in the key symbol table */
	for (pKeySym = pMap, i = MIN_KEYCODE;
	     i < (MIN_KEYCODE + NUM_KEYCODES);
	     i++, pKeySym += GLYPHS_PER_KEY)
	{
		switch (*pKeySym)
		{
		case XK_Shift_L:
		case XK_Shift_R:
			pModMap[i] = ShiftMask;
			break;

		case XK_Control_L:
		case XK_Control_R:
			pModMap[i] = ControlMask;
			break;

		case XK_Caps_Lock:
			pModMap[i] = LockMask;
			break;

		case XK_Alt_L:
		case XK_Alt_R:
			pModMap[i] = AltMask;
			break;

#if !WIN_NEW_KEYBOARD_SUPPORT
		case XK_Num_Lock:
			pModMap[i] = NumLockMask;
			break;

		case XK_Scroll_Lock:
			pModMap[i] = ScrollLockMask;
			break;

			/* Hirigana/Katakana toggle */
		case XK_Kana_Lock:
		case XK_Kana_Shift:
			pModMap[i] = KanaMask;
			break;
#endif

			/* alternate toggle for multinational support */
		case XK_Mode_switch:
			pModMap[i] = AltLangMask;
			break;
		}
	}

	pKeySyms->map        = (KeySym *) pMap;
	pKeySyms->mapWidth   = GLYPHS_PER_KEY;
	pKeySyms->minKeyCode = MIN_KEYCODE;
	pKeySyms->maxKeyCode = MAX_KEYCODE;
}


/* Ring the keyboard bell (system speaker on PCs) */
void
qtKeybdBell (int iPercent, DeviceIntPtr pDeviceInt,
	      pointer pCtrl, int iClass)
{
	/* ### */
}


/* Change some keyboard configuration parameters */
void
qtKeybdCtrl (DeviceIntPtr pDevice, KeybdCtrl *pCtrl)
{

}


/* 
 * See Porting Layer Definition - p. 18
 * winKeybdProc is known as a DeviceProc.
 */

int
qtKeybdProc (DeviceIntPtr pDeviceInt, int iState)
{
	KeySymsRec		keySyms;
	CARD8 		modMap[MAP_LENGTH];
	DevicePtr		pDevice = (DevicePtr) pDeviceInt;
#ifdef XKB
	XkbComponentNamesRec names;
#endif

	switch (iState)
	{
	case DEVICE_INIT:
		//qtConfigKeyboard (pDeviceInt);
		qtGetKeyMappings (&keySyms, modMap);

		defaultKeyboardControl.leds = 0;

#ifdef XKB
#if 0	/* ### */
		if (g_qtInfo.xkb.disable) 
		{
#endif
#endif
			InitKeyboardDeviceStruct (pDevice,
						  &keySyms,
						  modMap,
						  qtKeybdBell,
						  qtKeybdCtrl);
#ifdef XKB
#if 0	/* ### */
		} 
		else 
		{

			if (XkbInitialMap) 
			{
				names.keymap = XkbInitialMap;
				names.keycodes = NULL;
				names.types = NULL;
				names.compat = NULL;
				names.symbols = NULL;
				names.geometry = NULL;
			} 
			else 
			{
				names.keymap = g_qtInfo.xkb.keymap;
				names.keycodes = g_qtInfo.xkb.keycodes;
				names.types = g_qtInfo.xkb.types;
				names.compat = g_qtInfo.xkb.compat;
				names.symbols = g_qtInfo.xkb.symbols;
				names.geometry = g_qtInfo.xkb.geometry;
			}

			XkbSetRulesDflts (g_qtInfo.xkb.rules, g_qtInfo.xkb.model, 
					  g_qtInfo.xkb.layout, g_qtInfo.xkb.variant, 
					  g_qtInfo.xkb.options);
			XkbInitKeyboardDeviceStruct (pDeviceInt, &names, &keySyms,
						     modMap, qtKeybdBell, qtKeybdCtrl);
		}
#endif
#endif
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


/*
 * Detect current mode key states upon server startup.
 *
 * Simulate a press and release of any key that is currently
 * toggled.
 */

void
qtInitializeModeKeyStates (void)
{
#if 0
#if !WIN_NEW_KEYBOARD_SUPPORT
  /* Restore NumLock */
  if (GetKeyState (VK_NUMLOCK) & 0x0001)
    {
      winSendKeyEvent (KEY_NumLock, TRUE);
      winSendKeyEvent (KEY_NumLock, FALSE);
    }

  /* Restore CapsLock */
  if (GetKeyState (VK_CAPITAL) & 0x0001)
    {
      winSendKeyEvent (KEY_CapsLock, TRUE);
      winSendKeyEvent (KEY_CapsLock, FALSE);
    }

  /* Restore ScrollLock */
  if (GetKeyState (VK_SCROLL) & 0x0001)
    {
      winSendKeyEvent (KEY_ScrollLock, TRUE);
      winSendKeyEvent (KEY_ScrollLock, FALSE);
    }

  /* Restore KanaLock */
  if (GetKeyState (VK_KANA) & 0x0001)
    {
      winSendKeyEvent (KEY_HKTG, TRUE);
      winSendKeyEvent (KEY_HKTG, FALSE);
    }
#endif
#endif
}


/*
 * We have to store the last state of each mode
 * key before we lose the keyboard focus.
 */

#if 0
void
winStoreModeKeyStates (ScreenPtr pScreen)
{
#if !WIN_NEW_KEYBOARD_SUPPORT
  winScreenPriv(pScreen);

  /* Initialize all mode key states to off */
  pScreenPriv->dwModeKeyStates = 0x0L;

  pScreenPriv->dwModeKeyStates |= 
    (GetKeyState (VK_NUMLOCK) & 0x0001) << NumLockMapIndex;

  pScreenPriv->dwModeKeyStates |=
    (GetKeyState (VK_SCROLL) & 0x0001) << ScrollLockMapIndex;

  pScreenPriv->dwModeKeyStates |=
    (GetKeyState (VK_CAPITAL) & 0x0001) << LockMapIndex;

  pScreenPriv->dwModeKeyStates |=
    (GetKeyState (VK_KANA) & 0x0001) << KanaMapIndex;
#endif
}
#endif


/*
 * Upon regaining the keyboard focus we must
 * resynchronize our internal mode key states
 * with the actual state of the keys.
 */

#if 0
void
winRestoreModeKeyStates (ScreenPtr pScreen)
{
#if !WIN_NEW_KEYBOARD_SUPPORT
  winScreenPriv(pScreen);
  DWORD			dwKeyState;

  /* 
   * NOTE: The C XOR operator, ^, will not work here because it is
   * a bitwise operator, not a logical operator.  C does not
   * have a logical XOR operator, so we use a macro instead.
   */

  /* Has the key state changed? */
  dwKeyState = GetKeyState (VK_NUMLOCK) & 0x0001;
  if (WIN_XOR (pScreenPriv->dwModeKeyStates & NumLockMask, dwKeyState))
    {
      winSendKeyEvent (KEY_NumLock, TRUE);
      winSendKeyEvent (KEY_NumLock, FALSE);
    }

  /* Has the key state changed? */
  dwKeyState = GetKeyState (VK_CAPITAL) & 0x0001;
  if (WIN_XOR (pScreenPriv->dwModeKeyStates & LockMask, dwKeyState))
    {
      winSendKeyEvent (KEY_CapsLock, TRUE);
      winSendKeyEvent (KEY_CapsLock, FALSE);
    }

  /* Has the key state changed? */
  dwKeyState = GetKeyState (VK_SCROLL) & 0x0001;
  if (WIN_XOR (pScreenPriv->dwModeKeyStates & ScrollLockMask, dwKeyState))
    {
      winSendKeyEvent (KEY_ScrollLock, TRUE);
      winSendKeyEvent (KEY_ScrollLock, FALSE);
    }

  /* Has the key state changed? */
  dwKeyState = GetKeyState (VK_KANA) & 0x0001;
  if (WIN_XOR (pScreenPriv->dwModeKeyStates & KanaMask, dwKeyState))
    {
      winSendKeyEvent (KEY_HKTG, TRUE);
      winSendKeyEvent (KEY_HKTG, FALSE);
    }
#endif
}
#endif


#if !WIN_NEW_KEYBOARD_SUPPORT
/*
 * Look for the lovely fake Control_L press/release generated by Windows
 * when AltGr is pressed/released on a non-U.S. keyboard.
 */

#if 0
Bool
winIsFakeCtrl_L (UINT message, WPARAM wParam, LPARAM lParam)
{
  MSG		msgNext;
  LONG		lTime;
  Bool		fReturn;

  /*
   * Fake Ctrl_L presses will be followed by an Alt_R keypress
   * with the same timestamp as the Ctrl_L press.
   */
  if (message == WM_KEYDOWN
      && wParam == VK_CONTROL
      && (HIWORD (lParam) & KF_EXTENDED) == 0)
    {
      /* Got a Ctrl_L press */

      /* Get time of current message */
      lTime = GetMessageTime ();
      			
      /* Look for fake Ctrl_L preceeding an Alt_R press. */
      fReturn = PeekMessage (&msgNext, NULL,
			     WM_KEYDOWN, WM_KEYDOWN,
			     PM_NOREMOVE);

      /* Is next press an Alt_R with the same timestamp? */
      if (fReturn && msgNext.wParam == VK_MENU
	  && msgNext.time == lTime
	  && (HIWORD (msgNext.lParam) & KF_EXTENDED))
	{
	  /* 
	   * Next key press is Alt_R with same timestamp as current
	   * Ctrl_L message.  Therefore, this Ctrl_L press is a fake
	   * event, so discard it.
	   */
	  return TRUE;
	}
    }

  /* 
   * Fake Ctrl_L releases will be followed by an Alt_R release
   * with the same timestamp as the Ctrl_L release.
   */
  if ((message == WM_KEYUP || message == WM_SYSKEYUP)
      && wParam == VK_CONTROL
      && (HIWORD (lParam) & KF_EXTENDED) == 0)
    {
      /* Got a Ctrl_L release */

      /* Get time of current message */
      lTime = GetMessageTime ();

      /* Look for fake Ctrl_L release preceeding an Alt_R release. */
      fReturn = PeekMessage (&msgNext, NULL,
			     WM_KEYUP, WM_SYSKEYUP, 
			     PM_NOREMOVE);

      /* Is next press an Alt_R with the same timestamp? */
      if (fReturn
	  && (msgNext.message == WM_KEYUP
	      || msgNext.message == WM_SYSKEYUP)
	  && msgNext.wParam == VK_MENU
	  && msgNext.time == lTime
	  && (HIWORD (msgNext.lParam) & KF_EXTENDED))
	{
	  /*
	   * Next key release is Alt_R with same timestamp as current
	   * Ctrl_L message. Therefore, this Ctrl_L release is a fake
	   * event, so discard it.
	   */
	  return TRUE;
	}
    }
  
  /* Not a fake control left press/release */
  return FALSE;
}
#endif
#endif /* WIN_NEW_KEYBOARD_SUPPORT */


/*
 * Lift any modifier keys that are pressed
 */

#if 0	/*### */
void
winKeybdReleaseKeys ()
{
#if !WIN_NEW_KEYBOARD_SUPPORT
  int				i;

  /* Verify that the mi input system has been initialized */
  if (g_fdMessageQueue == WIN_FD_INVALID)
    return;

  /* Pop any pressed keys */
  for (i = 0; i < NUM_KEYCODES; ++i)
    {
      if (g_winKeyState[i]) winSendKeyEvent (i, FALSE);
    }
#endif
}
#endif


/*
 * Take a raw X key code and send an up or down event for it.
 *
 * Thanks to VNC for inspiration, though it is a simple function.
 */

void
qtSendKeyEvent (DWORD dwKey, int fDown)
{
	xEvent			xCurrentEvent;

	/*
	 * When alt-tabing between screens we can get phantom key up messages
	 * Here we only pass them through it we think we should!
	 */
	if (g_qtKeyState[dwKey] == FALSE && fDown == FALSE) return;

	/* Update the keyState map */
	g_qtKeyState[dwKey] = fDown;
  
	memset(&xCurrentEvent, 0, sizeof (xCurrentEvent));

	xCurrentEvent.u.u.type = fDown ? KeyPress : KeyRelease;
	xCurrentEvent.u.keyButtonPointer.time =
		g_c32LastInputEventTime = GetTimeInMillis();
	xCurrentEvent.u.u.detail = dwKey; // + MIN_KEYCODE;
	mieqEnqueue (&xCurrentEvent);
}

/*
 * Multi Byte String をそのまま XKeyEvent で送る
 * keycode の最上位ビットを立てる (X/Qt 独自仕様!)
 */
void
qtSendKeyMbText(const char *text)
{
	xEvent		xCurrentEvent;
	const char 	*p;

	memset(&xCurrentEvent, 0, sizeof (xCurrentEvent));

	xCurrentEvent.u.u.type = KeyPress;
	xCurrentEvent.u.keyButtonPointer.time =
		g_c32LastInputEventTime = GetTimeInMillis();
	xCurrentEvent.u.keyButtonPointer.pad1 = 0xff;	/* magic! */

	for (p = text; *p; p++) {
		xCurrentEvent.u.u.detail = *p;
		mieqEnqueue (&xCurrentEvent);
	}
}
