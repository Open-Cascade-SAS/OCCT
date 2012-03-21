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
#define TRACE_ERASE_WINDOW
#endif

/*
   STATUS Xw_erase_window (awindow)
   XW_EXT_WINDOW *awindow

	Erase the entire drawable.

	returns SUCCESS always

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_erase_window (void* awindow)
#else
XW_STATUS Xw_erase_window (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
int i ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_erase_window",pwindow) ;
            return (XW_ERROR) ;
        }

	if( _BPIXMAP ) {
            XCopyArea(_DISPLAY,_BPIXMAP,_DRAWABLE,pwindow->qgwind.gccopy,
                                                0,0,_WIDTH,_HEIGHT,0,0) ;
	} else {
	    if( _DRAWABLE == _WINDOW ) { 
		XClearWindow(_DISPLAY,_WINDOW) ;
	    } else {
	        XFillRectangle(_DISPLAY,_DRAWABLE,pwindow->qgwind.gcclear,
						0,0,_WIDTH,_HEIGHT) ;
	    }
	}

	for( i=0 ; i<MAXBUFFERS ; i++ ) {
	  _BUFFER(i).isdrawn = False ;
	}

	XFlush(_DISPLAY) ;

#ifdef  TRACE_ERASE_WINDOW
if( Xw_get_trace() > 1 ) {
    printf(" Xw_erase_window(%lx)\n",(long ) pwindow) ;
}
#endif

	return (XW_SUCCESS);
}
