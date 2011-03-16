
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_WINDOW_PIXELVALUE
#endif

/*
   float Xw_get_window_pixelvalue (awindow,pv):
   XW_EXT_WINDOW *awindow
   int pv	Pixel Values 

	Returns DWU value from Pixel value

*/

#ifdef XW_PROTOTYPE
float Xw_get_window_pixelvalue(void *awindow,int pv)
#else
float Xw_get_window_pixelvalue(awindow,pv)
void *awindow;
int pv ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
float uv ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_window_pixelvalue",pwindow) ;
            return (XW_ERROR) ;
        }

	uv = UVALUE(pv) ;
	
#ifdef  TRACE_GET_WINDOW_PIXELVALUE
if( Xw_get_trace() > 1 ) {
    printf (" %f = Xw_get_window_pixelvalue(%lx,%d)\n",uv,(long ) pwindow,pv) ;
}
#endif

	return (uv);
}
