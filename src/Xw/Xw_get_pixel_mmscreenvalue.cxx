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
#define TRACE_GET_PIXEL_MMSCREENVALUE
#endif

/*
   int Xw_get_pixel_mmscreenvalue (adisplay,sv):
   XW_EXT_DISPLAY *adisplay
   float sv		MM Screen Values 

	Returns pixel value from MM Screen value 

*/

#ifdef XW_PROTOTYPE
int Xw_get_pixel_mmscreenvalue(void* adisplay,float sv)
#else
int Xw_get_pixel_mmscreenvalue(adisplay,sv)
void *adisplay;
float sv ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;
int pv ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_pixel_mmscreenvalue",pdisplay) ;
            return (0) ;
        }

	pv = (PMMXVALUE(sv) + PMMYVALUE(sv))/2 ;

#ifdef  TRACE_GET_PIXEL_MMSCREENVALUE
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_pixel_mmscreenvalue(%lx,%f)\n",pv,(long ) pdisplay,sv) ;
}
#endif

	return (pv);
}
