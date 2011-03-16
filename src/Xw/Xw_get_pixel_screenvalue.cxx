
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_PIXEL_SCREENVALUE
#endif

/*
   int Xw_get_pixel_screenvalue (adisplay,sv):
   XW_EXT_DISPLAY *adisplay
   float sv		Screen Values in [0,1] Space

	Returns pixel value from screen value

*/

#ifdef XW_PROTOTYPE
int Xw_get_pixel_screenvalue(void* adisplay,float sv)
#else
int Xw_get_pixel_screenvalue(adisplay,sv)
void *adisplay;
float sv ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;
int pv ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_pixel_screenvalue",pdisplay) ;
            return (0) ;
        }

	pv = (int)(sv*min(WidthOfScreen(_DSCREEN),HeightOfScreen(_DSCREEN))) ;

#ifdef  TRACE_GET_PIXEL_SCREENVALUE
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_pixel_screenvalue(%lx,%f)\n",pv,(long ) pdisplay,sv) ;
}
#endif

	return (pv);
}
