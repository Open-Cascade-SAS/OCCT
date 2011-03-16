
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_PIXEL_WINDOWCOORD
#endif

/*
   STATUS Xw_get_pixel_windowcoord (awindow,ux,uy,px,py):
   XW_EXT_WINDOW *awindow
   float ux,uy		Window Coordinates defined in DWU space 
   int *px,*py		Returns Window Coordinates defined in Pixels 

	Get transformed coordinate from User Space to Pixel Space

	NOTE than the current mapping defined by Xw_set_mapping is apply

	returns ERROR if User Coordinate is outside the window
	returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_pixel_windowcoord(void* awindow,float ux,float uy,int* px,int* py)
#else
XW_STATUS Xw_get_pixel_windowcoord(awindow,ux,uy,px,py)
void *awindow;
float ux,uy ;
int *px,*py ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_DISPLAY *pdisplay = pwindow->connexion ;

//OCC186
	*px = PXPOINT(ux, pwindow->xratio) ;
	*py = PYPOINT(uy, pwindow->attributes.height, pwindow->yratio) ;
//OCC186
	
#ifdef  TRACE_GET_PIXEL_WINDOWCOORD
if( Xw_get_trace() > 1 ) {
    printf (" Xw_get_pixel_windowcoord(%lx,%f,%f,%d,%d)\n",(long ) pwindow,ux,uy,*px,*py) ;
}
#endif

	if( !_DGRAB && (*px < 0 || *px > _WIDTH || *py < 0 || *py > _HEIGHT) ) 
							return (XW_ERROR);
	else return (XW_SUCCESS);
}
