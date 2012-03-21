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
#define TRACE_DEF_BACKGROUND_COLOR
#endif

/*
   XW_STATUS Xw_def_background_color(awindow,r,g,b):
   XW_EXT_WINDOW *awindow
   float r,g,b ;	Red,Green,Blue color value 0. >= x <= 1.

	Update Background Color index with the specified R,G,B values .

	Returns ERROR if Extended Window or Colormap is wrong
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_def_background_color (void* awindow,
					float r,float g,float b)
#else
XW_STATUS Xw_def_background_color (awindow,r,g,b)
void *awindow;
float r,g,b ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_COLORMAP *pcolormap ;
unsigned long backpixel,highpixel ;
XColor color ;
int i,index,cclass,isapproximate ;
XW_STATUS status = XW_SUCCESS;

        if ( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_def_background_color",pwindow) ;
            return (XW_ERROR) ;
        }

        if ( !Xw_isdefine_colormap(_COLORMAP) ) {
            /*ERROR*Bad EXT_COLORMAP Address*/
            Xw_set_error(42,"Xw_def_background_color",_COLORMAP) ;
            return (XW_ERROR) ;
        }

        pcolormap = _COLORMAP ;

	index = pwindow->backindex ;

	if( index >= 0 ) {
	  if( pcolormap->define[index] != BACKCOLOR ) {
	    for( i=0 ; i<pcolormap->maxcolor ; i++ ) {
		if( pcolormap->define[i] == FREECOLOR ) break;
	    }

	    if( i < pcolormap->maxcolor ) {
		index = i ;
		pcolormap->define[i] = BACKCOLOR;
	    } 
	  }

          if( pcolormap->mapping == Xw_TOM_READONLY ) { 
            cclass = StaticColor; 
          } else { 
            cclass = _CCLASS; 
          }

	  switch ( cclass ) {
	    
	    case TrueColor :
		Xw_get_color_pixel(pcolormap,r,g,b,&backpixel,&isapproximate) ;
	    	break ;

	    case PseudoColor :
    		if( pcolormap->mapping == Xw_TOM_COLORCUBE ) {
        	  if( (_CGINFO.red_mult > 0) && (fabs(r-g) < 0.01) && (fabs(r-b) < 0.01) ) {
          	    index = (int)(0.5+r*_CGINFO.red_max)*_CGINFO.red_mult;
          	    if( _CINFO.red_max > 0 ) 
				index += (_CINFO.red_max+1)*(_CINFO.green_max+1)*(_CINFO.blue_max+1);
        	  } else if( _CINFO.red_mult > 0 ) { 
          	    index = ((int)(0.5+r*_CINFO.red_max))*_CINFO.red_mult+
                            ((int)(0.5+g*_CINFO.green_max))*_CINFO.green_mult+
                            ((int)(0.5+b*_CINFO.blue_max))*_CINFO.blue_mult;
       	   	  } else if( _CGINFO.red_mult > 0 ) {
          	    float l = (r+g+b)/3.; 
          	    index = (int)(0.5+l*_CGINFO.red_max)*_CGINFO.red_mult;
        	  } else {
		    index = 0;
		  } 
		  backpixel = _CINFO.base_pixel = index;
		} else if( pcolormap->define[index] == BACKCOLOR ) {
		    color.pixel = backpixel = pcolormap->pixels[index] ;
	            color.red   = (unsigned short) (r * 0xFFFF) ;
	            color.green = (unsigned short) (g * 0xFFFF) ;
	            color.blue  = (unsigned short) (b * 0xFFFF) ;
	            color.flags = DoRed|DoGreen|DoBlue ;
	            XStoreColor(_DISPLAY,_CINFO.colormap,&color) ;
		}
	    	break ;

	    case StaticColor :
                status = Xw_alloc_color(pcolormap,r,g,b,&backpixel,&isapproximate) ;
	    	break ;
	  }

	  if( status ) {
	    pwindow->backindex = index ;
	    pcolormap->pixels[index] = backpixel ;
	  }
	} else {
	  backpixel = pcolormap->backpixel ;	/* Transparent window */
	}

	if( status ) {
	  highpixel = pcolormap->highpixel ^ backpixel ;
    	  XSetWindowBackground (_DISPLAY, _WINDOW, backpixel) ;
    	  XSetBackground(_DISPLAY, pwindow->qgwind.gc, backpixel) ;
    	  XSetBackground(_DISPLAY, pwindow->qgwind.gccopy, backpixel) ;
    	  XSetBackground(_DISPLAY, pwindow->qgwind.gcclear, backpixel) ;
    	  XSetBackground(_DISPLAY, pwindow->qgwind.gchigh, backpixel) ;
    	  XSetForeground(_DISPLAY, pwindow->qgwind.gcclear, backpixel) ;
    	  XSetForeground(_DISPLAY, pwindow->qgwind.gchigh, highpixel) ;
    	  for( i=0 ; i<MAXQG ; i++ ) {
            XSetBackground(_DISPLAY, pwindow->qgline[i].gc, backpixel);
            XSetBackground(_DISPLAY, pwindow->qgpoly[i].gc, backpixel);
            XSetBackground(_DISPLAY, pwindow->qgtext[i].gc, backpixel);
            XSetBackground(_DISPLAY, pwindow->qgmark[i].gc, backpixel);
          }
        }

#ifdef TRACE_DEF_BACKGROUND_COLOR
if( Xw_get_trace() ) {
    printf(" %d = Xw_def_background_color(%lx,%f,%f,%f)\n",status,(long ) pwindow,r,g,b) ;
}
#endif
    return (status);
}
