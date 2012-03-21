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
#define TRACE_GET_WIDTH
#endif

/*
   XW_STATUS Xw_get_width (awidthmap,index,width):
   XW_EXT_WIDTHMAP *awidthmap
   int index			Line width index
   float *width			width in MM

	Get Line Width Extended widthmap WIDTH value from index . 

	Returns XW_ERROR if BadWidth Index or Width is not defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_width (void *awidthmap,int index,float *width)
#else
XW_STATUS Xw_get_width (awidthmap,index,width)
void *awidthmap;
int index ;
float *width ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap;
Screen *screen ;

	if ( !Xw_isdefine_width(pwidthmap,index) ) {
	    /*ERROR*Bad Defined Width*/
	    Xw_set_error(52,"Xw_get_width",&index) ;
	    return (XW_ERROR) ;
	}

	screen = ScreenOfDisplay(_WDISPLAY,DefaultScreen(_WDISPLAY)) ;

	*width = (float)pwidthmap->widths[index] *
		(float)WidthMMOfScreen(screen)/(float)WidthOfScreen(screen) ;

#ifdef  TRACE_GET_WIDTH
if( Xw_get_trace() > 1 ) {
    printf (" Xw_get_width(%lx,%d,%f)\n",(long ) pwidthmap,index,*width) ;
}
#endif

	return (XW_SUCCESS);
}
