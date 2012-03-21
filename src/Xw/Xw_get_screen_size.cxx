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
#define TRACE_GET_SCREEN_SIZE
#define TRACE_GET_MMSCREEN_SIZE
#endif

/*
   XW_STATUS Xw_get_screen_size (adisplay,width,height):
   XW_EXT_DISPLAY *adisplay
   int *width
   int *depth

	Get Screen Width and Height in Pixels

	Returns XW_ERROR if Ext_Display address is wrong
	Returns XW_SUCCESS always

   XW_STATUS Xw_get_mmscreen_size (adisplay,width,height):
   XW_EXT_DISPLAY *adisplay
   int *width
   int *depth

	Get Screen Width and Height in Millimeters

	Returns XW_ERROR if Ext_Display address is wrong
	Returns XW_SUCCESS always

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_screen_size (void *adisplay,int *width,int *height)
#else
XW_STATUS Xw_get_screen_size (adisplay,width,height)
void *adisplay;
int *width ;
int *height ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_screen_size",pdisplay) ;
            return (XW_ERROR) ;
        }

	*width = _DWIDTH ;
	*height = _DHEIGHT ;


#ifdef  TRACE_GET_SCREEN_SIZE
if( Xw_get_trace() ) {
    printf (" Xw_get_screen_size(%lx,%d,%d)\n",(long ) pdisplay,*width,*height) ;
}
#endif

	return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_mmscreen_size (void *adisplay,float *width,float *height)
#else
XW_STATUS Xw_get_mmscreen_size (adisplay,width,height)
void *adisplay;
float *width ;
float *height ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_mmscreen_size",pdisplay) ;
            return (XW_ERROR) ;
        }

	*width = WidthMMOfScreen(_DSCREEN) ;
	*height = HeightMMOfScreen(_DSCREEN) ;

#ifdef  TRACE_GET_SCREEN_SIZE
if( Xw_get_trace() ) {
    printf (" Xw_get_mmscreen_size(%lx,%f,%f)\n",(long ) pdisplay,*width,*height) ;
}
#endif

	return (XW_SUCCESS);
}
