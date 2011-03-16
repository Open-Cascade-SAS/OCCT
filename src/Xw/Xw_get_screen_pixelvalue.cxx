
#include <Xw_Extension.h>

#ifdef TRACE
#define TRACE_GET_SCREEN_PIXELVALUE
#endif

/*
   float Xw_get_screen_pixelvalue (adisplay,pv):
   XW_EXT_DISPLAY *adisplay
   int pv		Pixel Values 

	Returns screen value from Pixel value in [0,1] Space

*/

#ifdef XW_PROTOTYPE
float Xw_get_screen_pixelvalue(void *adisplay,int pv)
#else
float Xw_get_screen_pixelvalue(adisplay,pv)
void *adisplay;
int pv ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;
float sv ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_screen_pixelvalue",pdisplay) ;
            return (0.) ;
        }

	sv = (float)pv/min(WidthOfScreen(_DSCREEN),HeightOfScreen(_DSCREEN)) ;

#ifdef  TRACE_GET_SCREEN_PIXELVALUE
if( Xw_get_trace() > 1 ) {
    printf (" %f = Xw_get_screen_pixelvalue(%lx,%d)\n",sv,(long ) pdisplay,pv) ;
}
#endif

	return (sv);
}
