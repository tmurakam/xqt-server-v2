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
 * Qt version:  Takuya Murakami
 */


/* Standard library headers */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

/* X headers */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <locale.h>


/* Application constants */
#define WINDOW_CLASS		"xwinclip"
#define WINDOW_TITLE		"xwinclip"
#define WIN_USE_SELECT		1
#define WIN_CONNECT_RETRIES	3
#define WIN_CONNECT_DELAY	4
#define WIN_JMP_OKAY		0
#define WIN_JMP_ERROR_IO	2


/* Local headers */
#include "xevents.h"



Atom		atomClipboard, atomClipboardManager;
Atom		atomLocalProperty, atomCompoundText;
Atom		atomUTF8String, atomTargets;
Atom		atomDeleteWindow;
Display		*pDisplay;
Window		iWindow;


/*
 * Main function
 */

int
initXside(int argc, char *argv[])
{
#if 0
	Atom			atomClipboard, atomClipboardManager;
#endif
	int			iReturn;
	int			iConnectionNumber;
	int			iRetries;
	char			*pszDisplay = NULL;
	int			i;

	/* Parse command-line parameters */
	for (i = 1; i < argc; ++i)
	{
		/* Look for -display "display_name" or --display "display_name" */
		if (i < argc - 1
		    && (!strcmp (argv[i], "-display")
			|| !strcmp (argv[i], "--display")))
		{
			/* Grab a pointer to the display parameter */
			pszDisplay = argv[i + 1];

			/* Skip the display argument */
			i++;
			continue;
		}
      
		/* Yack when we find a parameter that we don't know about */
		printf ("Unknown parameter: %s\nExiting.\n", argv[i]);
		exit (1);
	}

	/* Set jump point for IO Error exits */
#if 0
	iReturn = setjmp (g_jmpEntry);
  
	/* Check if we should continue operations */
	if (iReturn != WIN_JMP_ERROR_IO
	    && iReturn != WIN_JMP_OKAY)
	{
		/* setjmp returned an unknown value, exit */
		printf ("setjmp returned: %d exiting\n",
			iReturn);
		exit (1);
	}
	else if (iReturn == WIN_JMP_ERROR_IO)
	{
		printf ("setjmp returned and hwnd: %08x\n", hwnd);
	}
#endif

	/* Initialize retry count */
	iRetries = 0;

#if 0
	/* Specify our signal handlers */
	signal (SIGQUIT, handle_kill);
#endif

	/* Set the current locale?  What does this do? */
	if (!setlocale (LC_ALL, ""))
	{
		printf ("setlocale() error\n");
		exit (1);
	}
      
	/* See if X supports the current locale */
	if (XSupportsLocale () == False)
	{
		printf ("Locale not supported by X\n");
		exit (1);
	}
  
	/* Open the X display */
	do
	{
		pDisplay = XOpenDisplay (pszDisplay);
		if (pDisplay == NULL)
		{
			printf ("Could not open display, try: %d, sleeping: %d\n",
				iRetries + 1, WIN_CONNECT_DELAY);
			++iRetries;
			sleep (WIN_CONNECT_DELAY);
			continue;
		}
		else
			break;
	}
	while (pDisplay == NULL && iRetries < WIN_CONNECT_RETRIES);

	/* Make sure that the display opened */
	if (pDisplay == NULL)
	{
		printf ("Failed opening the display, giving up\n");
		return 1;
	}

	/* Get our connection number */
	iConnectionNumber = ConnectionNumber (pDisplay);

	/* Select event types to watch */
	if (XSelectInput (pDisplay,
			  DefaultRootWindow (pDisplay),
			  SubstructureNotifyMask |
			  StructureNotifyMask |
			  PropertyChangeMask) == BadWindow)
		printf ("XSelectInput generated BadWindow on RootWindow\n\n");

	/* Create a messaging window */
	iWindow = XCreateSimpleWindow (pDisplay,
				       DefaultRootWindow (pDisplay),
				       1, 1,
				       500, 500,
				       0,
				       BlackPixel (pDisplay, 0),
				       BlackPixel (pDisplay, 0));
	if (iWindow == 0)
	{
		printf ("Could not create a window\n");
		exit (1);
	}

#if 0
	/* Print out our window number */
	printf ("Window number: %d\n", iWindow);
#endif

#if 0
	/* Display the messaging window, as a test */
	XMapWindow (pDisplay, iWindow);
#endif

	/* ChangeWindowAttributes can change own window's event mask */
#if 0
	attrib.event_mask = StructureNotifyMask;
	iReturn = XChangeWindowAttributes (pDisplay, iWindow, CWEventMask, &attrib);
	if (iReturn == BadWindow)
		printf ("XChangeWindowAttributes gave BadWindow\n");
	else if (iReturn == BadAccess)
		printf ("XChangeWindowAttributes gave BadAccess\n");
	else if (iReturn == BadValue)
		printf ("XChangeWindowAttributes gave BadValue\n");
#endif			   

	/* This looks like our only hope for getting a message before shutdown */
	/* Register for WM_DELETE_WINDOW message from window manager */
	atomDeleteWindow = XInternAtom (pDisplay, "WM_DELETE_WINDOW", False);
	XSetWMProtocols (pDisplay, iWindow, &atomDeleteWindow, 1);

	/* Set error handler */
#if 0
	XSetErrorHandler (error_handler);
#endif
	//XSetIOErrorHandler (error_handler_io);

	/* Create an atom for CLIPBOARD_MANAGER */
	atomClipboardManager = XInternAtom (pDisplay, "CLIPBOARD_MANAGER", False);
	if (atomClipboardManager == None)
	{
		printf ("Could not create CLIPBOARD_MANAGER atom\n");
		exit (1);
	}

	/* Assert ownership of CLIPBOARD_MANAGER */
	iReturn = XSetSelectionOwner (pDisplay, atomClipboardManager,
				      iWindow, CurrentTime);
	if (iReturn == BadAtom || iReturn == BadWindow)
	{
		printf ("Could not set CLIPBOARD_MANAGER owner\n");
		exit (1);
	}

	/* Create an atom for CLIPBOARD */
	atomClipboard = XInternAtom (pDisplay, "CLIPBOARD", False);
	if (atomClipboard == None)
	{
		printf ("Could not create CLIPBOARD atom\n");
		exit (1);
	}

	/* Assert ownership of CLIPBOARD */
	iReturn = XSetSelectionOwner (pDisplay, atomClipboard,
				      iWindow, CurrentTime);
	if (iReturn == BadAtom || iReturn == BadWindow)
	{
		printf ("Could not set CLIPBOARD owner\n");
		exit (1);
	}

	/* Assert ownership of PRIMARY */
	iReturn = XSetSelectionOwner (pDisplay, XA_PRIMARY,
				      iWindow, CurrentTime);
	if (iReturn == BadAtom || iReturn == BadWindow)
	{
		printf ("Could not set PRIMARY owner\n");
		exit (1);
	}

	/* Local property to hold pasted data */
	atomLocalProperty = XInternAtom (pDisplay, "CYGX_CUT_BUFFER", False);
	if (atomLocalProperty == None)
	{
		printf ("Could not create CYGX_CUT_BUFFER atom\n");
		exit (1);
	}

	/* Create an atom for UTF8_STRING */
	atomUTF8String = XInternAtom (pDisplay, "UTF8_STRING", False);
	if (atomUTF8String == None)
	{
		printf ("Could not create UTF8_STRING atom\n");
		exit (1);
	}

	/* Create an atom for COMPOUND_TEXT */
	atomCompoundText = XInternAtom (pDisplay, "COMPOUND_TEXT", False);
	if (atomCompoundText == None)
	{
		printf ("Could not create COMPOUND_TEXT atom\n");
		exit (1);
	}

	/* Create an atom for TARGETS */
	atomTargets = XInternAtom (pDisplay, "TARGETS", False);
	if (atomTargets == None)
	{
		printf ("Could not create TARGETS atom\n");
		exit (1);
	}

	/* Pre-flush X events */
	/* 
	 * NOTE: Apparently you'll freeze if you don't do this,
	 *	   because there may be events in local data structures
	 *	   already.
	 */
	FlushXEvents (atomLocalProperty, atomUTF8String,
		      atomCompoundText, atomTargets, atomDeleteWindow,
		      iWindow, pDisplay);

	return iConnectionNumber;
}


void dispatchXEvents(void)
{
	/* Process X events */
	int fReturn = FlushXEvents (atomLocalProperty,
				    atomUTF8String,
				    atomCompoundText,
				    atomTargets,
				    atomDeleteWindow,
				    iWindow, pDisplay);
				
	if (!fReturn)
	{
		printf ("Caught WM_DELETE_WINDOW - shutting down\n");
		/* ### */
	}
}
