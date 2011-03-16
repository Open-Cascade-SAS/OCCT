
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_PIXEL_MMSCREENVALUE
#endif

/*
   int Xw_get_pixel_mmscreenvalue (adisplay,sv):
   XW_EXT_DISPLAY *adisplay
   float sv		MM Screen Values 

	Returns pixel value from MM Screen value 

*/

#ifdef XW_PROTOTYPE
int Xw_get_pixel_mmscreenvalue(void* adisplay,float sv)
#else
int Xw_get_pixel_mmscreenvalue(adisplay,sv)
void *adisplay;
float sv ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;
int pv ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_pixel_mmscreenvalue",pdisplay) ;
            return (0) ;
        }

	pv = (PMMXVALUE(sv) + PMMYVALUE(sv))/2 ;

#ifdef  TRACE_GET_PIXEL_MMSCREENVALUE
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_pixel_mmscreenvalue(%lx,%f)\n",pv,(long ) pdisplay,sv) ;
}
#endif

	return (pv);
}
