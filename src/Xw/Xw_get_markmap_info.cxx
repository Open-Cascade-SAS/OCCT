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
#define TRACE_GET_MARKMAP_INFO
#endif

/*
   STATUS Xw_get_markmap_info (amarkmap,mmark,umark,dmark,fmark):
   XW_EXT_MARKMAP *amarkmap
   int *mmark		Return the maximum marker number of the markmap
   int *umark		Return the User marker number used in the markmap
   int *dmark		Return the User marker number defined in the markmap
   int *fmark		Return the First free marker index in the markmap
   NOTE than if fmark is < 0 No more Free Marker exist in the markmap

	Returns ERROR if the Extended Markmap is not defined
	Returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_markmap_info (void* amarkmap,
		int* mmark,int* umark,int* dmark,int* fmark)
#else
XW_STATUS Xw_get_markmap_info (amarkmap,mmark,umark,dmark,fmark)
void *amarkmap;
int *mmark,*umark,*dmark,*fmark ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_MARKMAP *pmarkmap = (XW_EXT_MARKMAP*) amarkmap;
int i ;

	if( !Xw_isdefine_markmap(pmarkmap) ) {
	    /*ERROR*Bad EXT_MARKMAP Address */
	    Xw_set_error(46,"Xw_get_markmap_info",pmarkmap) ;
	    return (XW_ERROR) ;
	}

	*mmark = MAXMARKER ;
	*umark = pmarkmap->maxmarker ;
	*dmark = 1 ;
	*fmark = -1 ;
	for( i=0 ; i< *umark ; i++ ) {
	    if( pmarkmap->marks[i] ) (*dmark)++ ;
	    else if( i && *fmark < 0 ) *fmark = i ;
	}
	
#ifdef  TRACE_GET_MARKMAP_INFO
if( Xw_get_trace() ) {
    printf (" Xw_get_markmap_info(%lx,%d,%d,%d,%d)\n",
			(long ) pmarkmap,*mmark,*umark,*dmark,*fmark) ;
}
#endif

	return (XW_SUCCESS);
}
