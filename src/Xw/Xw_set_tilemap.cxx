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
#define TRACE_SET_TILEMAP
#endif

/*
   XW_STATUS Xw_set_tilemap(awindow,atilemap):
   XW_EXT_WINDOW *awindow
   XW_EXT_TILEMAP *atilemap

	Set a tilemap to an existing window 

	Returns XW_ERROR if Bad Tilemap
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_tilemap (void *awindow,void *atilemap)
#else
XW_STATUS Xw_set_tilemap (awindow,atilemap)
void *awindow;
void *atilemap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*)atilemap ;
    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_set_colormap",pwindow) ;
        return (XW_ERROR) ;
    }

    if( !Xw_isdefine_tilemap(ptilemap) ) {
        /*ERROR*Bad EXT_TILEMAP Address*/
        Xw_set_error(49,"Xw_set_tilemap",ptilemap) ;
        return (XW_ERROR) ;
    }

    _TILEMAP = ptilemap ;
    _TILEMAP->maxwindow++ ;

#ifdef TRACE_SET_TILEMAP
if( Xw_get_trace() ) {
    printf(" Xw_set_tilemap(%lx,%lx)\n",(long ) pwindow,(long ) ptilemap) ;
}
#endif

	return (XW_SUCCESS);
}
