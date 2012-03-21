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
#define TRACE_GET_DISPLAY_VISUAL
#endif

/*
   Xw_TypeOfVisual Xw_get_display_visual (adisplay,pclass):
   XW_EXT_DISPLAY *adisplay	Extended Display structure
   Xw_TypeOfVisual pclass	Preferred Visual Class

	Returns the X display attributes of a connexion

	returns XW_DEFAULT_VISUAL if something is wrong
	returns the best Visual class if Successfull

*/

#ifdef XW_PROTOTYPE
Xw_TypeOfVisual Xw_get_display_visual (void *adisplay,Xw_TypeOfVisual pclass)
#else
Xw_TypeOfVisual Xw_get_display_visual (adisplay,pclass)
void   *adisplay ;
Xw_TypeOfVisual  pclass;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;
Xw_TypeOfVisual rclass = Xw_TOV_DEFAULT ;
XVisualInfo *ginfo ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_display_visual",pdisplay) ;
            return (rclass) ;
        }

        if( !(ginfo = Xw_get_visual_info(pdisplay,pclass)) ) return (rclass) ;

#if defined(__cplusplus) || defined(c_plusplus)
    	rclass = (Xw_TypeOfVisual) ginfo->visual->c_class ;
#else
    	rclass = (Xw_TypeOfVisual) ginfo->visual->class ;
#endif

    	XFree((char*)ginfo) ;


#ifdef  TRACE_GET_DISPLAY_VISUAL
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_display_visual(%lx,%d)\n",rclass,(long ) adisplay,pclass) ;
}
#endif

	return (rclass) ;
}
