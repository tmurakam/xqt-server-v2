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
 * Authors:	Harold L Hunt II
 *		Takuya Murakami (qt version)
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/winshadgdi.c,v 1.22 2003/02/12 15:01:38 alanh Exp $ */

#include "xqt.h"

/*
 * Local function prototypes
 */

//BOOL CALLBACK
//qtRedrawAllProcShadow (HWND hwnd, LPARAM lParam);

static int qtBpp;		/* BPP */

/*
 * Redraw all ---?
 */

#if 0	/* ### MultiWindow モードのみ */
BOOL CALLBACK
qtRedrawAllProcShadow (HWND hwnd, LPARAM lParam)
{
	char strClassName[100];

	if (GetClassName (hwnd, strClassName, 100))
	{
		if(strcmp (WINDOW_CLASS_X, strClassName) == 0)
		{
			InvalidateRect (hwnd, NULL, FALSE);
			UpdateWindow (hwnd);
		}
	}
	return TRUE;
}
#endif

/*
 * Allocate a DIB for the shadow framebuffer GDI server
 */
Bool
qtAllocateFBShadow (ScreenPtr pScreen)
{
	qtScreenPriv(pScreen);
	qtScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;

	/* ### Shadow image を作成する */
	pScreenPriv->hImageShadow = qtCreateShadowImage(
		&pScreenInfo->dwWidth,
		&pScreenInfo->dwHeight,
		&qtBpp,
		(void **)&pScreenInfo->pfb,
		&pScreenInfo->dwStride);

	/* Determine our color masks */
	/* ### */

	/* Redraw all windows */
#if 0	/* ### */
	if (pScreenInfo->fMultiWindow) EnumWindows(winRedrawAllProcShadowGDI, 0);
#endif

	return TRUE;
}


/*
 * Blit the damaged regions of the shadow fb to the screen
 */

void
qtShadowUpdate (ScreenPtr pScreen, 
		    shadowBufPtr pBuf)
{
	qtScreenPriv(pScreen);
	qtScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
	RegionPtr		damage = &pBuf->damage;
	DWORD			dwBox = REGION_NUM_RECTS (damage);
	BoxPtr			pBox = REGION_RECTS (damage);
	int			x, y, w, h;

	FbBits			*shaBase;
	FbStride		shaStride;
	int			shaBpp;
	int			shaXoff, shaYoff; /* assumed to be zero */

	/*
	 * Return immediately if the app is not active
	 * and we are fullscreen, or if we have a bad display depth
	 */
	if ((!pScreenPriv->fActive && pScreenInfo->fFullScreen)
	    || pScreenPriv->fBadDepth) return;

	/*
	 * get shadow pixmap
	 */
	fbGetDrawable(&pBuf->pPixmap->drawable, shaBase, shaStride, shaBpp, shaXoff, shaYoff);

	/*
	 * Handle small regions with multiple blits,
	 * handle large regions by creating a clipping region and 
	 * doing a single blit constrained to that clipping region.
	 */
	if (pScreenInfo->dwClipUpdatesNBoxes == 0
	    || dwBox < pScreenInfo->dwClipUpdatesNBoxes)
	{
		/* Loop through all boxes in the damaged region */
		while (dwBox--)
		{
			/*
			 * Calculate x offset, y offset, width, and height for
			 * current damage box
			 */
			x = pBox->x1;
			y = pBox->y1;
			w = pBox->x2 - pBox->x1;
			h = pBox->y2 - pBox->y1;

			/* redraw */
			qtRedrawScreen(x, y, w, h,
				       shaBase, shaStride*sizeof(FbBits), shaBpp,
				       pBuf->randr);

			/* Get a pointer to the next box */
			++pBox;
		}
	}
	else
	{
		int x1, y1, x2, y2;

		/* 
		   複数の damage box を merge して BitBlt してやる必要がある。
		   WIN32の場合は region を使うことができるが、Qt にはないので、
		   全ボックスを囲むボックスを計算しないといけない。
		*/
		x1 = pBox->x1; y1 = pBox->y1;
		x2 = pBox->x2; y2 = pBox->y2;

		dwBox--;
		pBox++;
		while (dwBox--)
		{
			if (pBox->x1 < x1) x1 = pBox->x1;
			if (pBox->x2 > x2) x2 = pBox->x2;
			if (pBox->y1 < y1) y1 = pBox->y1;
			if (pBox->y2 > y2) y2 = pBox->y2;
			pBox++;
		}
      
		/* redraw */
		qtRedrawScreen(x, y, w, h, 
			       shaBase, shaStride*sizeof(FbBits), shaBpp,
			       pBuf->randr);
	}

	/* Redraw all windows */
#if 0	/* ### */	
	if (pScreenInfo->fMultiWindow) EnumWindows(winRedrawAllProcShadow, 0);
#endif
}


/* See Porting Layer Definition - p. 33 */
/*
 * We wrap whatever CloseScreen procedure was specified by fb;
 * a pointer to said procedure is stored in our privates.
 */

Bool
qtCloseScreenShadow (int nIndex, ScreenPtr pScreen)
{
	qtScreenPriv(pScreen);
	qtScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
	Bool			fReturn;

#if QTDEBUG
	ErrorF ("qtCloseScreenShadow - Freeing screen resources\n");
#endif

	/* Flag that the screen is closed */
	pScreenPriv->fClosed = TRUE;
	pScreenPriv->fActive = FALSE;

	/* Call the wrapped CloseScreen procedure */
	pScreen->CloseScreen = pScreenPriv->CloseScreen;
	fReturn = (*pScreen->CloseScreen) (nIndex, pScreen);

	/* Kill our window */
#if 0
	if (pScreenPriv->hwndScreen)
	{
		DestroyWindow (pScreenPriv->hwndScreen);
		pScreenPriv->hwndScreen = NULL;
	}
#endif

	/* Destroy the thread startup mutex */
	//pthread_mutex_destroy (&pScreenPriv->pmServerStarted);

	/* Invalidate our screeninfo's pointer to the screen */
	pScreenInfo->pScreen = NULL;

	/* Invalidate the ScreenInfo's fb pointer */
	pScreenInfo->pfb = NULL;

	/* Free the screen privates for this screen */
	free ((pointer) pScreenPriv);

	return fReturn;
}


/*
 * Tell mi what sort of visuals we need.
 * 
 * Generally we only need one visual, as our screen can only
 * handle one format at a time, I believe.  You may want
 * to verify that last sentence.
 */

Bool
qtInitVisualsShadow (ScreenPtr pScreen)
{
	qtScreenPriv(pScreen);
	qtScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;

	/* Display debugging information */
	ErrorF ("qtInitVisualsShadow - Masks %08x %08x %08x BPRGB %d d %d "
		"bpp %d\n",
		pScreenPriv->dwRedMask,
		pScreenPriv->dwGreenMask,
		pScreenPriv->dwBlueMask,
		pScreenPriv->dwBitsPerRGB,
		pScreenInfo->dwDepth,
		pScreenInfo->dwBPP);

	/* Create a single visual according to the Windows screen depth */
	switch (pScreenInfo->dwDepth)
	{
	case 32:
	case 24:
	case 16:
	case 15:
#if defined(XFree86Server)
		/* Setup the real visual */
		if (!miSetVisualTypesAndMasks (pScreenInfo->dwDepth,
					       TrueColorMask,
					       pScreenPriv->dwBitsPerRGB,
					       -1,
					       pScreenPriv->dwRedMask,
					       pScreenPriv->dwGreenMask,
					       pScreenPriv->dwBlueMask))
		{
			ErrorF ("qtInitVisualsShadow - miSetVisualTypesAndMasks "
				"failed\n");
			return FALSE;
		}
#else /* XFree86Server */
      /* Setup the real visual */
		if (!fbSetVisualTypesAndMasks (pScreenInfo->dwDepth,
					       TrueColorMask,
					       pScreenPriv->dwBitsPerRGB,
					       pScreenPriv->dwRedMask,
					       pScreenPriv->dwGreenMask,
					       pScreenPriv->dwBlueMask))
		{
			ErrorF ("qtInitVisualsShadow - fbSetVisualTypesAndMasks "
				"failed for TrueColor\n");
			return FALSE;
		}
#endif /* XFree86Server */
		break;

	default:
		ErrorF ("qtInitVisualsShadow - Unknown screen depth\n");
		return FALSE;
	}

#if QTDEBUG
	ErrorF ("qtInitVisualsShadow - Returning\n");
#endif

	return TRUE;
}

/*
 * Do any engine-specific appliation-activation processing
 */

Bool
winActivateAppShadow (ScreenPtr pScreen)
{
#if 0	/* ### NOT YET */
	winScreenPriv(pScreen);
	winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;

#if QTDEBUG
	ErrorF ("qtActivateAppShadow\n");
#endif

	/*
	 * Are we active?
	 * Are we fullscreen?
	 */
	if (pScreenPriv->fActive
	    && pScreenInfo->fFullScreen)
	{
		/*
		 * Activating, attempt to bring our window 
		 * to the top of the display
		 */
		ShowWindow (pScreenPriv->hwndScreen, SW_RESTORE);
	}
	else if (!pScreenPriv->fActive
		 && pScreenInfo->fFullScreen)
	{
		/*
		 * Deactivating, stuff our window onto the
		 * task bar.
		 */
		ShowWindow (pScreenPriv->hwndScreen, SW_MINIMIZE);
	}

#if QTDEBUG
	ErrorF ("qtActivateAppShadow - Returning\n");
#endif

#endif
	return TRUE;
}


/*
 * Realize the currently installed colormap
 */

Bool
qtRealizeInstalledPaletteShadow (ScreenPtr pScreen)
{
#if 0	/* ### NOT YET */
	winScreenPriv(pScreen);
	winPrivCmapPtr	pCmapPriv = NULL;

#if QTDEBUG
	ErrorF ("qtRealizeInstalledPaletteShadow\n");
#endif

	/* Don't do anything if there is not a colormap */
	if (pScreenPriv->pcmapInstalled == NULL)
	{
#if QTDEBUG
		ErrorF ("qtRealizeInstalledPaletteShadow - No colormap "
			"installed\n");
#endif
		return TRUE;
	}

	pCmapPriv = winGetCmapPriv (pScreenPriv->pcmapInstalled);
  
	/* Realize our palette for the screen */
	if (RealizePalette (pScreenPriv->hdcScreen) == _ERROR)
	{
		ErrorF ("qtRealizeInstalledPaletteShadow - RealizePalette () "
			"failed\n");
		return FALSE;
	}
  
	/* Set the DIB color table */
	if (SetDIBColorTable (pScreenPriv->hdcShadow,
			      0,
			      WIN_NUM_PALETTE_ENTRIES,
			      pCmapPriv->rgbColors) == 0)
	{
		ErrorF ("qtRealizeInstalledPaletteShadow - SetDIBColorTable () "
			"failed\n");
		return FALSE;
	}
#endif  
	return TRUE;
}


/*
 * Install the specified colormap
 */

Bool
qtInstallColormapShadow (ColormapPtr pColormap)
{
#if 0	/* ### NOT YET */
	ScreenPtr		pScreen = pColormap->pScreen;
	winScreenPriv(pScreen);
	winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
	winCmapPriv(pColormap);

	/*
	 * Tell Windows to install the new colormap
	 */
	if (SelectPalette (pScreenPriv->hdcScreen,
			   pCmapPriv->hPalette,
			   FALSE) == NULL)
	{
		ErrorF ("qtInstallColormapShadow - SelectPalette () failed\n");
		return FALSE;
	}
      
	/* Realize the palette */
	if (GDI_ERROR == RealizePalette (pScreenPriv->hdcScreen))
	{
		ErrorF ("qtInstallColormapShadowGDI - RealizePalette () failed\n");
		return FALSE;
	}

	/* Set the DIB color table */
	if (SetDIBColorTable (pScreenPriv->hdcShadow,
			      0,
			      WIN_NUM_PALETTE_ENTRIES,
			      pCmapPriv->rgbColors) == 0)
	{
		ErrorF ("qtInstallColormapShadowGDI - SetDIBColorTable () failed\n");
		return FALSE;
	}

	/* Redraw the whole window, to take account for the new colors */
	BitBlt (pScreenPriv->hdcScreen,
		0, 0,
		pScreenInfo->dwWidth, pScreenInfo->dwHeight,
		pScreenPriv->hdcShadow,
		0, 0,
		SRCCOPY);

	/* Save a pointer to the newly installed colormap */
	pScreenPriv->pcmapInstalled = pColormap;

	/* Redraw all windows */
	if (pScreenInfo->fMultiWindow) EnumWindows(winRedrawAllProcShadow, 0);
#endif

	return TRUE;
}


/*
 * Store the specified colors in the specified colormap
 */

Bool
qtStoreColorsShadow (ColormapPtr pColormap,
			 int ndef,
			 xColorItem *pdefs)
{
#if 0	/* ### NOT YET */
	ScreenPtr		pScreen = pColormap->pScreen;
	winScreenPriv(pScreen);
	winCmapPriv(pColormap);
	ColormapPtr curpmap = pScreenPriv->pcmapInstalled;
  
	/* Put the X colormap entries into the Windows logical palette */
	if (SetPaletteEntries (pCmapPriv->hPalette,
			       pdefs[0].pixel,
			       ndef,
			       pCmapPriv->peColors + pdefs[0].pixel) == 0)
	{
		ErrorF ("qtStoreColorsShadow - SetPaletteEntries () failed\n");
		return FALSE;
	}

	/* Don't install the Windows palette if the colormap is not installed */
	if (pColormap != curpmap)
	{
		return TRUE;
	}

	/* Try to install the newly modified colormap */
	if (!winInstallColormapShadow (pColormap))
	{
		ErrorF ("qtInstallColormapShadow - winInstallColormapShadow "
			"failed\n");
		return FALSE;
	}
#endif

	return TRUE;
}


/*
 * Colormap initialization procedure
 */

Bool
qtCreateColormapShadow (ColormapPtr pColormap)
{
#if 0	/* ### NOT YET */
	LPLOGPALETTE		lpPaletteNew = NULL;
	DWORD			dwEntriesMax;
	VisualPtr		pVisual;
	HPALETTE		hpalNew = NULL;
	winCmapPriv(pColormap);

	/* Get a pointer to the visual that the colormap belongs to */
	pVisual = pColormap->pVisual;

	/* Get the maximum number of palette entries for this visual */
	dwEntriesMax = pVisual->ColormapEntries;

	/* Allocate a Windows logical color palette with max entries */
	lpPaletteNew = malloc (sizeof (LOGPALETTE)
			       + (dwEntriesMax - 1) * sizeof (PALETTEENTRY));
	if (lpPaletteNew == NULL)
	{
		ErrorF ("qtCreateColormapShadow - Couldn't allocate palette "
			"with %d entries\n",
			dwEntriesMax);
		return FALSE;
	}

	/* Zero out the colormap */
	ZeroMemory (lpPaletteNew, sizeof (LOGPALETTE)
		    + (dwEntriesMax - 1) * sizeof (PALETTEENTRY));
  
	/* Set the logical palette structure */
	lpPaletteNew->palVersion = 0x0300;
	lpPaletteNew->palNumEntries = dwEntriesMax;

	/* Tell Windows to create the palette */
	hpalNew = CreatePalette (lpPaletteNew);
	if (hpalNew == NULL)
	{
		ErrorF ("qtCreateColormapShadow - CreatePalette () failed\n");
		free (lpPaletteNew);
		return FALSE;
	}

	/* Save the Windows logical palette handle in the X colormaps' privates */
	pCmapPriv->hPalette = hpalNew;

	/* Free the palette initialization memory */
	free (lpPaletteNew);

#endif
	return TRUE;
}


/*
 * Colormap destruction procedure
 */

Bool
qtDestroyColormapShadow (ColormapPtr pColormap)
{
#if 0	/* ### NOT YET */
	qtScreenPriv(pColormap->pScreen);
	qtCmapPriv(pColormap);

	/*
	 * Is colormap to be destroyed the default?
	 *
	 * Non-default colormaps should have had winUninstallColormap
	 * called on them before we get here.  The default colormap
	 * will not have had winUninstallColormap called on it.  Thus,
	 * we need to handle the default colormap in a special way.
	 */
	if (pColormap->flags & IsDefault)
	{
#if QTDEBUG
		ErrorF ("qtDestroyColormapShadow - Destroying default "
			"colormap\n");
#endif
      
		/*
		 * FIXME: Walk the list of all screens, popping the default
		 * palette out of each screen device context.
		 */
      
		/* Pop the palette out of the device context */
		SelectPalette (pScreenPriv->hdcScreen,
			       GetStockObject (DEFAULT_PALETTE),
			       FALSE);

		/* Clear our private installed colormap pointer */
		pScreenPriv->pcmapInstalled = NULL;
	}
  
	/* Try to delete the logical palette */
	if (DeleteObject (pCmapPriv->hPalette) == 0)
	{
		ErrorF ("qtDestroyColormap - DeleteObject () failed\n");
		return FALSE;
	}
  
	/* Invalidate the colormap privates */
	pCmapPriv->hPalette = NULL;
#endif
	return TRUE;
}


/*
 * Set engine specific funtions
 */

Bool
qtInitializeShadow (ScreenPtr pScreen)
{
	qtScreenPriv(pScreen);
	qtScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;

	if (qtAllocateFBShadow(pScreen) == FALSE) {
		ErrorF ("qtFinishScreenInitFB - Could not allocate framebuffer\n");
		return FALSE;
	}

	pScreenInfo->dwBPP   = qtBpp;
	pScreenInfo->dwDepth = qtBpp;

	if (qtBpp == 16) {
		pScreenPriv->dwBitsPerRGB = 6;
		pScreenPriv->dwRedMask    = 0xf800;	// 5
		pScreenPriv->dwGreenMask  = 0x07e0;	// 6
		pScreenPriv->dwBlueMask   = 0x001f;	// 5
	} else {
		pScreenPriv->dwBitsPerRGB = 8;
		pScreenPriv->dwRedMask   = 0xff0000;
		pScreenPriv->dwGreenMask = 0x00ff00;
		pScreenPriv->dwBlueMask  = 0x0000ff;
	}

	/* Set our pointers */
	pScreenPriv->pqtShadowUpdate = qtShadowUpdate;
	pScreenPriv->pqtCloseScreen = qtCloseScreenShadow;
	pScreenPriv->pqtInitVisuals = qtInitVisualsShadow;
/*
	if (pScreenInfo->fFullScreen)
		pScreenPriv->pqtCreateBoundingWindow = qtCreateBoundingWindowFullScreen;
	else
		pScreenPriv->pqtCreateBoundingWindow = qtCreateBoundingWindowWindowed;
*/

	pScreenPriv->pqtRealizeInstalledPalette = 
		qtRealizeInstalledPaletteShadow;
	pScreenPriv->pqtInstallColormap = qtInstallColormapShadow;
	pScreenPriv->pqtStoreColors = qtStoreColorsShadow;
	pScreenPriv->pqtCreateColormap = qtCreateColormapShadow;
	pScreenPriv->pqtDestroyColormap = qtDestroyColormapShadow;
	pScreenPriv->pqtHotKeyAltTab = (qtHotKeyAltTabProcPtr) (void (*)())NoopDDA;
	pScreenPriv->pqtCreatePrimarySurface
		= (qtCreatePrimarySurfaceProcPtr) (void (*)())NoopDDA;
	pScreenPriv->pqtReleasePrimarySurface
		= (qtReleasePrimarySurfaceProcPtr) (void (*)())NoopDDA;

	return TRUE;
}
