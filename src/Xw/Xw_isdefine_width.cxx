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
#define TRACE_DEFINE_WIDTH
#endif

/*
   XW_STATUS Xw_isdefine_width (awidthmap,index):
   XW_EXT_WIDTHMAP *awidthmap
   int index			Line width index

	Returns XW_ERROR if BadWidth Index or Width is not defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_width (void *awidthmap,int index)
#else
XW_STATUS Xw_isdefine_width (awidthmap,index)
void *awidthmap;
int index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap;
XW_STATUS status = XW_ERROR ;

    	if( index ) {
	    if ( pwidthmap && (index < pwidthmap->maxwidth) &&
				(pwidthmap->widths[index] > 0) ) {
	        status = XW_SUCCESS ;
	    }
	} else status = XW_SUCCESS ;

#ifdef  TRACE_DEFINE_WIDTH
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_width(%lx,%d)\n",status,(long ) pwidthmap,index) ;
}
#endif

	return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_widthindex  (XW_EXT_WIDTHMAP *awidthmap,int index)
#else
XW_STATUS Xw_isdefine_widthindex  (awidthmap,index)
XW_EXT_WIDTHMAP *awidthmap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Verify Width index range Only
*/
{
XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap;
XW_STATUS status = XW_ERROR ;

	if ( pwidthmap && (index > 0) && (index < pwidthmap->maxwidth) ) {
	    status = XW_SUCCESS ;
	}

#ifdef  TRACE_DEFINE_WIDTH
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_widthindex(%lx,%d)\n",status,(long ) pwidthmap,index) ;
}
#endif

	return (status);
}
