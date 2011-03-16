
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DEF_COLOR
#endif

/*
   XW_STATUS Xw_def_color(acolormap,index,r,g,b):
   XW_EXT_COLORMAP *acolormap
   int index ;		Color index 0 >= x < MAXCOLOR
   float r,g,b ;	Red,Green,Blue color value 0. >= x <= 1.

	Update Color Extended colormap index with the specified R,G,B values .

	Returns ERROR if BadColor Index
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_def_color (void* acolormap,
			int index,float r,float g,float b)
#else
XW_STATUS Xw_def_color (acolormap,index,r,g,b)
void *acolormap;
int index ;
float r,g,b ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XColor color ;
int cclass,isapproximate;
unsigned long pixel;
XW_STATUS status = XW_SUCCESS;

       if ( !Xw_isdefine_colorindex(pcolormap,index) ) {
	    /*ERROR*Bad Color Index*/
	    Xw_set_error(1,"Xw_def_color",&index) ;
	    return (XW_ERROR) ;
        }

	if( pcolormap->mapping == Xw_TOM_READONLY ) {
	  cclass = StaticColor;
	} else {
	  cclass = _CCLASS;
	}

	switch (cclass) {
	    
	    case TrueColor :
		Xw_get_color_pixel(pcolormap,r,g,b,&pixel,&isapproximate) ;
	    	break ;

	    case PseudoColor :
    		if( pcolormap->mapping == Xw_TOM_COLORCUBE ) {
		  int kindex;
        	  if( (_CGINFO.red_mult > 0) && (fabs(r-g) < 0.01) && (fabs(r-b) < 0.01) ) {
          	    kindex = (int)(0.5+r*_CGINFO.red_max)*_CGINFO.red_mult;
          	    if( _CINFO.red_max > 0 ) 
				kindex += (_CINFO.red_max+1)*(_CINFO.green_max+1)*(_CINFO.blue_max+1);
        	  } else if( _CINFO.red_mult > 0 ) {
          	    kindex = ((int)(0.5+r*_CINFO.red_max))*_CINFO.red_mult+
                            ((int)(0.5+g*_CINFO.green_max))*_CINFO.green_mult+
                            ((int)(0.5+b*_CINFO.blue_max))*_CINFO.blue_mult;
        	  } else if( _CGINFO.red_mult > 0 ) {
          	    float l = (r+g+b)/3.;
          	    kindex = (int)(0.5+l*_CGINFO.red_max)*_CGINFO.red_mult;
        	  } else {
          	    kindex = 0;
        	  }
		  pixel = _CINFO.base_pixel + kindex;
		} else {
		    color.pixel = pixel = pcolormap->pixels[index] ;
	            color.red   = (unsigned short) (r * 0xFFFF) ;
	            color.green = (unsigned short) (g * 0xFFFF) ;
	            color.blue  = (unsigned short) (b * 0xFFFF) ;
	            color.flags = DoRed|DoGreen|DoBlue ;
	            XStoreColor(_CDISPLAY,_CINFO.colormap,&color) ;
		}
	    	break ;

	    case StaticColor :
	        status = Xw_alloc_color(pcolormap,r,g,b,&pixel,&isapproximate) ;
	    	break ;
	}

	if( status ) {
          pcolormap->define[index] = USERCOLOR ;
	  pcolormap->pixels[index] = pixel ;
	}

#ifdef TRACE_DEF_COLOR
if( Xw_get_trace() ) {
    printf(" %d = Xw_def_color(%lx,%d,%f,%f,%f)\n",status,(long ) pcolormap,index,r,g,b) ;
}
#endif
    return (status);
}
