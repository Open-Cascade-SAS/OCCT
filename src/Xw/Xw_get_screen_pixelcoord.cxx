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
#define TRACE_GET_SCREEN_PIXELCOORD
#endif

/*
   XW_STATUS Xw_get_screen_pixelcoord (adisplay,px,py,sx,sy):
   XW_EXT_DISPLAY *adisplay
   int px,py		Screen Coordinates defined in Pixels 
   float *sx,*sy		Returns Screen Coordinates defined in [0,1] Space 

	Get screen coordinate from Pixel Space to [0,1] Space

	returns XW_ERROR if Pixel Coordinate is outside the screen
		      or the if Extended Window address is not defined
	returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_screen_pixelcoord(void *adisplay,int px,int py,float *sx,float *sy)
#else
XW_STATUS Xw_get_screen_pixelcoord(adisplay,px,py,sx,sy)
void *adisplay;
int px,py ;
float *sx,*sy ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;
  int displaywidth ;
  int displayheight ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_screen_pixelcoord",pdisplay) ;
            return (XW_ERROR) ;
        }

	displaywidth = WidthOfScreen(_DSCREEN) ;
	displayheight = HeightOfScreen(_DSCREEN) ;

	*sx = (float)px/(float)displaywidth ;
	*sy = 1. - (float)py/(float)displayheight ;

	if( px < 0 || px >= displaywidth || py < 0 || py >= displayheight ) 
							return (XW_ERROR);

#ifdef  TRACE_GET_SCREEN_PIXELCOORD
if( Xw_get_trace() ) 
printf (" Xw_get_screen_pixelcoord(%lx,%d,%d,%f,%f)\n",(long ) pdisplay,px,py,*sx,*sy) ;
#endif

	return (XW_SUCCESS);
}
