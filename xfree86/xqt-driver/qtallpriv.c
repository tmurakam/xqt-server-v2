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
 * Authors:	Keith Packard, MIT X Consortium
 *		Harold L Hunt II
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/winallpriv.c,v 1.12 2002/10/31 23:04:39 alanh Exp $ */

#include "xqt.h"
#include "qtwindow.h"

/* See Porting Layer Definition - p. 58 */
/*
 * Allocate indexes for the privates that we use.
 * Allocate memory directly for the screen privates.
 * Reserve space in GCs and Pixmaps for our privates.
 * Colormap privates are handled in winAllocateCmapPrivates ()
 */

Bool
qtAllocatePrivates (ScreenPtr pScreen)
{
	qtPrivScreenPtr		pScreenPriv;

#if QTDEBUG
	ErrorF ("qtAllocateScreenPrivates - g_ulServerGeneration: %d "
		"serverGeneration: %d\n",
		g_ulServerGeneration, serverGeneration);
#endif

	/* We need a new slot for our privates if the screen gen has changed */
	if (g_ulServerGeneration != serverGeneration)
	{
		/* Get an index that we can store our privates at */
		g_iScreenPrivateIndex = AllocateScreenPrivateIndex ();
		g_iGCPrivateIndex = AllocateGCPrivateIndex ();
		//###g_iPixmapPrivateIndex = AllocatePixmapPrivateIndex ();
		g_iWindowPrivateIndex = AllocateWindowPrivateIndex ();

		g_ulServerGeneration = serverGeneration;
	}

	/* Allocate memory for the screen private structure */
	pScreenPriv = (qtPrivScreenPtr) malloc (sizeof (qtPrivScreenRec));
	if (!pScreenPriv)
	{
		ErrorF ("qtAllocateScreenPrivates - malloc () failed\n");
		return FALSE;
	}

	/* Initialize the memory of the private structure */
	memset(pScreenPriv, 0, sizeof (qtPrivScreenRec));

	/* Intialize private structure members */
	pScreenPriv->fActive = TRUE;

	/* Save the screen private pointer */
	qtSetScreenPriv (pScreen, pScreenPriv);

	/* Reserve GC memory for our privates */
	if (!AllocateGCPrivate (pScreen, g_iGCPrivateIndex,
				sizeof (qtPrivGCRec)))
	{
		ErrorF ("qtAllocatePrivates - AllocateGCPrivate () failed\n");
		return FALSE;
	}

	/* Reserve Pixmap memory for our privates */
#if 0	// ### PIXPRIV が定義されてないとエラーになるので、とりあえず外す
	if (!AllocatePixmapPrivate (pScreen, g_iPixmapPrivateIndex,
				    sizeof (qtPrivPixmapRec)))
	{
		ErrorF ("qtAllocatePrivates - AllocatePixmapPrivates () failed\n");
		return FALSE;
	}
#endif

	/* Reserve Window memory for our privates */
	if (!AllocateWindowPrivate (pScreen, g_iWindowPrivateIndex,
				    sizeof (qtPrivWinRec)))
	{
		ErrorF ("qtAllocatePrivates () - AllocateWindowPrivates () failed\n");
		return FALSE;
	}

	return TRUE;
}


/*
 * Colormap privates may be allocated after the default colormap has
 * already been created for some screens.  This initialization procedure
 * is called for each default colormap that is found.
 */

Bool
qtInitCmapPrivates (ColormapPtr pcmap)
{
	return TRUE;
}


/*
 * Allocate memory for our colormap privates
 */

Bool
qtAllocateCmapPrivates (ColormapPtr pCmap)
{
	/* ### not yet */
	return TRUE;
}
