
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_COLOR
#endif

/*
   STATUS Xw_get_color(acolormap,index,r,g,b,pixel):
   XW_EXT_COLORMAP *acolormap	Colormap extension structure
   int index ;		Color index 0 >= x < MAXCOLOR
   float *r,*g,*b ;	Return Red,Green,Blue color value 0. >= x <= 1.
   unsigned long *pixel ;

	Get R,G,B Color values from index .

	Returns ERROR if BadColor Index or color is not defined
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_color (void* acolormap,int index,float* r,float* g,float* b,unsigned long *pixel)
#else
XW_STATUS Xw_get_color (acolormap,index,r,g,b,pixel)
void *acolormap;
int index ;
float *r,*g,*b ;
unsigned long *pixel ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap;
XColor color ;

	*pixel = 0;
	*r = *g = *b = 0.;

	if ( !Xw_isdefine_colorindex(pcolormap,index) ) {
	    /*ERROR*Bad color Index*/
	    Xw_set_error(1,"Xw_get_color",&index) ;
	    return (XW_ERROR) ;
	}


	switch ( _CCLASS ) {

	    case PseudoColor :
	    case StaticColor :
	      color.pixel = pcolormap->pixels[index] ;
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
 
	        color.pixel = pcolormap->pixels[index] ;
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
	      Xw_set_error(67,"Xw_get_color",&_CCLASS) ;
	      return (XW_ERROR) ;
	}

	*pixel = color.pixel ;


#ifdef TRACE_GET_COLOR
if( Xw_get_trace() > 1 ) {
    printf(" Xw_get_color(%lx,%d,%f,%f,%f,%lx)\n",(long ) pcolormap,index,*r,*g,*b,*pixel) ;
}
#endif

	return (XW_SUCCESS);
}
