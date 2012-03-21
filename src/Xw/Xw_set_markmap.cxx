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
#define TRACE_SET_MARKMAP
#endif

/*
   XW_STATUS Xw_set_markmap(awindow,amarkmap):
   XW_EXT_WINDOW *awindow
   XW_EXT_MARKMAP *amarkmap

	Set a markmap to an existing window 

	Returns XW_ERROR if Bad Markmap
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_markmap (void *awindow,void *amarkmap)
#else
XW_STATUS Xw_set_markmap (awindow,amarkmap)
void *awindow;
void *amarkmap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_MARKMAP *pmarkmap = (XW_EXT_MARKMAP*) amarkmap ;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_set_markmap",pwindow) ;
        return (XW_ERROR) ;
    }

    if( !Xw_isdefine_markmap(pmarkmap) ) {
        /*ERROR*Bad EXT_MARKMAP Address*/
        Xw_set_error(46,"Xw_set_markmap",pmarkmap) ;
        return (XW_ERROR) ;
    }

    _MARKMAP = pmarkmap ;
    _MARKMAP->maxwindow++ ;

#ifdef TRACE_SET_MARKMAP
if( Xw_get_trace() ) {
    printf(" Xw_set_markmap(%lx,%lx)\n",(long ) pwindow,(long ) pmarkmap) ;
}
#endif

	return (XW_SUCCESS);
}
