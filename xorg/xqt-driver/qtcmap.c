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
/* $XFree86: xc/programs/Xserver/hw/xwin/wincmap.c,v 1.10 2002/10/17 08:18:22 alanh Exp $ */

#include "xqt.h"


/* See Porting Layer Definition - p. 30 */
/*
 * Walk the list of installed colormaps, filling the pmaps list
 * with the resource ids of the installed maps, and return
 * a count of the total number of installed maps.
 */
int
qtListInstalledColormaps (ScreenPtr pScreen, Colormap *pmaps)
{
	/* ### */
	return 1;
}


/* See Porting Layer Definition - p. 30 */
/* See Programming Windows - p. 663 */
void
qtInstallColormap (ColormapPtr pColormap)
{
	/* ### */
}


/* See Porting Layer Definition - p. 30 */
void
qtUninstallColormap (ColormapPtr pmap)
{
	/* ### */
}


/* See Porting Layer Definition - p. 30 */
void
qtStoreColors (ColormapPtr pmap,
		int ndef,
		xColorItem *pdefs)
{
	/* ### */
}


/* See Porting Layer Definition - p. 30 */
void
qtResolveColor (unsigned short *pred,
		 unsigned short *pgreen,
		 unsigned short *pblue,
		 VisualPtr	pVisual)
{
#if QTDEBUG
	ErrorF ("qtResolveColor ()\n");
#endif

	miResolveColor (pred, pgreen, pblue, pVisual);
}


/* See Porting Layer Definition - p. 29 */
Bool
qtCreateColormap (ColormapPtr pmap)
{
	/* ### */
	return TRUE;
}


/* See Porting Layer Definition - p. 29, 30 */
void
qtDestroyColormap (ColormapPtr pColormap)
{
	/* ### */
}


int
qtExpandDirectColors (ColormapPtr pmap, int ndef,
		       xColorItem *indefs, xColorItem *outdefs)
{
	return miExpandDirectColors (pmap, ndef, indefs, outdefs);
}



/*
 * Install the standard fb colormap, or the GDI colormap,
 * depending on the current screen depth.
 */

Bool
qtCreateDefColormap (ScreenPtr pScreen)
{
	qtScreenPriv(pScreen);
	qtScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;

#if QTDEBUG
	ErrorF ("qtCreateDefColormap\n");
#endif

	/* Use standard fb colormaps for non palettized color modes */
	if (pScreenInfo->dwBPP > 8)
	{
		ErrorF ("qtCreateDefColormap - Deferring to " \
			"fbCreateDefColormap ()\n");
		return fbCreateDefColormap (pScreen);
	}

	/* ### NOT SUPPORTED */
	return TRUE;
}
