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
#define TRACE_GET_TYPEMAP_INFO
#endif

/*
   XW_STATUS Xw_get_typemap_info (atypemap,mtype,utype,dtype,ftype):
   XW_EXT_TYPEMAP *atypemap
   int *mtype		Return the maximum type number of the typemap
   int *utype		Return the User type number used in the typemap
   int *dtype		Return the User type number defined in the typemap
   int *ftype		Return the First Free type index in the typemap
   NOTE than if ftype < 0 , No more Free type exist in the typemap

	Returns XW_ERROR if the Extended Typemap is not defined
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_typemap_info (void *atypemap,int *mtype,int *utype,int *dtype,int *ftype)
#else
XW_STATUS Xw_get_typemap_info (atypemap,mtype,utype,dtype,ftype)
void *atypemap;
int *mtype,*utype,*dtype,*ftype ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_TYPEMAP *ptypemap = (XW_EXT_TYPEMAP*)atypemap;
  int i ;

	if( !Xw_isdefine_typemap(ptypemap) ) {
	    /*ERROR*Bad EXT_TYPEMAP Address*/
	    Xw_set_error(51,"Xw_get_typemap_info",ptypemap) ;
	    return (XW_ERROR) ;
	}

	*mtype = MAXTYPE ;
	*utype = ptypemap->maxtype ;
	*dtype = 1 ;
	*ftype = -1 ;
	for( i=0 ; i< *utype ; i++ ) {
	    if( ptypemap->types[i] ) (*dtype)++ ;
	    else if( i && *ftype < 0 ) *ftype = i ;
	}
	
#ifdef  TRACE_GET_TYPEMAP_INFO
if( Xw_get_trace() ) {
    printf (" Xw_get_typemap_info(%lx,%d,%d,%d,%d)\n",
			(long ) ptypemap,*mtype,*utype,*dtype,*ftype) ;
}
#endif

	return (XW_SUCCESS);
}
