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
#define TRACE_DEF_WIDTH
#endif

/*
   STATUS Xw_def_width (awidthmap,index,width):
   XW_EXT_WIDTHMAP *awidthmap
   int index			Line width index
   float width			width in MM

	Update Line Width Extended widthmap index 
				with the specified WIDTH value . 
	NOTE than width index 0 is the default width (1 pixel) 
					and cann't be redefined .

	Returns ERROR if BadWidth Index
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_def_width (void* awidthmap,int index,float width)
#else
XW_STATUS Xw_def_width (awidthmap,index,width)
void *awidthmap ;
int index ;
float width ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap ;
Screen *screen ;
int pwidth ;

	if ( !Xw_isdefine_widthindex(pwidthmap,index) ) {
	    /*Bad Width Index*/
	    Xw_set_error(21,"Xw_def_width",&index) ;
	    return (XW_ERROR) ;
	}

	screen = ScreenOfDisplay(_WDISPLAY,DefaultScreen(_WDISPLAY)) ;
	width = width * (float)HeightOfScreen(screen)/
					(float)HeightMMOfScreen(screen) ; 
	pwidth = (int) (0.5 + width) ;

	pwidthmap->widths[index] = 1 ;
	if( width == 0. || pwidth < 0 || pwidth > 255 ) {
	    /*Bad Width Thickness*/
	    Xw_set_error(22,"Xw_def_width",&width) ;
	}

	pwidthmap->widths[index] = (unsigned char) (pwidth) ? pwidth : 1 ;

#ifdef  TRACE_DEF_WIDTH
if( Xw_get_trace() ) {
    printf (" Xw_def_width(%lx,%d,%f)\n",(long ) pwidthmap,index,width) ;
}
#endif

	return (XW_SUCCESS);
}
