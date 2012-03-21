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
#define TRACE_GET_DOUBLE_BUFFER
#endif

/*
   DOUBLEBUFFERMODE Xw_get_double_buffer (awindow)
   XW_EXT_WINDOW *awindow

	Returns the double_buffering status for this window .

	returns SUCCESS always
*/

#ifdef XW_PROTOTYPE
XW_DOUBLEBUFFERMODE Xw_get_double_buffer (void* awindow)
#else
XW_DOUBLEBUFFERMODE Xw_get_double_buffer (awindow)
XW_EXT_WINDOW *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_DOUBLEBUFFERMODE state ;

	if( (_PIXMAP && (_DRAWABLE == _PIXMAP)) ||
	    (_NWBUFFER > 0 && (_DRAWABLE == _BWBUFFER)) ) state = XW_ENABLE ;
	else state = XW_DISABLE ;

#ifdef TRACE_GET_DOUBLE_BUFFER
if( Xw_get_trace() ) {
    printf(" %d = Xw_get_double_buffer(%lx)\n",state,(long ) pwindow) ;
}
#endif
	
    return (state);
}
