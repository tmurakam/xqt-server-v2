/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86Keymap.h,v 3.19 2002/05/31 18:45:58 dawes Exp $ */
/*
 *
 * based on xf86Keymap.h
 *
 */
/* $XConsortium: xf86Keymap.h /main/14 1996/02/21 17:38:47 kaleb $ */

#define XK_TECHNICAL
#define XK_KATAKANA
#include "keysym.h"

#define GLYPHS_PER_KEY	4

#define 	K(qtkey, keysym1, keysym2)	keysym1, keysym2, NoSymbol, NoSymbol,
#define 	KD(qtkey, keysym1, keysym2)	keysym1, keysym2, NoSymbol, NoSymbol,

static KeySym qtKeyMap[] = {
#include "KeyMap.h"
};

#define	NUM_KEYCODES	(sizeof(qtKeyMap) / sizeof(KeySym) / GLYPHS_PER_KEY)
#define	MAX_KEYCODE	(NUM_KEYCODES + MIN_KEYCODE - 1)

#define AltMask         Mod1Mask
#define NumLockMask     Mod2Mask
#define AltLangMask     Mod3Mask
#define KanaMask        Mod4Mask
#define ScrollLockMask  Mod5Mask
