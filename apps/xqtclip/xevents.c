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


/* Standard library headers */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


/* X headers */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>


/* Local headers */
#include "xqtclip.h"
#include "xevents.h"


extern Atom atomClipboard;

#define FALSE 0
#define	TRUE (!0)

/*
 * Process any pending X events
 */

Bool
FlushXEvents (Atom atomLocalProperty, Atom atomUTF8String,
	      Atom atomCompoundText, Atom atomTargets, Atom atomDeleteWindow,
	      int iWindow, Display *pDisplay)
{
	XTextProperty		xtpText;
	XEvent		event;
	XSelectionEvent	eventSelection;
	unsigned long		ulReturnBytesLeft;
	unsigned char		*pszReturnData = NULL;
	int			iReturn;
	Bool			fReturn = TRUE;
	XICCEncodingStyle	xiccesStyle;
	char			*pszUTF8 = NULL;
	char			*pszTextList[2];
	int			iCount;
	char			**ppszTextList = NULL;

	/* Process all pending events */
	while (XPending (pDisplay))
	{
		/* Get the next event - will not block because one is ready */
		XNextEvent (pDisplay, &event);

		/* Branch on the event type */
		switch (event.type)
		{
		case ClientMessage:
			if (event.xclient.data.l[0] == atomDeleteWindow)
			{
				printf ("\nReceived WM_DELETE_WINDOW\n\n");
				fReturn = FALSE;
			}
			else
				printf ("\nUnknown ClientMessage\n\n");
			break;

		case SelectionClear:
			/* Request the lost selection contents */
			iReturn = XConvertSelection (pDisplay,
						     event.xselectionclear.selection,
						     atomCompoundText,
						     atomLocalProperty,
						     iWindow,
						     CurrentTime);
			if (iReturn == BadAtom || iReturn == BadWindow)
			{
				printf ("SelectionClear - XConvertSelection () failed\n");
				exit (1);
			}
			break;

	  
			/*
			 * SelectionRequest
			 */

		case SelectionRequest:
#if 0
			printf ("SelectionRequest - target %d\n",
				event.xselectionrequest.target);
			printf ("SelectionRequest - Target atom name %s\n",
				XGetAtomName (pDisplay, event.xselectionrequest.target));
#endif

			/* Abort if invalid target type */
			if (event.xselectionrequest.target != XA_STRING
			    && event.xselectionrequest.target != atomUTF8String
			    && event.xselectionrequest.target != atomCompoundText
			    && event.xselectionrequest.target != atomTargets)
			{
				/* Setup selection notify event */
				eventSelection.type = SelectionNotify;
				eventSelection.send_event = True;
				eventSelection.display = pDisplay;
				eventSelection.requestor = event.xselectionrequest.requestor;
				eventSelection.selection = event.xselectionrequest.selection;
				eventSelection.target = event.xselectionrequest.target;
				eventSelection.property = None;
				eventSelection.time = event.xselectionrequest.time;

				/* Notify the requesting window that the operation is complete */
				iReturn = XSendEvent (pDisplay,
						      eventSelection.requestor,
						      False,
						      0L,
						      (XEvent *) &eventSelection);
				if (iReturn == BadValue || iReturn == BadWindow)
				{
					printf ("XSendEvent () failed\n");
					exit (1);
				}

				break;
			}

			/* Handle targets type of request */
			if (event.xselectionrequest.target == atomTargets)
			{
				Atom atomTargetArr[4] = {atomTargets,
							 atomCompoundText,
							 atomUTF8String,
							 XA_STRING};

				/* Try to change the property */
				iReturn = XChangeProperty (pDisplay,
							   event.xselectionrequest.requestor,
							   event.xselectionrequest.property,
							   event.xselectionrequest.target,
							   8,
							   PropModeReplace,
							   (char *) atomTargetArr,
							   sizeof (atomTargetArr));
				if (iReturn == BadAlloc
				    || iReturn == BadAtom
				    || iReturn == BadMatch
				    || iReturn == BadValue
				    || iReturn == BadWindow)
				{
					printf ("SelectionRequest - XChangeProperty failed: %d\n",
						iReturn);
				}

				/* Setup selection notify xevent */
				eventSelection.type = SelectionNotify;
				eventSelection.send_event = True;
				eventSelection.display	 = pDisplay;
				eventSelection.requestor = event.xselectionrequest.requestor;
				eventSelection.selection = event.xselectionrequest.selection;
				eventSelection.target	 = event.xselectionrequest.target;
				eventSelection.property  = event.xselectionrequest.property;
				eventSelection.time	 = event.xselectionrequest.time;

				/*
				 * Notify the requesting window that
				 * the operation has completed
				 */
				iReturn = XSendEvent (pDisplay,
						      eventSelection.requestor,
						      False,
						      0L,
						      (XEvent *) &eventSelection);
				if (iReturn == BadValue || iReturn == BadWindow)
				{
					printf ("XSendEvent () failed\n");
				}
				break;
			}

			/* Setup the string style */
			if (event.xselectionrequest.target == XA_STRING)
				xiccesStyle = XStringStyle;
			else if (event.xselectionrequest.target == atomUTF8String)
				xiccesStyle = XUTF8StringStyle;
			else if (event.xselectionrequest.target == atomCompoundText)
				xiccesStyle = XCompoundTextStyle;
			else
				xiccesStyle = XStringStyle;

			/*
			 * FIXME: Can't pass CF_UNICODETEXT on Windows 95/98/Me
			 */
	  
			/* Get a pointer to the clipboard text */
			pszUTF8 = qtGetClipboardDataUtf8();// ###

			/* Setup our text list */
			pszTextList[0] = pszUTF8;
			pszTextList[1] = NULL;
	      
			/* Initialize the text property */
			xtpText.value = NULL;
	      
			/* Create the text property from the text list */
			iReturn = Xutf8TextListToTextProperty (pDisplay,
							       pszTextList,
							       1,
							       xiccesStyle,
							       &xtpText);
			if (iReturn == XNoMemory || iReturn == XLocaleNotSupported)
			{
				printf ("SelectionRequest - Xutf8TextListToTextProperty "
					"failed: %d\n",
					iReturn);
				exit(1);
			}
	      
			/* Free the UTF8 string */
			free (pszUTF8);

			/*
			 * FIXME: Pass pszGlobalData and strlen (pszGlobalData(
			 * on 1 byte, pass xtpText.value and xtpText.nitems
			 * on 2 byte.
			 */

			/* Copy the clipboard text to the requesting window */
			iReturn = XChangeProperty (pDisplay,
						   event.xselectionrequest.requestor,
						   event.xselectionrequest.property,
						   event.xselectionrequest.target,
						   8,
						   PropModeReplace,
						   xtpText.value,
						   xtpText.nitems);
			if (iReturn == BadAlloc || iReturn == BadAtom
			    || iReturn == BadMatch || iReturn == BadValue
			    || iReturn == BadWindow)
			{
				printf ("SelectionRequest - XChangeProperty failed: %d\n",
					iReturn);
				exit (1);
			}

			/* FIXME: Don't clean up on 1 byte. */
			XFree (xtpText.value);
			xtpText.value = NULL;

			/* Setup selection notify event */
			eventSelection.type = SelectionNotify;
			eventSelection.send_event = True;
			eventSelection.display = pDisplay;
			eventSelection.requestor = event.xselectionrequest.requestor;
			eventSelection.selection = event.xselectionrequest.selection;
			eventSelection.target = event.xselectionrequest.target;
			eventSelection.property = event.xselectionrequest.property;
			eventSelection.time = event.xselectionrequest.time;

			/* Notify the requesting window that the operation has completed */
			iReturn = XSendEvent (pDisplay,
					      eventSelection.requestor,
					      False,
					      0L,
					      (XEvent *) &eventSelection);
			if (iReturn == BadValue || iReturn == BadWindow)
			{
				printf ("XSendEvent () failed\n");
				exit (1);
			}
			break;
	  

			/*
			 * SelectionNotify
			 */ 

		case SelectionNotify:
#if 0
			printf ("SelectionNotify\n");
#endif

#if 0
			/*
			 * TEMP: Bail if selection is anything other than CLIPBOARD
			 */

			if (event.xselection.selection != atomClipboard)
				break;
#endif

			/*
			 *
			 * What are we doing here?
			 *
			 */
			if (event.xselection.property == None)
			{
				if(event.xselection.target == XA_STRING)
				{
#if 0
					printf ("SelectionNotify XA_STRING\n");
#endif
					return fReturn;
				}
				else if (event.xselection.target == atomUTF8String)
				{
					printf ("SelectionNotify UTF8\n");
					iReturn = XConvertSelection (pDisplay,
								     event.xselection.selection,
								     XA_STRING,
								     atomLocalProperty,
								     iWindow,
								     CurrentTime);
					if (iReturn == BadAtom || iReturn == BadWindow)
					{
						printf ("SelectionNotify - XConvertSelection () "
							"failed\n");
						exit (1);
					}
					return fReturn;
				}
				else if (event.xselection.target == atomCompoundText)
				{
					printf ("SelectionNotify CompoundText\n");
					iReturn = XConvertSelection (pDisplay,
								     event.xselection.selection,
								     atomUTF8String,
								     atomLocalProperty,
								     iWindow,
								     CurrentTime);
					if (iReturn == BadAtom || iReturn == BadWindow)
					{
						printf ("SelectionNotify - XConvertSelection () "
							"failed\n");
						exit (1);
					}
					return fReturn;
				}
				else
				{
					printf("Unknown format\n");
					return fReturn;
				}
			}

			/* Retrieve the size of the stored data */
			iReturn = XGetWindowProperty (pDisplay,
						      iWindow,
						      atomLocalProperty,
						      0,
						      0, /* Don't get data, just size */
						      False,
						      AnyPropertyType,
						      &xtpText.encoding,
						      &xtpText.format,
						      &xtpText.nitems,
						      &ulReturnBytesLeft,
						      &xtpText.value);
			if (iReturn != Success)
			{
				printf ("SelectionNotify - XGetWindowProperty () failed\n");
				exit (1);
			}

#if 0
			printf ("SelectionNotify - returned data %d left %d\n",
				xtpText.nitems, ulReturnBytesLeft);
#endif

			/* Request the selection data */
			iReturn = XGetWindowProperty (pDisplay,
						      iWindow,
						      atomLocalProperty,
						      0,
						      ulReturnBytesLeft,
						      False,
						      AnyPropertyType,
						      &xtpText.encoding,
						      &xtpText.format,
						      &xtpText.nitems,
						      &ulReturnBytesLeft,
						      &xtpText.value);
			if (iReturn != Success)
			{
				printf ("SelectionNotify - XGetWindowProperty () failed\n");
				exit (1);
			}

#if 0
			printf ("SelectionNotify - returned data %d left %d\n",
				prop.nitems, ulReturnBytesLeft);
			printf ("Notify atom name %s\n",
				XGetAtomName(pDisplay, prop.encoding));
#endif
	      
			/* Convert the text property to a text list */
			Xutf8TextPropertyToTextList (pDisplay,
						     &xtpText,
						     &ppszTextList,
						     &iCount);
			if (iCount > 0)
			{
				pszReturnData = malloc (strlen (ppszTextList[0]) + 1);
				strcpy (pszReturnData, ppszTextList[0]);
			}
			else
			{
				pszReturnData = malloc (1);
				pszReturnData[0] = 0;
			}
	      
			/* Free the data returned from XGetWindowProperty */
			XFreeStringList (ppszTextList);
			XFree (xtpText.value);
	      
			/* Push the selection data to the Windows clipboard */
			qtSetClipboardDataUtf8(pszReturnData);
			free(pszReturnData);

			/* Reassert ownership of the selection */	  
			iReturn = XSetSelectionOwner (pDisplay,
						      event.xselection.selection,
						      iWindow, CurrentTime);
			if (iReturn == BadAtom || iReturn == BadWindow)
			{
				printf ("SelectionNotify - Could not reassert ownership "
					"of selection ATOM: %s\n",
					XGetAtomName (pDisplay,
						      event.xselection.selection));
				exit (1);
			}
			else
			{
				/*printf ("SelectionNotify - Reasserted ownership of ATOM: %s\n",
					XGetAtomName (pDisplay,
					event.xselection.selection));*/
			}
#if 0
			/* Reassert ownership of the CLIPBOARD */	  
			iReturn = XSetSelectionOwner (pDisplay,
						      atomClipboard,
						      iWindow, CurrentTime);
			if (iReturn == BadAtom || iReturn == BadWindow)
			{
				printf ("Could not reassert ownership of selection\n");
				exit (1);
			}
#endif
			break;

#if 0
		case CreateNotify:
			printf ("FlushXEvents - CreateNotify parent: %ld\twindow: %ld\n",
				event.xcreatewindow.parent, event.xcreatewindow.window);
			break;

		case DestroyNotify:
			printf ("FlushXEvents - DestroyNotify window: %ld\tevent: %ld\n",
				event.xdestroywindow.window, event.xdestroywindow.event);
			break;
#endif

		default:
			break;
		}
	}

	return fReturn;
}
