
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DEF_HIGHLIGHT_COLOR
#endif

/*
   STATUS Xw_def_highlight_color(acolormap,r,g,b):
   XW_EXT_COLORMAP *acolormap
   float r,g,b ;	Red,Green,Blue color value 0. >= x <= 1.

	Update Highlight Color index with the specified R,G,B values .
	NOTE than highlight color is shared between all Windows

	Returns ERROR if Extended Colormap is not properly defined
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_def_highlight_color (void* acolormap,
					float r,float g,float b)
#else
XW_STATUS Xw_def_highlight_color (acolormap,r,g,b)
void *acolormap;
float r,g,b ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XColor color ;
int i,cclass,index = -1,isapproximate ;
XW_STATUS status = XW_SUCCESS;
unsigned long highpixel;

        if ( !Xw_isdefine_colormap(pcolormap) ) {
	     /*ERROR*Bad EXT_COLORMAP Address*/
	     Xw_set_error(42,"Xw_def_highlight_color",pcolormap) ;
	     return (XW_ERROR) ;
        }

        if( pcolormap->mapping == Xw_TOM_READONLY ) { 
          cclass = StaticColor; 
        } else { 
          cclass = _CCLASS; 
        }

	switch ( cclass ) {
	    
	    case TrueColor :
                Xw_get_color_pixel(pcolormap,r,g,b,&highpixel,&isapproximate) ;
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
            	    highpixel = _CINFO.base_pixel + index ;
        	} else if( pcolormap->mapping == Xw_TOM_BESTRAMP ) {
	            color.red   = (unsigned short) (r * 0xFFFF) ;
	            color.green = (unsigned short) (g * 0xFFFF) ;
	            color.blue  = (unsigned short) (b * 0xFFFF) ;
	            color.flags = DoRed|DoGreen|DoBlue ;
		    for( i=0 ; i<pcolormap->maxcolor ; i++ ) {
		      if( (pcolormap->pixels[i] & 1) &&
			    ((pcolormap->define[i] == FREECOLOR) ||
			     (pcolormap->define[i] == HIGHCOLOR)) ) {
			 pcolormap->define[i] = HIGHCOLOR ;
			 color.pixel =  pcolormap->pixels[i] ;
			 XStoreColor(_CDISPLAY,_CINFO.colormap,&color) ;
		       }
		    }
		    highpixel = pcolormap->pixels[1];
        	} else if( pcolormap->mapping == Xw_TOM_SIMPLERAMP ) {
	            Xw_get_color_index(pcolormap,r,g,b,&index) ;
            	    highpixel = _CINFO.base_pixel + index ;
		}
	    	break ;

	    case StaticColor :
                status = Xw_alloc_color(pcolormap,r,g,b,&highpixel,&isapproximate) ;
	    	break ;
	}

	if( status ) {
	  if( index >= 0 ) {
	    pcolormap->define[index] = HIGHCOLOR ;
	  }
	  pcolormap->highpixel  = highpixel ;
#ifdef BUG_PRO3709
	  Xw_reset_attrib(NULL) ;
#endif
	}

#ifdef TRACE_DEF_HIGHLIGHT_COLOR
if( Xw_get_trace() ) {
    printf(" %d = Xw_def_highlight_color(%lx,%f,%f,%f)\n",status,(long ) pcolormap,r,g,b) ;
}
#endif
	return (status);
}
