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
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/winlayer.c,v 1.9 2002/10/31 23:04:39 alanh Exp $ */

#include "xqt.h"

#include "mivalidate.h"

static Bool
qtRandRRotateMain(ScreenPtr		pScreen,
		  Rotation		randr);
static void  qtSetRootClip (ScreenPtr pScreen, BOOL enable);

/* RandR rotation */
Rotation	g_randr = RR_Rotate_0;


/*
 * Create initial layer.  Cygwin only needs one initial layer.
 */

LayerPtr
qtLayerCreate (ScreenPtr pScreen)
{
	qtScreenPriv(pScreen);
	qtScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;

	/* set new height/width */
	if (g_randr == RR_Rotate_0 || g_randr == RR_Rotate_180) {
		pScreen->width    = pScreenInfo->dwWidth;
		pScreen->height   = pScreenInfo->dwHeight;
		pScreen->mmWidth  = pScreenInfo->dwWidth / QT_DEFAULT_DPI;
		pScreen->mmHeight = pScreenInfo->dwHeight / QT_DEFAULT_DPI;
	} else {
		pScreen->width    = pScreenInfo->dwHeight;
		pScreen->height   = pScreenInfo->dwWidth;
		pScreen->mmWidth  = pScreenInfo->dwHeight / QT_DEFAULT_DPI;
		pScreen->mmHeight = pScreenInfo->dwWidth / QT_DEFAULT_DPI;
	}

	/* create layer */
	return LayerCreate (pScreen,
			    LAYER_SHADOW,
			    pScreenInfo->dwBPP,
			    0,
			    qtShadowUpdate,
			    0,
			    g_randr, /* Rotate */
			    0);
}



/*
 * Used as a function parameter to WalkTree.
 */

int
qtLayerAdd (WindowPtr pWindow, pointer value)
{
	ScreenPtr		pScreen = pWindow->drawable.pScreen;
	LayerPtr		pLayer = (LayerPtr) value;
  
	if (!LayerWindowAdd (pScreen, pLayer, pWindow))
		return WT_STOPWALKING;
  
	return WT_WALKCHILDREN;
}


/*
 * Used as a function parameter to WalkTree.
 */

int
qtLayerRemove (WindowPtr pWindow, pointer value)
{
	ScreenPtr		pScreen = pWindow->drawable.pScreen;
	LayerPtr		pLayer = (LayerPtr) value;
  
	LayerWindowRemove (pScreen, pLayer, pWindow);

	return WT_WALKCHILDREN;
}

#ifdef RANDR
/*
 * Answer queries about the RandR features supported.
 */

Bool
qtRandRGetInfo (ScreenPtr pScreen, Rotation *pRotations)
{
	qtScreenPriv(pScreen);
	qtScreenInfo			*pScreenInfo = pScreenPriv->pScreenInfo;
	RRScreenSizePtr		pSize;

	ErrorF ("qtRandRGetInfo ()\n");

	*pRotations = RR_Rotate_0|RR_Rotate_270;	// 0 and 270 only
  
#if 0
	/* Check for something naughty.  Don't know what exactly... */
	for (n = 0; n < pScreen->numDepths; n++)
		if (pScreen->allowedDepths[n].numVids)
			break;
	if (n == pScreen->numDepths)
		return FALSE;
#endif
  
	/*
	 * Register supported sizes.  This can be called many times, but
	 * we only support one size for now.
	 */
	pSize = RRRegisterSize (pScreen,
				pScreenInfo->dwWidth,
				pScreenInfo->dwHeight,
				pScreenInfo->dwWidth / QT_DEFAULT_DPI,
				pScreenInfo->dwHeight / QT_DEFAULT_DPI);
  
	/* Tell RandR what the current config is */
	RRSetCurrentConfig (pScreen, g_randr, 0, pSize);
  
	return TRUE;
}


/*
 * Configure which RandR features are supported.
 */

ScreenPtr g_pScreen = NULL;	/* save */

void qtRandRRotate(int rot)
{
	ErrorF ("qtRandRRotate(%d)\n", rot);

	if (g_pScreen == NULL) {
		/* screen not initialized yet. */
		switch (rot) {
		case 270:
			g_randr = RR_Rotate_270;
			break;

		case 0:
		default:
			g_randr = RR_Rotate_0;
			break;
		}
	} else {
		/* execute xrandr */
		char *cmdline;
		switch (rot) {
		case 270:
			cmdline = "xrandr -o 3 &";
			break;

		case 0:
		default:
			cmdline = "xrandr -o 0 &";
			break;
		}
		system(cmdline);
	}
}


Bool
qtRandRSetConfig (ScreenPtr		pScreen,
		  Rotation		randr,
		  int			rate,
		  RRScreenSizePtr	pSize)
{
	ErrorF ("qtRandRSetConfig (%d:%d:%d:%d)\n", randr, pSize->id, pSize->width, pSize->height);
	qtRandRRotateMain(pScreen, randr);
	return TRUE;
}

static Bool
qtRandRRotateMain(ScreenPtr		pScreen,
		  Rotation		randr)
{
	qtScreenPriv(pScreen);

	/*
	 * The only thing that can change is rotation
	 */
	if (g_randr != randr) {
		LayerPtr	pNewLayer;
		int		oldrandr = g_randr;
		int		oldwidth = pScreen->width;
		int		oldheight = pScreen->height;

		qtSetRootClip(pScreen, FALSE);
	
		g_randr = randr;
		pNewLayer = qtLayerCreate (pScreen);
		if (!pNewLayer)
		{
			g_randr = oldrandr;
		}
		if (WalkTree (pScreen, qtLayerAdd, (pointer) pNewLayer) == WT_STOPWALKING)
		{
			WalkTree (pScreen, qtLayerRemove, (pointer) pNewLayer);
			LayerDestroy (pScreen, pNewLayer);
			g_randr = oldrandr;
			pScreen->width = oldwidth;
			pScreen->height = oldheight;
			qtSetRootClip(pScreen, TRUE);
			return FALSE;
		}
		WalkTree (pScreen, qtLayerRemove, (pointer) pScreenPriv->pLayer);
		LayerDestroy (pScreen, pScreenPriv->pLayer);
		pScreenPriv->pLayer = pNewLayer;
		//KdSetSubpixelOrder (pScreen, pScreenPriv->randr);

		qtSetRootClip(pScreen, TRUE);
	}
	return TRUE;
}


/*
 * Initialize the RandR layer.
 */

Bool
qtRandRInit (ScreenPtr pScreen)
{
	rrScrPrivPtr		pRRScrPriv;

	ErrorF ("qtRandRInit ()\n");

	g_pScreen = pScreen;

	if (!RRScreenInit (pScreen))
	{
		ErrorF ("qtRandRInit () - RRScreenInit () failed\n");
		return FALSE;
	}

	/* Set some RandR function pointers */
	pRRScrPriv = rrGetScrPriv (pScreen);
	pRRScrPriv->rrGetInfo = qtRandRGetInfo;
	pRRScrPriv->rrSetConfig = qtRandRSetConfig;

	return TRUE;
}

static void
qtSetRootClip (ScreenPtr pScreen, BOOL enable)
{
#ifndef FB_OLD_SCREEN
	WindowPtr	pWin = WindowTable[pScreen->myNum];
	WindowPtr	pChild;
	Bool	WasViewable;
	Bool	anyMarked;
	RegionPtr	pOldClip, bsExposed;
#ifdef DO_SAVE_UNDERS
	Bool	dosave = FALSE;
#endif
	WindowPtr   pLayerWin;
	BoxRec	box;

	if (!pWin)
		return;
	WasViewable = (Bool)(pWin->viewable);
	if (WasViewable)
	{
		for (pChild = pWin->firstChild; pChild; pChild = pChild->nextSib)
		{
			(void) (*pScreen->MarkOverlappedWindows)(pChild,
								 pChild,
								 &pLayerWin);
		}
		(*pScreen->MarkWindow) (pWin);
		anyMarked = TRUE;
		if (pWin->valdata)
		{
			if (HasBorder (pWin))
			{
				RegionPtr	borderVisible;

				borderVisible = REGION_CREATE(pScreen, NullBox, 1);
				REGION_SUBTRACT(pScreen, borderVisible,
						&pWin->borderClip, &pWin->winSize);
				pWin->valdata->before.borderVisible = borderVisible;
			}
			pWin->valdata->before.resized = TRUE;
		}
	}

	if (enable)
	{
		box.x1 = 0;
		box.y1 = 0;
		box.x2 = pScreen->width;
		box.y2 = pScreen->height;
		pWin->drawable.width = pScreen->width;
		pWin->drawable.height = pScreen->height;
		REGION_INIT (pScreen, &pWin->winSize, &box, 1);
		REGION_INIT (pScreen, &pWin->borderSize, &box, 1);
		REGION_RESET(pScreen, &pWin->borderClip, &box);
		REGION_BREAK (pWin->drawable.pScreen, &pWin->clipList);
	}
	else
	{
		REGION_EMPTY(pScreen, &pWin->borderClip);
		REGION_BREAK (pWin->drawable.pScreen, &pWin->clipList);
	}
    
	ResizeChildrenWinSize (pWin, 0, 0, 0, 0);
    
	if (WasViewable)
	{
		if (pWin->backStorage)
		{
			pOldClip = REGION_CREATE(pScreen, NullBox, 1);
			REGION_COPY(pScreen, pOldClip, &pWin->clipList);
		}

		if (pWin->firstChild)
		{
			anyMarked |= (*pScreen->MarkOverlappedWindows)(pWin->firstChild,
								       pWin->firstChild,
								       (WindowPtr *)NULL);
		}
		else
		{
			(*pScreen->MarkWindow) (pWin);
			anyMarked = TRUE;
		}

#ifdef DO_SAVE_UNDERS
		if (DO_SAVE_UNDERS(pWin))
		{
			dosave = (*pScreen->ChangeSaveUnder)(pLayerWin, pLayerWin);
		}
#endif /* DO_SAVE_UNDERS */

		if (anyMarked)
			(*pScreen->ValidateTree)(pWin, NullWindow, VTOther);
	}

	if (pWin->backStorage &&
	    ((pWin->backingStore == Always) || WasViewable))
	{
		if (!WasViewable)
			pOldClip = &pWin->clipList; /* a convenient empty region */
		bsExposed = (*pScreen->TranslateBackingStore)
			(pWin, 0, 0, pOldClip,
			 pWin->drawable.x, pWin->drawable.y);
		if (WasViewable)
			REGION_DESTROY(pScreen, pOldClip);
		if (bsExposed)
		{
			RegionPtr	valExposed = NullRegion;
    
			if (pWin->valdata)
				valExposed = &pWin->valdata->after.exposed;
			(*pScreen->WindowExposures) (pWin, valExposed, bsExposed);
			if (valExposed)
				REGION_EMPTY(pScreen, valExposed);
			REGION_DESTROY(pScreen, bsExposed);
		}
	}
	if (WasViewable)
	{
		if (anyMarked)
			(*pScreen->HandleExposures)(pWin);
#ifdef DO_SAVE_UNDERS
		if (dosave)
			(*pScreen->PostChangeSaveUnder)(pLayerWin, pLayerWin);
#endif /* DO_SAVE_UNDERS */
		if (anyMarked && pScreen->PostValidateTree)
			(*pScreen->PostValidateTree)(pWin, NullWindow, VTOther);
	}
	if (pWin->realized)
		WindowsRestructured ();
#endif	/* !FB_OLD_SCREEN */
}

#else	/* RANDR */
Bool qtRandRInit(ScreenPtr pScreen)
{
	return TRUE;
}
#endif	/* RANDR */

