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
 *		Kensuke Matsuzaki
 * Qt Version:	Takuya Murakami
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/winwindow.c,v 1.6 2003/02/12 15:01:38 alanh Exp $ */

#include "xqt.h"
#include "qtwindow.h"

/*
 * Prototypes for local functions
 */

static int
qtAddRgn (WindowPtr pWindow, pointer data);

static
void
qtUpdateRgn (WindowPtr pWindow);

#ifdef SHAPE
static
void
qtReshape (WindowPtr pWin);
#endif


/* See Porting Layer Definition - p. 37 */
/* See mfb/mfbwindow.c - mfbCreateWindow() */

Bool
qtCreateWindowPRootless (WindowPtr pWin)
{
	Bool			fResult = FALSE;
	qtWindowPriv(pWin);

#if QTDEBUG
	ErrorF ("qtCreateWindowPRootless ()\n");
#endif

	fResult = qtGetScreenPriv(pWin->drawable.pScreen)->CreateWindow(pWin);
  
	pWinPriv->hRgn = NULL;
	/*qtUpdateRgn (pWin);*/
  
	return fResult;
}


/* See Porting Layer Definition - p. 37 */
/* See mfb/mfbwindow.c - mfbDestroyWindow() */

Bool
qtDestroyWindowPRootless (WindowPtr pWin)
{
	Bool			fResult = FALSE;
	qtWindowPriv(pWin);

#if QTDEBUG
	ErrorF ("qtDestroyWindowPRootless ()\n");
#endif

	fResult = qtGetScreenPriv(pWin->drawable.pScreen)->DestroyWindow(pWin);
  
	if (pWinPriv->hRgn != NULL)
	{
		qtDeleteRgn(pWinPriv->hRgn);
		pWinPriv->hRgn = NULL;
	}
  
	qtUpdateRgn (pWin);
  
	return fResult;
}


/* See Porting Layer Definition - p. 37 */
/* See mfb/mfbwindow.c - mfbPositionWindow() */

Bool
qtPositionWindowPRootless (WindowPtr pWin, int x, int y)
{
	Bool			fResult = FALSE;

#if QTDEBUG
	ErrorF ("qtPositionWindowPRootless ()\n");
#endif

	fResult = qtGetScreenPriv(pWin->drawable.pScreen)->PositionWindow(pWin, x, y);
  
	qtUpdateRgn (pWin);
  
	return fResult;
}


/* See Porting Layer Definition - p. 37 */
/* See mfb/mfbwindow.c - mfbChangeWindowAttributes() */

Bool
qtChangeWindowAttributesPRootless (WindowPtr pWin, unsigned long mask)
{
	Bool			fResult = FALSE;

#if QTDEBUG
	ErrorF ("qtChangeWindowAttributesPRootless ()\n");
#endif

	fResult = qtGetScreenPriv(pWin->drawable.pScreen)->ChangeWindowAttributes(pWin, mask);
  
	qtUpdateRgn (pWin);
  
	return fResult;
}


/* See Porting Layer Definition - p. 37
 * Also referred to as UnrealizeWindow
 */

Bool
qtUnmapWindowPRootless (WindowPtr pWin)
{
	Bool			fResult = FALSE;
	qtWindowPriv(pWin);

#if QTDEBUG
	ErrorF ("qtUnmapWindowPRootless ()\n");
#endif

	fResult = qtGetScreenPriv(pWin->drawable.pScreen)->UnrealizeWindow(pWin);
  
	if (pWinPriv->hRgn != NULL)
	{
		qtDeleteRgn(pWinPriv->hRgn);
		pWinPriv->hRgn = NULL;
	}
  
	qtUpdateRgn (pWin);
  
	return fResult;
}


/* See Porting Layer Definition - p. 37
 * Also referred to as RealizeWindow
 */

Bool
qtMapWindowPRootless (WindowPtr pWin)
{
	Bool			fResult = FALSE;

#if QTDEBUG
	ErrorF ("qtMapWindowPRootless ()\n");
#endif

	fResult = qtGetScreenPriv(pWin->drawable.pScreen)->RealizeWindow(pWin);
  
	qtReshape (pWin);
  
	qtUpdateRgn (pWin);
  
	return fResult;
}


#ifdef SHAPE
void
qtSetShapePRootless (WindowPtr pWin)
{
#if QTDEBUG
	ErrorF ("qtSetShapePRootless ()\n");
#endif

	qtGetScreenPriv(pWin->drawable.pScreen)->SetShape(pWin);
  
	qtReshape (pWin);
	qtUpdateRgn (pWin);
  
	return;
}
#endif


/*
 * Local function for adding a region to the Windows window region
 */

static
int
qtAddRgn (WindowPtr pWin, pointer data)
{
	int		iX, iY, iWidth, iHeight, iBorder;
	QRegionP	hRgn = *(QRegionP *)data;	// ###!!!!!
	QRegionP	hRgnWin;
	qtWindowPriv(pWin);
  
	/* If pWin is not Root */
	if (pWin->parent != NULL) 
	{
#if QTDEBUG
		ErrorF ("qtAddRgn ()\n");
#endif
		if (pWin->mapped)
		{
			iBorder = wBorderWidth (pWin);
	  
			iX = pWin->drawable.x - iBorder;
			iY = pWin->drawable.y - iBorder;
	  
			iWidth = pWin->drawable.width + iBorder * 2;
			iHeight = pWin->drawable.height + iBorder * 2;
	  
			hRgnWin = qtCreateRectRgn (0, 0, iWidth, iHeight);
	  
			if (hRgnWin == NULL)
			{
				ErrorF ("qtAddRgn - qtCreateRectRgn () failed\n");
				ErrorF ("  Rect %d %d %d %d\n",
					iX, iY, iX + iWidth, iY + iHeight);
			}
	  
			if (pWinPriv->hRgn)
			{
				qtCombineRgnAnd(hRgnWin, pWinPriv->hRgn);
			}
	  
			qtOffsetRgn (hRgnWin, iX, iY);

			qtCombineRgnOr(hRgn, hRgnWin);
	  
			qtDeleteRgn(hRgnWin);
		}
		return WT_DONTWALKCHILDREN;
	}
	else
	{
		return WT_WALKCHILDREN;
	}
}


/*
 * Local function to update the Windows window's region
 */

static
void
qtUpdateRgn (WindowPtr pWin)
{
	QRegionP	hRgn = qtCreateRectRgn (0, 0, 0, 0);
  
	if (hRgn != NULL)
	{
		WalkTree (pWin->drawable.pScreen, qtAddRgn, &hRgn);
		qtSetWindowRgn (hRgn);
	}
	else
	{
		ErrorF ("qtUpdateRgn - qtCreateRectRgn failed.\n");
	}
}


#ifdef SHAPE
static
void
qtReshape (WindowPtr pWin)
{
	int		nRects;
	ScreenPtr	pScreen = pWin->drawable.pScreen;
	RegionRec	rrNewShape;
	BoxPtr		pShape, pRects, pEnd;
	QRegionP	hRgn, hRgnRect;
	qtWindowPriv(pWin);

#if QTDEBUG
	ErrorF ("qtReshape ()\n");
#endif

	/* Bail if the window is the root window */
	if (pWin->parent == NULL)
		return;

	/* Bail if the window is not top level */
	if (pWin->parent->parent != NULL)
		return;

	/* Free any existing window region stored in the window privates */
	if (pWinPriv->hRgn != NULL)
	{
		qtDeleteRgn(pWinPriv->hRgn);
		pWinPriv->hRgn = NULL;
	}
  
	/* Bail if the window has no bounding region defined */
	if (!wBoundingShape (pWin))
		return;

	REGION_INIT(pScreen, &rrNewShape, NullBox, 0);
	REGION_COPY(pScreen, &rrNewShape, wBoundingShape(pWin));
	REGION_TRANSLATE(pScreen, &rrNewShape, pWin->borderWidth,
			 pWin->borderWidth);
  
	nRects = REGION_NUM_RECTS(&rrNewShape);
	pShape = REGION_RECTS(&rrNewShape);
  
	if (nRects > 0)
	{
		/* Create initial empty Windows region */
		hRgn = qtCreateRectRgn (0, 0, 0, 0);

		/* Loop through all rectangles in the X region */
		for (pRects = pShape, pEnd = pShape + nRects; pRects < pEnd; pRects++)
		{
			/* Create a Windows region for the X rectangle */
			hRgnRect = qtCreateRectRgn (pRects->x1, pRects->y1,
						  pRects->x2, pRects->y2);
			if (hRgnRect == NULL)
			{
				ErrorF("qtReshape - qtCreateRectRgn() failed\n");
			}

			/* Merge the Windows region with the accumulated region */
			qtCombineRgnOr(hRgn, hRgnRect);

			/* Delete the temporary Windows region */
			qtDeleteRgn(hRgnRect);
		}
      
		/* Save a handle to the composite region in the window privates */
		pWinPriv->hRgn = hRgn;
	}

	REGION_UNINIT(pScreen, &rrNewShape);
  
	return;
}
#endif
