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
 *		Kensuke Matsuzaki
 *
 * Qt Version:	Takuya Murakami
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/win.h,v 1.34 2003/02/12 15:01:38 alanh Exp $ */

#ifndef _XQT_H_
#define _XQT_H_

/*
 * Build toggles for experimental features
 */
/* Turn debug messages on or off */
#define NEED_EVENTS

#define QT_DEFAULT_DPI				75

#define AltMapIndex		Mod1MapIndex
#define NumLockMapIndex		Mod2MapIndex
#define AltLangMapIndex		Mod3MapIndex
#define KanaMapIndex		Mod4MapIndex
#define ScrollLockMapIndex	Mod5MapIndex

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>

#ifdef SVR4
# ifdef X_POSIX_C_SOURCE
#  define _POSIX_C_SOURCE X_POSIX_C_SOURCE
#  include <signal.h>
#  undef _POSIX_C_SOURCE
# else
#  define _POSIX_SOURCE
#  include <signal.h>
#  undef _POSIX_SOURCE
# endif
#endif

#include <X11/XWDFile.h>

#include "X.h"
#include "Xproto.h"
#include "Xos.h"
#include "Xprotostr.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "pixmap.h"
#include "region.h"
#include "regionstr.h"
#include "gcstruct.h"
#include "colormap.h"
#include "colormapst.h"
#include "miscstruct.h"
#include "servermd.h"
#include "windowstr.h"
#include "mi.h"
#include "micmap.h"
#include "migc.h"
#include "mifillarc.h"
#include "mifpoly.h"
#include "mibstore.h"
#include "input.h"
#include "mipointer.h"
#include "keysym.h"
#include "mibstore.h"
#include "micoord.h"
#include "dix.h"
#include "miline.h"
#include "shadow.h"
#include "fb.h"
#include "layer.h"

#ifdef RENDER
#include "mipict.h"
#include "picturestr.h"
#endif

#ifdef RANDR
#include "randrstr.h"
#endif
#include "layer.h"

#include "xqtcmn.h"

/*
 * Qt のヘッダはinclude不能。定義が衝突しまくる。
 */

/*
 * Multi-Window Window Manager header
 */
/*#include "qtwindow.h"*/

/*
 * Debugging macros
 */

#if QTDEBUG
#define DEBUG_MSG(str,...) 
if (fDebugProcMsg) \
{ \
  fprintf(stderr, str, ##__VA_ARGS__); \
}
#else
#define DEBUG_MSG(str,...)
#endif

#if QTDEBUG || YES
#define DEBUG_FN_NAME(str) PTSTR szFunctionName = str
#else
#define DEBUG_FN_NAME(str)
#endif

#if QTDEBUG || YES
#define DEBUGVARS BOOL fDebugProcMsg = FALSE
#else
#define DEBUGVARS
#endif

#if QTDEBUG || YES
#define DEBUGPROC_MSG fDebugProcMsg = TRUE
#else
#define DEBUGPROC_MSG
#endif

/* We use xor this macro for detecting toggle key state changes */
#define WIN_XOR(a,b) ((!(a) && (b)) || ((a) && !(b)))


/*
 * Typedefs for engine dependent function pointers
 */

typedef void (*qtShadowUpdateProcPtr)(ScreenPtr, shadowBufPtr);

typedef Bool (*qtCloseScreenProcPtr)(int, ScreenPtr);

typedef Bool (*qtInitVisualsProcPtr)(ScreenPtr);

typedef Bool (*qtCreateBoundingWindowProcPtr)(ScreenPtr);

typedef Bool (*qtBltExposedRegionsProcPtr)(ScreenPtr);

typedef Bool (*qtActivateAppProcPtr)(ScreenPtr);

typedef Bool (*qtRedrawScreenProcPtr)(ScreenPtr pScreen);

typedef Bool (*qtRealizeInstalledPaletteProcPtr)(ScreenPtr pScreen);

typedef Bool (*qtInstallColormapProcPtr)(ColormapPtr pColormap);

typedef Bool (*qtStoreColorsProcPtr)(ColormapPtr pmap, 
				      int ndef, xColorItem *pdefs);

typedef Bool (*qtCreateColormapProcPtr)(ColormapPtr pColormap);

typedef Bool (*qtDestroyColormapProcPtr)(ColormapPtr pColormap);

typedef Bool (*qtHotKeyAltTabProcPtr)(ScreenPtr);

typedef Bool (*qtCreatePrimarySurfaceProcPtr)(ScreenPtr);

typedef Bool (*qtReleasePrimarySurfaceProcPtr)(ScreenPtr);


/*
 * GC (graphics context) privates
 */

typedef struct
{
#if 0
	HDC			hdc;		/* screen DC */
	HDC			hdcMem;		/* compatible DC */
#else
	QPainterP	       	hPainter;
	QPainterP		hPainterMem;
#endif
} qtPrivGCRec, *qtPrivGCPtr;


/*
 * Pixmap privates
 */

typedef struct
{
#if 0
	HDC		hdcSelected;
	HBITMAP		hBitmap;
	BYTE		*pbBits;
	DWORD		dwScanlineBytes;
	BITMAPINFOHEADER	*pbmih;
#else
	//QPainterP	hPainterSelected;
	QImageP		hImage;
#endif
} qtPrivPixmapRec, *qtPrivPixmapPtr;


/*
 * Colormap privates
 */

typedef struct
{
#if 0	
	HPALETTE		hPalette;
	LPDIRECTDRAWPALETTE	lpDDPalette;
	RGBQUAD		rgbColors[WIN_NUM_PALETTE_ENTRIES];
	PALETTEENTRY		peColors[WIN_NUM_PALETTE_ENTRIES];
#else
	/* ### */
#endif
} qtPrivCmapRec, *qtPrivCmapPtr;


#if WIN_NEW_KEYBOARD_SUPPORT
/*
 * Keyboard event structure
 */

typedef struct
{
  DWORD			dwXKeycodes[WIN_MAX_KEYS_PER_KEY];
  DWORD			dwReleaseModifiers;
} winKeyEventsRec, *winKeyEventsPtr;

#endif /* WIN_NEW_KEYBOARD_SUPPORT */

/*
 * Screen information structure that we need before privates are available
 * in the server startup sequence.
 */

typedef struct
{
	ScreenPtr		pScreen;

	/* Did the user specify a height and width? */
//	Bool			fUserGaveHeightAndWidth;

//	DWORD			dwScreen;
//	DWORD			dwUserWidth;
//	DWORD			dwUserHeight;
	DWORD			dwWidth;
	DWORD			dwHeight;
//	DWORD			dwWidth_mm;
//	DWORD			dwHeight_mm;
	DWORD			dwPaddedWidth;

	/*
	 * dwStride is the number of whole pixels that occupy a scanline,
	 * including those pixels that are not displayed.  This is basically
	 * a rounding up of the width.
	 */
	DWORD			dwStride;

	/* Offset of the screen in the window when using scrollbars */
//	DWORD			dwXOffset;
//	DWORD			dwYOffset;

	DWORD			dwBPP;
	DWORD			dwDepth;
//	DWORD			dwRefreshRate;
	char			*pfb;
	XWDColor		*pXWDCmap;
//	XWDFileHeader		*pXWDHeader;
//	DWORD			dwEngine;
//	DWORD			dwEnginePreferred;
	DWORD			dwClipUpdatesNBoxes;
//	Bool			fEmulatePseudo;
	Bool			fFullScreen;
//	Bool			fDecoration;
//	Bool			fRootless;
//	Bool			fMultiWindow;
//	Bool	                fMultipleMonitors;
//	Bool			fClipboard;
//	Bool			fLessPointer;
//	Bool			fScrollbars;
//	int			iE3BTimeout;
	/* Windows (Alt+F4) and Unix (Ctrl+Alt+Backspace) Killkey */
//	Bool                  fUseWinKillKey;
//	Bool                  fUseUnixKillKey;
//	Bool			fIgnoreInput;

	/* Did the user explicitly set this screen? */
//	Bool			fExplicitScreen;
} qtScreenInfo, *qtScreenInfoPtr;


/*
 * Screen privates
 */

typedef struct _qtPrivScreenRec
{
	qtScreenInfoPtr		pScreenInfo;

	Bool			fEnabled;
	Bool			fClosed;
	Bool			fActive;
	Bool			fBadDepth;

	int			iDeltaZ;

	CloseScreenProcPtr	CloseScreen;

	DWORD			dwRedMask;
	DWORD			dwGreenMask;
	DWORD			dwBlueMask;
	DWORD			dwBitsPerRGB;

	DWORD			dwModeKeyStates;

	/* Clipboard support */
	pthread_t		ptClipboardProc;

#if 0
	HWND			hwndNextViewer;
	void			*display;
	int			window;
#endif

	/* Last width, height, and depth of the Windows display */
	//DWORD			dwLastWindowsWidth;
	//DWORD			dwLastWindowsHeight;
	//DWORD			dwLastWindowsBitsPixel;

	/* Layer support */
	LayerPtr		pLayer;

	/* Palette management */
	ColormapPtr		pcmapInstalled;

	/* Pointer to the root visual so we only have to look it up once */
	VisualPtr		pRootVisual;

	/* 3 button emulation variables */
	int			iE3BCachedPress;
	Bool			fE3BFakeButton2Sent;

	/* Privates used by shadow fb server */
	QImageP			hImageShadow;	
  
	/* Privates used for any module running in a seperate thread */
	pthread_mutex_t		pmServerStarted;
	Bool			fServerStarted;

	/* Engine specific functions */
	qtShadowUpdateProcPtr			pqtShadowUpdate;
	qtCloseScreenProcPtr			pqtCloseScreen;
	qtInitVisualsProcPtr			pqtInitVisuals;
	//qtCreateBoundingWindowProcPtr		pqtCreateBoundingWindow;
	qtBltExposedRegionsProcPtr		pqtBltExposedRegions;
	qtActivateAppProcPtr			pqtActivateApp;
	qtRedrawScreenProcPtr			pqtRedrawScreen;
	qtRealizeInstalledPaletteProcPtr	pqtRealizeInstalledPalette;
	qtInstallColormapProcPtr		pqtInstallColormap;
	qtStoreColorsProcPtr			pqtStoreColors;
	qtCreateColormapProcPtr			pqtCreateColormap;
	qtDestroyColormapProcPtr		pqtDestroyColormap;
	qtHotKeyAltTabProcPtr			pqtHotKeyAltTab;
	qtCreatePrimarySurfaceProcPtr		pqtCreatePrimarySurface;
	qtReleasePrimarySurfaceProcPtr		pqtReleasePrimarySurface;

	/* Window Procedures for Rootless mode */
	CreateWindowProcPtr			CreateWindow;
	DestroyWindowProcPtr			DestroyWindow;
	PositionWindowProcPtr			PositionWindow;
	ChangeWindowAttributesProcPtr		ChangeWindowAttributes;
	RealizeWindowProcPtr			RealizeWindow;
	UnrealizeWindowProcPtr			UnrealizeWindow;
	ValidateTreeProcPtr			ValidateTree;
	PostValidateTreeProcPtr			PostValidateTree;
	WindowExposuresProcPtr			WindowExposures;
	PaintWindowBackgroundProcPtr		PaintWindowBackground;
	PaintWindowBorderProcPtr		PaintWindowBorder;
	CopyWindowProcPtr			CopyWindow;
	ClearToBackgroundProcPtr		ClearToBackground;
	ClipNotifyProcPtr			ClipNotify;
	RestackWindowProcPtr			RestackWindow;
	ReparentWindowProcPtr			ReparentWindow;
#ifdef SHAPE
	SetShapeProcPtr				SetShape;
#endif
} qtPrivScreenRec;

typedef struct _qtPrivScreenRec *qtPrivScreenPtr;

/*
 * Extern declares for general global variables
 */

extern qtScreenInfo		g_ScreenInfo[];
extern miPointerScreenFuncRec	g_qtPointerCursorFuncs;
extern DWORD			g_dwEvents;
extern int			g_fdMessageQueue;
extern int			g_iScreenPrivateIndex;
extern int			g_iCmapPrivateIndex;
extern int			g_iGCPrivateIndex;
//extern int			g_iPixmapPrivateIndex;
extern int			g_iWindowPrivateIndex;
extern unsigned long		g_ulServerGeneration;
extern CARD32			g_c32LastInputEventTime;
extern DWORD			g_dwEnginesSupported;
//extern HINSTANCE		g_hInstance;
//extern HWND			g_hDlgDepthChange;

/*
 * Extern declares for dynamically loaded libraries and function pointers
 */
#if 0
extern HMODULE			g_hmodDirectDraw;
extern FARPROC			g_fpDirectDrawCreate;
extern FARPROC			g_fpDirectDrawCreateClipper;

extern HMODULE			g_hmodCommonControls;
extern FARPROC			g_fpTrackMouseEvent;
#endif

/*
 * Screen privates macros
 */

#define qtGetScreenPriv(pScreen) \
	((qtPrivScreenPtr) (pScreen)->devPrivates[g_iScreenPrivateIndex].ptr)

#define qtSetScreenPriv(pScreen,v) \
	((pScreen)->devPrivates[g_iScreenPrivateIndex].ptr = (pointer) v)

#define qtScreenPriv(pScreen) \
	qtPrivScreenPtr pScreenPriv = qtGetScreenPriv(pScreen)


/*
 * Colormap privates macros
 */

#define qtGetCmapPriv(pCmap) \
	((qtPrivCmapPtr) (pCmap)->devPrivates[g_iCmapPrivateIndex].ptr)

#define qtSetCmapPriv(pCmap,v) \
	((pCmap)->devPrivates[g_iCmapPrivateIndex].ptr = (pointer) v)

#define qtCmapPriv(pCmap) \
	qtPrivCmapPtr pCmapPriv = qtGetCmapPriv(pCmap)


/*
 * GC privates macros
 */

#define qtGetGCPriv(pGC) \
	((qtPrivGCPtr) (pGC)->devPrivates[g_iGCPrivateIndex].ptr)

#define qtSetGCPriv(pGC,v) \
	((pGC)->devPrivates[g_iGCPrivateIndex].ptr = (pointer) v)

#define qtGCPriv(pGC) \
	qtPrivGCPtr pGCPriv = qtGetGCPriv(pGC)


/*
 * Window privates macros
 */

#define qtGetWindowPriv(pWin) \
	((qtPrivWinPtr) (pWin)->devPrivates[g_iWindowPrivateIndex].ptr)

#define qtSetWindowPriv(pWin,v) \
	((pWin)->devPrivates[g_iWindowPrivateIndex].ptr = (pointer) v)

#define qtWindowPriv(pWin) \
	qtPrivWinPtr pWinPriv = qtGetWindowPriv(pWin)


/*
 * FIXME: Windows mouse wheel macro; should be in Cygwin w32api headers.
 * Has been fixed after May 05, 2001.  Remove this section after the
 * fixed headers are in distribution.
 */

#ifndef GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wparam)		((short)HIWORD (wparam))
#endif /* GET_WHEEL_DELTA_WPARAM */


/*
 * FIXME: Windows shell API defines.  Should be in w32api shellapi.h
 */

#ifndef ABS_AUTOHIDE
#define	ABS_AUTOHIDE		0x00000001
#endif
#ifndef ABS_ALWAYSONTOP
#define	ABS_ALWAYSONTOP		0x00000002
#endif


/*
 * BEGIN DDX and DIX Function Prototypes
 */


/*
 * winallpriv.c
 */

Bool
qtAllocatePrivates (ScreenPtr pScreen);

Bool
qtInitCmapPrivates (ColormapPtr pCmap);

Bool
qtAllocateCmapPrivates (ColormapPtr pCmap);


/*
 * winblock.c
 */

void
qtBlockHandler (int nScreen,
		 pointer pBlockData,
		 pointer pTimeout,
		 pointer pReadMask);


/*
 * wincmap.c
 */

int
qtListInstalledColormaps (ScreenPtr pScreen, Colormap *pmaps);

void
qtStoreColors (ColormapPtr pmap, int ndef, xColorItem *pdefs);

void
qtInstallColormap (ColormapPtr pmap);

void
qtUninstallColormap (ColormapPtr pmap);

void
qtResolveColor (unsigned short *pred,
		 unsigned short *pgreen,
		 unsigned short *pblue,
		 VisualPtr	pVisual);

Bool
qtCreateColormap (ColormapPtr pmap);

void
qtDestroyColormap (ColormapPtr pmap);

int
qtExpandDirectColors (ColormapPtr pmap, int ndef,
		       xColorItem *indefs, xColorItem *outdefs);

Bool
qtCreateDefColormap (ScreenPtr pScreen);

//Bool
//qtSetVisualTypesNativeGDI (int nDepth, int nVisuals, int nBitsPerRGB);

void
qtClearVisualTypes (void);


/*
 * wincreatewnd.c
 */

Bool
qtCreateBoundingWindowFullScreen (ScreenPtr pScreen);

Bool
qtCreateBoundingWindowWindowed (ScreenPtr pScreen);


/*
 * wincursor.c
 */

void
qtPointerWarpCursor (ScreenPtr pScreen, int x, int y);

Bool
qtCursorOffScreen (ScreenPtr *ppScreen, int *x, int *y);

void
qtCrossScreen (ScreenPtr pScreen, Bool fEntering);


/*
 * winengine.c
 */

void
qtDetectSupportedEngines ();

Bool
qtSetEngine (ScreenPtr pScreen);

Bool
qtGetDDProcAddresses ();


/*
 * winerror.c
 */

void
OSVenderVErrorF (const char *pszFormat, va_list va_args);


/*
 * winkeybd.c
 */

#if WIN_NEW_KEYBOARD_SUPPORT
winKeyEventsRec
qtTranslateKey (DWORD dwVirtualKey, DWORD dwKeyData);
#else
// ###
//void
//qtTranslateKey (WPARAM wParam, LPARAM lParam, int *piScanCode);
#endif

void
qtGetKeyMappings (KeySymsPtr pKeySyms, CARD8 *pModMap);

void
qtKeybdBell (int iPercent, DeviceIntPtr pDeviceInt,
	      pointer pCtrl, int iClass);

void
qtKeybdCtrl (DeviceIntPtr pDevice, KeybdCtrl *pCtrl);

int
qtKeybdProc (DeviceIntPtr pDeviceInt, int iState);

void
qtInitializeModeKeyStates (void);

void
qtStoreModeKeyStates (ScreenPtr pScreen);

void
qtRestoreModeKeyStates (ScreenPtr pScreen);

//Bool
//qtIsFakeCtrl_L (UINT message, WPARAM wParam, LPARAM lParam);

void
qtKeybdReleaseKeys ();

void
qtSendKeyEvent (DWORD dwKey, Bool fDown);

#if WIN_NEW_KEYBOARD_SUPPORT
void
qtProcessKeyEvent (DWORD dwVirtKey, DWORD dwKeyData);
#endif


// ###
//BOOL
//qtPaintBackground (QWidgetP hwnd, COLORREF colorref);


/*
 * winmouse.c
 */

void
qtMouseCtrl (DeviceIntPtr pDevice, PtrCtrl *pCtrl);

int
qtMouseProc (DeviceIntPtr pDeviceInt, int iState);

int
qtMouseWheel (ScreenPtr pScreen, int iDeltaZ);

void
qtMouseButtonsSendEvent (int iEventType, int iButton);

//int
//qtMouseButtonsHandle (ScreenPtr pScreen,
//		       int iEventType, int iButton,
//		       WPARAM wParam);


/*
 * qtscrinit.c
 */

Bool
qtScreenInit (int index,
	       ScreenPtr pScreen,
	       int argc, char **argv);

Bool
qtFinishScreenInitFB (int index,
		       ScreenPtr pScreen,
		       int argc, char **argv);

Bool
qtSaveScreen (ScreenPtr pScreen, int on);

PixmapPtr
qtGetWindowPixmap (WindowPtr pWin);

void
qtSetWindowPixmap (WindowPtr pWin, PixmapPtr pPix);


/*
 * qtshadow.c
 */

Bool
qtAllocateFBShadow (ScreenPtr pScreen);

void
qtShadowUpdate (ScreenPtr pScreen, 
		    shadowBufPtr pBuf);

Bool
qtCloseScreenShadow (int nIndex, ScreenPtr pScreen);

Bool
qtInitVisualsShadow (ScreenPtr pScreen);

Bool
qtActivateAppShadow (ScreenPtr pScreen);

Bool
qtRedrawScreenShadow (ScreenPtr pScreen);

Bool
qtSetEngineFunctionsShadow (ScreenPtr pScreen);

Bool
qtRealizeInstalledPaletteShadow (ScreenPtr pScreen);

Bool
qtInstallColormapShadow (ColormapPtr pColormap);

Bool
qtStoreColorsShadow (ColormapPtr pmap, 
			 int ndef,
			 xColorItem *pdefs);

Bool
qtCreateColormapShadow (ColormapPtr pColormap);

Bool
qtDestroyColormapShadow (ColormapPtr pColormap);


/*
 * winwakeup.c
 */

void
qtWakeupHandler (int nScreen,
		  pointer pWakeupData,
		  unsigned long ulResult,
		  pointer pReadmask);

/*
 * qtwindow.c
 */

Bool
qtCreateWindowPRootless (WindowPtr pWindow);

Bool
qtDestroyWindowPRootless (WindowPtr pWindow);

Bool
qtPositionWindowPRootless (WindowPtr pWindow, int x, int y);

Bool
qtChangeWindowAttributesPRootless (WindowPtr pWindow, unsigned long mask);

Bool
qtUnmapWindowPRootless (WindowPtr pWindow);

Bool
qtMapWindowPRootless (WindowPtr pWindow);

#ifdef SHAPE
void
qtSetShapePRootless (WindowPtr pWindow);
#endif

/*
 * qtrandr.c
 */
Bool qtRandRInit(ScreenPtr pScreen);


/*
 * END DDX and DIX Function Prototypes
 */

#endif /* _XQT_H_ */

