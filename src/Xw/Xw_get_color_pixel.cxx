
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_COLOR_PIXEL
#endif

/*
   STATUS Xw_get_color_pixel(acolormap,r,g,b,pixel,isapproximate):
   XW_EXT_COLORMAP *acolormap	Colormap extension structure
   float r,g,b ;	Red,Green,Blue color value 0. >= x <= 1.
   unsigned long *pixel ;	Return Color pixel depending of Visual Class 
   bool isapproximate ;

	Gets the nearest color pixel from R,G,B Color values.

	Returns ERROR if Colormap is not defined properly
	Returns SUCCESS if Successful

*/

#define TEST 1
#ifdef TEST	/* Optim. no more necessary with XQueryColors */
#define MAX_COLRS 256
static int indice = 0;
static int indice_max = 0;
static XW_EXT_COLORMAP *qcolormap = NULL;
static float tab_ind[MAX_COLRS][4];
#endif

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_color_pixel(void* acolormap,
		float r,float g,float b,unsigned long* pixel,int *isapproximate)
#else
XW_STATUS Xw_get_color_pixel(acolormap,r,g,b,pixel,isapproximate)
void *acolormap ;
float r,g,b ;
unsigned long *pixel ;
int *isapproximate;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XW_STATUS status = XW_ERROR ;
//XColor colors[MAXCOLOR] ;
//int color_indexs[MAXCOLOR] ;
//register i,j,n ;
register int i ;

    *pixel = 0;
    if( !pcolormap ) {
	/*ERROR*Bad EXT_COLORMAP Address*/
	Xw_set_error( 42,"Xw_get_color_pixel",pcolormap ) ;
	return( XW_ERROR ) ;
    }

    *isapproximate = False;
    if( _CCLASS == TrueColor ) {
        unsigned long mask = _CVISUAL->map_entries-1 ;
        unsigned long red   = (unsigned long) (r * mask) ;
        unsigned long green = (unsigned long) (g * mask) ;
        unsigned long blue  = (unsigned long) (b * mask) ;

	mask = _CVISUAL->red_mask;
        while ( !(mask & 0x01) ) { mask >>= 1; red <<= 1; }
	mask = _CVISUAL->green_mask;
        while ( !(mask & 0x01) ) { mask >>= 1; green <<= 1; }
	mask = _CVISUAL->blue_mask;
        while ( !(mask & 0x01) ) { mask >>= 1; blue <<= 1; }
	*pixel = red|green|blue ;
	status = XW_SUCCESS ;
    } else if( pcolormap->mapping == Xw_TOM_COLORCUBE ) {
	int curind = 0;
        if( (_CGINFO.red_mult > 0) && (fabs(r-g) < 0.01) && (fabs(r-b) < 0.01) ) {
          curind = (int)(0.5+r*_CGINFO.red_max)*_CGINFO.red_mult;
          if( _CINFO.red_max > 0 ) curind += (_CINFO.red_max+1)*(_CINFO.green_max+1)*(_CINFO.blue_max+1);
        } else if( _CINFO.red_mult > 0 ) {
          curind = ((int)(0.5+r*_CINFO.red_max))*_CINFO.red_mult+
                            ((int)(0.5+g*_CINFO.green_max))*_CINFO.green_mult+
                            ((int)(0.5+b*_CINFO.blue_max))*_CINFO.blue_mult;
        } else if( _CGINFO.red_mult > 0 ) {
          float l = (r+g+b)/3.;
          curind = (int)(0.5+l*_CGINFO.red_max)*_CGINFO.red_mult;
        }
        *pixel = _CINFO.base_pixel + curind ;
	if( pcolormap->pixels[curind] == *pixel ) status = XW_SUCCESS ;
    } 

    if( !status ) {

#ifdef TEST
      if (pcolormap != qcolormap) {
	for (i=0; i<MAX_COLRS; i++) tab_ind[i][0] = -1.;
	qcolormap = pcolormap;
	indice_max = 0;
	indice = 0;
      }

      for (i=0; i<indice_max; i++) {
	if (tab_ind[i][0] >= 0.) {
		if ((tab_ind[i][1] == r) &&
			    (tab_ind[i][2] == g) &&
			    (tab_ind[i][3] == b)) {
		  *pixel	= (int) tab_ind[i][0];
		  status	= XW_SUCCESS;
		  break;
		}
	}
      }
#endif

      if( !status ) {
	status = Xw_alloc_color(pcolormap,r,g,b,pixel,isapproximate) ; 

#ifdef TEST
	if( status ) {
	  tab_ind[indice][0] = *pixel ;
	  tab_ind[indice][1] = r ;
	  tab_ind[indice][2] = g ;
	  tab_ind[indice][3] = b ;
	/*
	 * Une fois que le tableau est  plein, il faut le
	 * parcourir completement, donc on ne touche plus
	 * a indice_max.
	 */
	  indice ++ ;
	  if( indice < MAX_COLRS ) {
	      indice_max = max(indice_max,indice);
	  } else {
	      indice = 0 ; /* on repart a 0 */
#ifdef TRACE_GET_COLOR_PIXEL
			if( Xw_get_trace() > 1 ) {
			    printf(" ************************\n") ;
			    printf(" Xw_get_color_pixel, full array\n") ;
			    printf(" ************************\n") ;
			}
#endif
	  }
	}
#endif
      }
    }


#ifdef TRACE_GET_COLOR_PIXEL
if( Xw_get_trace() > 3 ) {
    printf(" %d = Xw_get_color_pixel(%lx,%f,%f,%f,%lx,%d)\n",
			status,(long ) pcolormap,r,g,b,*pixel,*isapproximate) ;
}
#endif

    return( status ) ;
}
