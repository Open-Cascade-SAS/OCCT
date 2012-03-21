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
#define TRACE_GET_BACKGROUND_PIXEL
#endif

/*
   STATUS Xw_get_background_pixel(awindow,pixel):
   XW_EXT_WINDOW *awindow	Window extension structure
   unsigned long *pixel ;	Return Color pixel depending of Visual Class 

	Returns ERROR if Window or background pixel is not defined properly
	Returns SUCCESS if Successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_background_pixel(void* awindow, unsigned long* pixel)
#else
XW_STATUS Xw_get_background_pixel(awindow,pixel)
void *awindow ;
unsigned long *pixel ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_STATUS status = XW_SUCCESS ;

    *pixel = 0;

    if( !pwindow ) {
	/*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error( 24,"Xw_get_background_pixel",pwindow ) ;
        return( XW_ERROR ) ;
    }
    if( pwindow->backindex < 0 ) {
      *pixel = _COLORMAP->backpixel;
    } else if( Xw_isdefine_color(_COLORMAP,pwindow->backindex) ) {
      *pixel = _COLORMAP->pixels[pwindow->backindex];
    } else {
      status = XW_ERROR;
    }

#ifdef TRACE_GET_BACKGROUND_PIXEL
if( Xw_get_trace() > 3 ) {
    printf(" %d = Xw_get_background_pixel(%lx,%lx)\n",
                        status,(long ) pwindow,*pixel) ;
}
#endif

    return( status ) ;
}
