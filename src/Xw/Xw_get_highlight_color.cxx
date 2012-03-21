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
#define TRACE_GET_HIGHLIGHT_COLOR
#define TRACE_GET_HIGHLIGHT_PIXEL
#endif

/*
   XW_STATUS Xw_get_highlight_color(acolormap,r,g,b):
   XW_EXT_COLORMAP *acolormap	Colormap extension structure
   float *r,*g,*b ;		Returns Red,Green,Blue color value 0. >= x <= 1.

	Get R,G,B Highlight Color values from Default index .

	Returns ERROR if Extended Colormap Address is badly defined
	Returns SUCCESS if Successful      

   XW_STATUS Xw_get_highlight_pixel(acolormap,pixel):
   XW_EXT_COLORMAP *acolormap	Colormap extension structure
   unsigned long *pixel;	Returns HighLight pixel

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_highlight_color (void* acolormap,
					float* r,float* g,float* b)
#else
XW_STATUS Xw_get_highlight_color (acolormap,r,g,b)
void *acolormap;
float *r,*g,*b ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap;
XColor color ;

	if ( !Xw_isdefine_colormap(pcolormap) ) {
	    /*ERROR*Bad EXTENDED Colormap ADDRESS*/
	    Xw_set_error(42,"Xw_get_highlight_color",pcolormap) ;
	    return (XW_ERROR) ;
	}

	switch ( _CCLASS ) {

	  case PseudoColor :
	  case StaticColor :
	    color.pixel = pcolormap->highpixel ;
	    XQueryColor(_CDISPLAY,_CINFO.colormap,&color) ;
	    *r = (float)color.red/0xFFFF ;
	    *g = (float)color.green/0xFFFF ;
	    *b = (float)color.blue/0xFFFF ;
	    break ;

	  case TrueColor :
            { unsigned long mask = _CVISUAL->map_entries-1 ;
              unsigned long red,green,blue ;
              unsigned long rmask = _CVISUAL->red_mask ;
              unsigned long gmask = _CVISUAL->green_mask ;
              unsigned long bmask = _CVISUAL->blue_mask ;

	      color.pixel = pcolormap->highpixel ;
              red = color.pixel & rmask ;
              while ( !(rmask & 0x01) ) { rmask >>= 1; red >>= 1; }
 
              green = color.pixel & gmask ;
              while ( !(gmask & 0x01) ) { gmask >>= 1; green >>= 1; }
 
              blue = color.pixel & bmask ;
              while ( !(bmask & 0x01) ) { bmask >>= 1; blue >>= 1; }
 
              *r = (float)red/mask ;
              *g = (float)green/mask ;
              *b = (float)blue/mask ;
	    }
	    break ;

            default :
              /*Unmatchable Visual class*/
              Xw_set_error(67,"Xw_get_highlight_color",&_CCLASS) ;
              return (XW_ERROR) ;
	}



#ifdef TRACE_GET_HIGHLIGHT_COLOR
if( Xw_get_trace() > 1 ) {
    printf(" Xw_get_highlight_color(%lx,%f,%f,%f)\n",(long ) pcolormap,*r,*g,*b) ;
}
#endif

	return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_highlight_pixel (void* acolormap,unsigned long *pixel)
#else
XW_STATUS Xw_get_highlight_pixel (acolormap,pixel)
void *acolormap;
unsigned long *pixel;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap;
//XColor color ;

	if ( !Xw_isdefine_colormap(pcolormap) ) {
	    /*ERROR*Bad EXTENDED Colormap ADDRESS*/
	    Xw_set_error(42,"Xw_get_highlight_pixel",pcolormap) ;
	    return (XW_ERROR) ;
	}

	*pixel = pcolormap->highpixel ;

#ifdef TRACE_GET_HIGHLIGHT_PIXEL
if( Xw_get_trace() > 1 ) {
    printf(" Xw_get_highlight_pixel(%lx,%lx)\n",(long ) pcolormap,*pixel) ;
}
#endif

	return (XW_SUCCESS);
}
