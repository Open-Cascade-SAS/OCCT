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
#define TRACE_GET_WINDOW_VISUAL
#endif

/*
   Xw_TypeOfVisual Xw_get_window_visual (awindow):
   XW_EXT_WINDOW *awindow

	Returns the window  Visual class ,must be one of :

		Xw_TOV_STATICGRAY,
		Xw_TOV_GRAYSCALE,
		Xw_TOV_STATICCOLOR,
		Xw_TOV_PSEUDOCOLOR,
		Xw_TOV_TRUECOLOR,
		Xw_TOV_DIRECTCOLOR
*/

#ifdef XW_PROTOTYPE
Xw_TypeOfVisual Xw_get_window_visual (void *awindow)
#else
Xw_TypeOfVisual Xw_get_window_visual (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_window_visual",pwindow) ;
            return (Xw_TOV_DEFAULT) ;
        }

#ifdef  TRACE_GET_WINDOW_VISUAL
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_window_visual(%lx)\n",_CLASS,(long ) pwindow) ;
}
#endif

	return (Xw_TypeOfVisual) _CLASS;
}
