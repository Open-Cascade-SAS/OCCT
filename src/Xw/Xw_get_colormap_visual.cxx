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
#define TRACE_GET_COLORMAP_VISUAL
#endif

/*
   Xw_TypeOfVisual Xw_get_colormap_visual (acolormap):
   XW_EXT_COLORMAP *acolormap

	Returns the colormap  Visual class ,must be one of :

		STATICGRAY,GRAYSCALE,STATICCOLOR,PSEUDOCOLOR,
		TRUECOLOR,DIRECTCOLOR
*/

#ifdef XW_PROTOTYPE
Xw_TypeOfVisual Xw_get_colormap_visual (void* acolormap)
#else
Xw_TypeOfVisual Xw_get_colormap_visual (acolormap)
void *acolormap;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap;

        if( !Xw_isdefine_colormap(pcolormap) ) {
            /*ERROR*Bad EXT_COLORMAP Address*/
            Xw_set_error(42,"Xw_get_colormap_visual",pcolormap) ;
            return (Xw_TOV_DEFAULT) ;
        }

#ifdef  TRACE_GET_COLORMAP_VISUAL
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_colormap_visual(%lx)\n",_CCLASS,(long ) pcolormap) ;
}
#endif

	return (Xw_TypeOfVisual) _CCLASS ;
}
