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
#define TRACE_GET_MMSCREEN_PIXELVALUE
#endif

/*
   float Xw_get_mmscreen_pixelvalue (adisplay,pv):
   XW_EXT_DISPLAY *adisplay
   int pv		Pixel Values 

	Returns Screen value from Pixel value in MM

*/

#ifdef XW_PROTOTYPE
float Xw_get_mmscreen_pixelvalue(void* adisplay,int pv)
#else
float Xw_get_mmscreen_pixelvalue(adisplay,pv)
void *adisplay;
int pv ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;
float sv ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_mmscreen_pixelvalue",pdisplay) ;
            return (XW_ERROR) ;
        }

	sv = (MMPXVALUE(pv) + MMPYVALUE(pv))/2. ;

#ifdef  TRACE_GET_MMSCREEN_PIXELVALUE
if( Xw_get_trace() > 1 ) {
    printf (" %f = Xw_get_mmscreen_pixelvalue(%lx,%d)\n",sv,(long ) pdisplay,pv) ;
}
#endif

	return (sv);
}
