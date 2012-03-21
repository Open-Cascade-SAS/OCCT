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
#define TRACE_SET_TYPEMAP
#endif

/*
   XW_STATUS Xw_set_typemap(awindow,atypemap):
   XW_EXT_WINDOW *awindow
   XW_EXT_TYPEMAP *atypemap

	Set a typemap to an existing window 

	Returns XW_ERROR if Bad Typemap
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_typemap (void *awindow,void *atypemap)
#else
XW_STATUS Xw_set_typemap (awindow,atypemap)
void *awindow;
void *atypemap ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
  XW_EXT_TYPEMAP *ptypemap = (XW_EXT_TYPEMAP*)atypemap ;
    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_set_typemap",pwindow) ;
        return (XW_ERROR) ;
    }

    if( !Xw_isdefine_typemap(ptypemap) ) {
        /*ERROR*Bad EXT_TYPEMAP Address*/
        Xw_set_error(20,"Xw_set_typemap",ptypemap) ;
        return (XW_ERROR) ;
    }

    _TYPEMAP = ptypemap ;
    _TYPEMAP->maxwindow++ ;

#ifdef TRACE_SET_TYPEMAP
if( Xw_get_trace() ) {
    printf(" Xw_set_typemap(%lx,%lx)\n",(long ) pwindow,(long ) ptypemap) ;
}
#endif

	return (XW_SUCCESS);
}
