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
#define TRACE_FLUSH
#endif

/*
   STATUS Xw_flush (awindow,synchro)
   XW_EXT_WINDOW *awindow
				
	Flush all primitives in the previous window . 

	Returns SUCCESS always

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_flush (void* awindow,int synchro)
#else
XW_STATUS Xw_flush (awindow,synchro)
void *awindow;
int synchro;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;

	if( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXT_WINDOW Address*/
	    Xw_set_error(24,"Xw_flush",pwindow) ;
	    return (XW_ERROR) ;
	}

	if( synchro ) {
	    XSync(_DISPLAY,False) ;
	} else {
            XFlush(_DISPLAY) ;
	}

        if( (_DRAWABLE == _PIXMAP) || (_DRAWABLE == _BWBUFFER) ) 
                                                Xw_restore_window (pwindow) ;
 

#ifdef  TRACE_FLUSH
if( Xw_get_trace() ) {
    printf (" Xw_flush(%lx)\n",(long ) pwindow) ;
}
#endif

        return (XW_SUCCESS);
}
