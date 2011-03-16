
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_WINDOW_PIXELCOORD
#endif

/*
   XW_STATUS Xw_get_window_pixelcoord (awindow,px,py,ux,uy):
   XW_EXT_WINDOW *awindow
   int px,py		Window Coordinates defined in Pixels 
   float *ux,*uy		Returns Window Coordinates defined in User Space 

	Get transformed coordinate from Pixel Space to DWU space 

	returns XW_ERROR if Pixel Coordinate is outside the window
	returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_window_pixelcoord(void *awindow,int px,int py,float *ux,float *uy)
#else
XW_STATUS Xw_get_window_pixelcoord(awindow,px,py,ux,uy)
void *awindow;
int px,py ;
float *ux,*uy ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
//XW_EXT_DISPLAY *pdisplay = pwindow->connexion ;
float size ;

	size = ( _WIDTH >= _HEIGHT) ? _WIDTH : _HEIGHT ;
	*ux = UXPOINT(px) ;
	*uy = UYPOINT(py) ;
	
#ifdef  TRACE_GET_WINDOW_PIXELCOORD
if ( Xw_get_trace() > 1 ) {
    printf (" Xw_get_window_pixelcoord(%lx,%d,%d,%f,%f)\n",(long ) pwindow,px,py,*ux,*uy) ;
}
#endif

	return (XW_SUCCESS);
}
