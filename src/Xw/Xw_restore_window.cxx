// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <Xw_Extension.h>

/* ifdef then trace on */
#ifdef TRACE
#define TRACE_RESTORE_WINDOW
#endif

/*
   XW_STATUS Xw_restore_window (awindow):
   XW_EXT_WINDOW *awindow

	Redraw the window from the attched pixmap   

	returns XW_ERROR if NO attached pixmap exist
	returns XW_SUCCESS if successful
*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_restore_window (void *awindow)
#else
XW_STATUS Xw_restore_window (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
//XGCValues gc_values ;
//int function,color,mask = 0 ;
//unsigned long planemask,hcolor ;

	XFlush(_DISPLAY) ;
	if( _NWBUFFER > 0 ) {
            XCopyArea(_DISPLAY,_BWBUFFER,_FWBUFFER,pwindow->qgwind.gccopy,
                    				0,0,_WIDTH,_HEIGHT,0,0) ;
        } else if( _PIXMAP ) {
            XCopyArea(_DISPLAY,_PIXMAP,_WINDOW,pwindow->qgwind.gccopy,
                    				0,0,_WIDTH,_HEIGHT,0,0) ;
	} else return (XW_ERROR) ;

	XFlush(_DISPLAY) ;

#ifdef  TRACE_RESTORE_WINDOW
if( Xw_get_trace() ) {
	printf (" Xw_restore_window(%lx)\n",(long ) pwindow) ;
}
#endif

	return (XW_SUCCESS);
}
