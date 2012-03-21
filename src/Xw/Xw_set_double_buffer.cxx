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
#define TRACE_SET_DOUBLE_BUFFER
#endif

/*
   XW_STATUS Xw_set_double_buffer (awindow, state)
   XW_EXT_WINDOW *awindow
   DOUBLEBUFFERMODE state ;

	Sets the double_buffering status of the window .

	returns ERROR if Double Buffering is not XW_ENABLE on this station
	returns XW_SUCCESS if successful
*/

static char SetDoubleBuffer = '\0' ;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_double_buffer (void *awindow, XW_DOUBLEBUFFERMODE state)
#else
XW_STATUS Xw_set_double_buffer (awindow, state)
void *awindow;
XW_DOUBLEBUFFERMODE state ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_STATUS status = XW_SUCCESS;
char svalue[4] ;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_set_double_buffer",pwindow) ;
        return (XW_ERROR) ;
    }

    if( SetDoubleBuffer == '\0' ) {
	SetDoubleBuffer = 'U' ;
        if( Xw_get_env("Xw_SET_DOUBLE_BUFFER",svalue,sizeof(svalue)) ) {
	    if( strlen(svalue) ) {
		SetDoubleBuffer = svalue[0] ;
	    }
	    printf(" Xw_SET_DOUBLE_BUFFER is '%c'\n",SetDoubleBuffer) ;
        }
    }

    if( (SetDoubleBuffer == 'Y') ) { 
	state = XW_ENABLE ;
    } else if( SetDoubleBuffer == 'N' ) {
	state = XW_DISABLE ;
    }

    if( state == XW_DISABLE ) {
      if( _NWBUFFER > 0 ) {
	if( _DRAWABLE == _FWBUFFER ) return (XW_SUCCESS);
	_DRAWABLE = _FWBUFFER ;
      } else {
	if( _DRAWABLE == _WINDOW ) return (XW_SUCCESS);
	_DRAWABLE = _WINDOW ;
      }	
    } else if( state == XW_ENABLE ) {
	if( !_PIXMAP && !_NWBUFFER ) {
	    status = Xw_open_pixmap(awindow) ; 
	} 
	if( _NWBUFFER > 0 ) {
	  _DRAWABLE = _BWBUFFER ;
	} else if( _PIXMAP ) {
	  _DRAWABLE = _PIXMAP ;
	} else {
	  _DRAWABLE = _WINDOW ;
	}
    }

#ifdef TRACE_SET_DOUBLE_BUFFER
if( Xw_get_trace() ) {
    printf(" Xw_set_double_buffer(%lx,%d)\n",(long ) pwindow,state) ;
}
#endif
	
    return (status);
}
