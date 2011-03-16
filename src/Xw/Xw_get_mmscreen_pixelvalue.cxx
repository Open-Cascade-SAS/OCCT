
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_MMSCREEN_PIXELVALUE
#endif

/*
   float Xw_get_mmscreen_pixelvalue (adisplay,pv):
   XW_EXT_DISPLAY *adisplay
   int pv		Pixel Values 

	Returns Screen value from Pixel value in MM

*/

#ifdef XW_PROTOTYPE
float Xw_get_mmscreen_pixelvalue(void* adisplay,int pv)
#else
float Xw_get_mmscreen_pixelvalue(adisplay,pv)
void *adisplay;
int pv ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;
float sv ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_mmscreen_pixelvalue",pdisplay) ;
            return (XW_ERROR) ;
        }

	sv = (MMPXVALUE(pv) + MMPYVALUE(pv))/2. ;

#ifdef  TRACE_GET_MMSCREEN_PIXELVALUE
if( Xw_get_trace() > 1 ) {
    printf (" %f = Xw_get_mmscreen_pixelvalue(%lx,%d)\n",sv,(long ) pdisplay,pv) ;
}
#endif

	return (sv);
}
