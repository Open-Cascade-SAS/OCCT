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
#define TRACE_ISDEFINE_MARKER
#endif

/*
   XW_STATUS Xw_isdefine_marker (amarkmap,index):
   XW_EXT_MARKMAP *amarkmap
   int index			marker index

	Returns XW_ERROR if BadMarker Index or marker is not defined
	Returns XW_SUCCESS if Successful      

*/
#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_marker (void *amarkmap,int index)
#else
XW_STATUS Xw_isdefine_marker (amarkmap,index)
void *amarkmap;
int index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_MARKMAP *pmarkmap = (XW_EXT_MARKMAP*) amarkmap;
XW_STATUS status = XW_ERROR ;

	if( index > 0 ) {
	    if ( pmarkmap && (index < pmarkmap->maxmarker) &&
				(pmarkmap->npoint[index] > 0) ) {
	        return (XW_SUCCESS) ;
	    } 
	} else status = XW_SUCCESS ;


#ifdef  TRACE_ISDEFINE_MARKER
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_marker(%lx,%d)\n",status,(long ) pmarkmap,index) ;
}
#endif

	return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_markerindex (XW_EXT_MARKMAP *amarkmap,int index)
#else
XW_STATUS Xw_isdefine_markerindex (amarkmap,index)
XW_EXT_MARKMAP *amarkmap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Verify Marker range index Only
*/
{
XW_EXT_MARKMAP *pmarkmap = amarkmap;
XW_STATUS status = XW_ERROR ;

	if( pmarkmap && (index > 0) && (index < pmarkmap->maxmarker) ) {
	    return (XW_SUCCESS) ;
	}

#ifdef  TRACE_ISDEFINE_MARKER
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_markerindex(%lx,%d)\n",status,(long ) pmarkmap,index) ;
}
#endif
	return (status) ;
}
