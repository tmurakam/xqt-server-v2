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
 * Qt Version:	Takuya Murakami		
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/winscrinit.c,v 1.26 2003/02/12 15:01:38 alanh Exp $ */

#include "xqt.h"

qtScreenInfo    g_ScreenInfo[MAXSCREENS];
int		g_iScreenPrivateIndex;

/*
 * Determine what type of screen we are initializing
 * and call the appropriate procedure to intiailize
 * that type of screen.
 */

Bool
qtScreenInit (int index,
	      ScreenPtr pScreen,
	      int argc, char **argv)
{
	qtScreenInfoPtr      pScreenInfo = &g_ScreenInfo[index];
	qtPrivScreenPtr	pScreenPriv;

#if QTDEBUG
	ErrorF ("qtScreenInit - dwWidth: %d dwHeight: %d\n",
		pScreenInfo->dwWidth, pScreenInfo->dwHeight);
#endif

	/* Allocate privates for this screen */
	if (!qtAllocatePrivates (pScreen))
	{
		ErrorF ("qtScreenInit - Couldn't allocate screen privates\n");
		return FALSE;
	}

	/* Get a pointer to the privates structure that was allocated */
	pScreenPriv = qtGetScreenPriv (pScreen);

	/* Save a pointer to this screen in the screen info structure */
	pScreenInfo->pScreen = pScreen;

	/* Save a pointer to the screen info in the screen privates structure */
	/* This allows us to get back to the screen info from a screen pointer */
	pScreenPriv->pScreenInfo = pScreenInfo;

	/* Initialize Shadow engine */
	qtInitializeShadow (pScreen);

	pScreenInfo->dwClipUpdatesNBoxes = 0;

	/* Check for supported display depth */
#if 0	/* ### */
	if (!(WIN_SUPPORTED_BPPS & (1 << (pScreenInfo->dwBPP - 1))))
	{
		ErrorF ("qtScreenInit - Unsupported display depth: %d\n" \
			"Change your Windows display depth to 15, 16, 24, or 32 bits "
			"per pixel.\n",
			pScreenInfo->dwBPP);
		ErrorF ("qtScreenInit - Supported depths: %08x\n",
			WIN_SUPPORTED_BPPS);
#if WIN_CHECK_DEPTH
		return FALSE;
#endif
	}
#endif

	/*
	 * Check that all monitors have the same display depth if we are using
	 * multiple monitors
	 */
#if 0	/* ### */
	if (pScreenInfo->fMultipleMonitors 
	    && !GetSystemMetrics (SM_SAMEDISPLAYFORMAT))
	{
		ErrorF ("qtScreenInit - Monitors do not all have same pixel format / "
			"display depth.\n"
			"Using primary display only.\n");
		pScreenInfo->fMultipleMonitors = FALSE;
	}
#endif

#if 0
	pScreenPriv->dwLastWindowsWidth = pScreenInfo->dwWidth;
	pScreenPriv->dwLastWindowsHeight = pScreenInfo->dwHeight;

	/* Save the original bits per pixel */
	pScreenPriv->dwLastWindowsBitsPixel = pScreenInfo->dwBPP;
#endif

	/* Clear the visuals list */
	miClearVisualTypes ();
  
	/* Set the padded screen width */
//	pScreenInfo->dwPaddedWidth = PixmapBytePad (pScreenInfo->dwWidth,
//						    pScreenInfo->dwBPP);


#ifdef RANDR
	qtRandRInit(pScreen);
#endif	

	/* Call the engine dependent screen initialization procedure */
	if (!qtFinishScreenInitFB(index, pScreen, argc, argv))
	{
		ErrorF ("qtScreenInit - winFinishScreenInit () failed\n");
		return FALSE;
	}

#if QTDEBUG || YES
	ErrorF ("qtScreenInit - returning\n");
#endif

	return TRUE;
}


/* See Porting Layer Definition - p. 20 */
Bool
qtFinishScreenInitFB (int index,
		       ScreenPtr pScreen,
		       int argc, char **argv)
{
	qtScreenPriv(pScreen);
	qtScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
	VisualPtr		pVisual = NULL;
	char			*pbits = NULL;
	int			iReturn;

	/*
	 * Grab the number of bits that are used to represent color in each pixel.
	 */
#if 0
	if (pScreenInfo->dwBPP == 8)
		pScreenInfo->dwDepth = 8;
	else
		pScreenInfo->dwDepth = winCountBits (pScreenPriv->dwRedMask)
			+ winCountBits (pScreenPriv->dwGreenMask)
			+ winCountBits (pScreenPriv->dwBlueMask);
	ErrorF ("qtFinishScreenInitFB - Masks: %08x %08x %08x\n",
		pScreenPriv->dwRedMask, pScreenPriv->dwGreenMask,
		pScreenPriv->dwBlueMask);

#endif
	pScreenInfo->dwDepth = pScreenInfo->dwBPP;

	/* Init visuals */
	if (!(*pScreenPriv->pqtInitVisuals) (pScreen))
	{
		ErrorF ("qtFinishScreenInitFB - winInitVisuals failed\n");
		return FALSE;
	}

	/* Setup a local variable to point to the framebuffer */
	pbits = pScreenInfo->pfb;
  
	/* Apparently we need this for the render extension */
	miSetPixmapDepths ();

	/* Start fb initialization */
	if (!fbSetupScreen (pScreen,
			    pScreenInfo->pfb,
			    pScreenInfo->dwWidth, pScreenInfo->dwHeight,
			    monitorResolution, monitorResolution,
			    pScreenInfo->dwStride,
			    pScreenInfo->dwBPP))
	{
		ErrorF ("qtFinishScreenInitFB - fbSetupScreen failed\n");
		return FALSE;
	}

	/* Override default colormap routines if visual class is dynamic */
#if 0	/* do not support depth == 8 */
	if (pScreenInfo->dwDepth == 8)
	{
		pScreen->CreateColormap = qtCreateColormap;
		pScreen->DestroyColormap = qtDestroyColormap;
		pScreen->InstallColormap = qtInstallColormap;
		pScreen->UninstallColormap = qtUninstallColormap;
		pScreen->ListInstalledColormaps = qtListInstalledColormaps;
		pScreen->StoreColors = qtStoreColors;
		pScreen->ResolveColor = winResolveColor;

		/*
		 * NOTE: Setting whitePixel to 255 causes Magic 7.1 to allocate its
		 * own colormap, as it cannot allocate 7 planes in the default
		 * colormap.  Setting whitePixel to 1 allows Magic to get 7
		 * planes in the default colormap, so it doesn't create its
		 * own colormap.  This latter situation is highly desireable,
		 * as it keeps the Magic window viewable when switching to
		 * other X clients that use the default colormap.
		 */
		pScreen->blackPixel = 0;
		pScreen->whitePixel = 1;
	}
#endif

	/* Place our save screen function */
	pScreen->SaveScreen = qtSaveScreen;

	/* Backing store functions */
	/*
	 * FIXME: Backing store support still doesn't seem to be working.
	 */
	pScreen->BackingStoreFuncs.SaveAreas = fbSaveAreas;
	pScreen->BackingStoreFuncs.RestoreAreas = fbRestoreAreas;

	/* Finish fb initialization */
	if (!fbFinishScreenInit (pScreen,
				 pScreenInfo->pfb,
				 pScreenInfo->dwWidth, pScreenInfo->dwHeight,
				 monitorResolution, monitorResolution,
				 pScreenInfo->dwStride,
				 pScreenInfo->dwBPP))
	{
		ErrorF ("qtFinishScreenInitFB - fbFinishScreenInit failed\n");
		return FALSE;
	}

	/* Save a pointer to the root visual */
	for (pVisual = pScreen->visuals;
	     pVisual->vid != pScreen->rootVisual;
	     pVisual++);
	pScreenPriv->pRootVisual = pVisual;

	/* 
	 * Setup points to the block and wakeup handlers.  Pass a pointer
	 * to the current screen as pWakeupdata.
	 */
	pScreen->BlockHandler = qtBlockHandler;
	pScreen->WakeupHandler = qtWakeupHandler;
	pScreen->blockData = pScreen;
	pScreen->wakeupData = pScreen;

#ifdef RENDER
	/* Render extension initialization, calls miPictureInit */
	if (!fbPictureInit (pScreen, NULL, 0))
	{
		ErrorF ("qtFinishScreenInitFB - fbPictureInit () failed\n");
		return FALSE;
	}
#endif

	/*
	 * Layer Support
	 */
	/* KDrive does LayerStartInit right after fbPictureInit */
	if (!LayerStartInit (pScreen))
	{
		ErrorF ("qtFinishScreenInitFB - LayerStartInit () failed\n");
		return FALSE;
	}

	/* Not sure what we're adding to shadow, but add it anyway */
	if (!shadowAdd (pScreen, 0, pScreenPriv->pqtShadowUpdate, NULL, 0, 0))
	{
		ErrorF ("qtFinishScreenInitFB - shadowAdd () failed\n");
		return FALSE;
	}

	/* KDrive does LayerFinishInit right after LayerStartInit */
	if (!LayerFinishInit (pScreen))
	{
		ErrorF ("qtFinishScreenInitFB - LayerFinishInit () failed\n");
		return FALSE;
	}

	/* KDrive does LayerCreate right after LayerFinishInit */
	pScreenPriv->pLayer = qtLayerCreate (pScreen);
	if (!pScreenPriv->pLayer)
	{
		ErrorF ("qtFinishScreenInitFB - qtLayerCreate () failed\n");
		return FALSE;
	}

	/* KDrive does RandRInit right after LayerCreate */
#ifdef RANDR
	if (pScreenInfo->dwDepth != 8 && !qtRandRInit (pScreen))
	{
		ErrorF ("qtFinishScreenInitFB - qtRandRInit () failed\n");
		return FALSE;
	}
#endif

	/*
	 * Backing store support should reduce network traffic and increase
	 * performance.
	 */
	miInitializeBackingStore (pScreen);

	/* KDrive does miDCInitialize right after miInitializeBackingStore */
	/* Setup the cursor routines */
#if QTDEBUG
	ErrorF ("qtFinishScreenInitFB - Calling miDCInitialize ()\n");
#endif
	miDCInitialize (pScreen, &g_qtPointerCursorFuncs);

	/* KDrive does winCreateDefColormap right after miDCInitialize */
	/* Create a default colormap */
#if QTDEBUG
	ErrorF ("qtFinishScreenInitFB - Calling winCreateDefColormap ()\n");
#endif
	if (!qtCreateDefColormap (pScreen))
	{
		ErrorF ("qtFinishScreenInitFB - Could not create colormap\n");
		return FALSE;
	}

	/* Handle pseudo-rootless mode */
	if (g_fRootless)
	{
		/* Define the WRAP macro temporarily for local use */
#define WRAP(a) \
    if (pScreen->a) { \
        pScreenPriv->a = pScreen->a; \
    } else { \
        ErrorF("null screen fn " #a "\n"); \
        pScreenPriv->a = NULL; \
    }

		/* Save a pointer to each lower-level window procedure */
		WRAP(CreateWindow);
		WRAP(DestroyWindow);
		WRAP(RealizeWindow);
		WRAP(UnrealizeWindow);
		WRAP(PositionWindow);
		WRAP(ChangeWindowAttributes);
#ifdef SHAPE
		WRAP(SetShape);
#endif

		/* Assign pseudo-rootless window procedures to be top level procedures */
		pScreen->CreateWindow = qtCreateWindowPRootless;
		pScreen->DestroyWindow = qtDestroyWindowPRootless;
		pScreen->PositionWindow = qtPositionWindowPRootless;
		pScreen->ChangeWindowAttributes = qtChangeWindowAttributesPRootless;
		pScreen->RealizeWindow = qtMapWindowPRootless;
		pScreen->UnrealizeWindow = qtUnmapWindowPRootless;
#ifdef SHAPE
		pScreen->SetShape = qtSetShapePRootless;
#endif

		/* Undefine the WRAP macro, as it is not needed elsewhere */
#undef WRAP
	}

#if 0
	/* Handle multi window mode */
	else if (pScreenInfo->fMultiWindow)
	{
		/* Define the WRAP macro temporarily for local use */
#define WRAP(a) \
    if (pScreen->a) { \
        pScreenPriv->a = pScreen->a; \
    } else { \
        ErrorF("null screen fn " #a "\n"); \
        pScreenPriv->a = NULL; \
    }

		/* Save a pointer to each lower-level window procedure */
		WRAP(CreateWindow);
		WRAP(DestroyWindow);
		WRAP(RealizeWindow);
		WRAP(UnrealizeWindow);
		WRAP(PositionWindow);
		WRAP(ChangeWindowAttributes);
		WRAP(ReparentWindow);
		WRAP(RestackWindow);
#ifdef SHAPE
		WRAP(SetShape);
#endif

		/* Assign multi-window window procedures to be top level procedures */
		pScreen->CreateWindow = winCreateWindowMultiWindow;
		pScreen->DestroyWindow = winDestroyWindowMultiWindow;
		pScreen->PositionWindow = winPositionWindowMultiWindow;
		pScreen->ChangeWindowAttributes = winChangeWindowAttributesMultiWindow;
		pScreen->RealizeWindow = winMapWindowMultiWindow;
		pScreen->UnrealizeWindow = winUnmapWindowMultiWindow;
		pScreen->ReparentWindow = winReparentWindowMultiWindow;
		pScreen->RestackWindow = winRestackWindowMultiWindow;
#ifdef SHAPE
		pScreen->SetShape = winSetShapeMultiWindow;
#endif

		/* Undefine the WRAP macro, as it is not needed elsewhere */
#undef WRAP
	}
#endif	/* multiwindow */

	/* Wrap either fb's or shadow's CloseScreen with our CloseScreen */
	pScreenPriv->CloseScreen = pScreen->CloseScreen;
	pScreen->CloseScreen = pScreenPriv->pqtCloseScreen;

	/* Create a mutex for modules in seperate threads to wait for */
	iReturn = pthread_mutex_init (&pScreenPriv->pmServerStarted, NULL);
	if (iReturn != 0)
	{
		ErrorF ("qtFinishScreenInitFB - pthread_mutex_init () failed: %d\n",
			iReturn);
		return FALSE;
	}

	/* Own the mutex for modules in seperate threads */
	iReturn = pthread_mutex_lock (&pScreenPriv->pmServerStarted);
	if (iReturn != 0)
	{
		ErrorF ("qtFinishScreenInitFB - pthread_mutex_lock () failed: %d\n",
			iReturn);
		return FALSE;
	}

	/* Set the ServerStarted flag to false */
	pScreenPriv->fServerStarted = FALSE;

#if QTDEBUG || YES
	if (g_fMultiWindow)
		ErrorF ("qtFinishScreenInitFB - Calling winInitWM.\n");
#endif

	/* Initialize multi window mode */
#if 0
	if (pScreenInfo->fMultiWindow
	    && !winInitWM (&pScreenPriv->pWMInfo,
			   &pScreenPriv->ptWMProc,
			   &pScreenPriv->pmServerStarted,
			   pScreenInfo->dwScreen))
	{
		ErrorF ("qtFinishScreenInitFB - winInitWM () failed.\n");
		return FALSE;
	}
#endif

#if 0	/* NOT YET ### */
#if QTDEBUG || YES
	if (pScreenInfo->fClipboard)
		ErrorF ("qtFinishScreenInitFB - Calling winInitClipboard.\n");
#endif

	/* Initialize the clipboard manager */
	if (pScreenInfo->fClipboard
	    && !winInitClipboard (&pScreenPriv->ptClipboardProc,
				  &pScreenPriv->pmServerStarted,
				  pScreenInfo->dwScreen))
	{
		ErrorF ("qtFinishScreenInitFB - winClipboardInit () failed.\n");
		return FALSE;
	}
#endif

	/* Tell the server that we are enabled */
	pScreenPriv->fEnabled = TRUE;

	/* Tell the server that we have a valid depth */
	pScreenPriv->fBadDepth = FALSE;

#if QTDEBUG || YES
	ErrorF ("qtFinishScreenInitFB - returning\n");
#endif

	return TRUE;
}


/* See Porting Layer Definition - p. 33 */
Bool
qtSaveScreen (ScreenPtr pScreen, int on)
{
	return TRUE;
}
