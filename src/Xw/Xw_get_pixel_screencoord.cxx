
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_PIXEL_SCREENCOORD
#endif

/*
   STATUS Xw_get_screen_pixelcoord (adisplay,sx,sy,px,py):
   XW_EXT_DISPLAY *adisplay
   float sx,sy		Screen Coordinates defined in DSU Space 
   int px,py		Returns Screen Coordinates defined in Pixels 

	Get screen coordinate from Screen in DSU Space

	returns ERROR if resulting Pixel Coordinate is outside the screen
		      or the if Extended Window address is not defined
	returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_pixel_screencoord(void* adisplay,float sx,float sy,int* px,int* py)
#else
XW_STATUS Xw_get_pixel_screencoord(adisplay,sx,sy,px,py)
void *adisplay;
float sx,sy ;
int *px,*py ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;
int displaywidth ;
int displayheight ;

	if( !Xw_isdefine_display(pdisplay) ) {
	    /*ERROR*Bad EXT_DISPLAY Address*/
	    Xw_set_error(96,"Xw_get_pixel_screencoord",pdisplay) ;
	    return (XW_ERROR) ;
	}

	displaywidth = WidthOfScreen(_DSCREEN) ;
	displayheight = HeightOfScreen(_DSCREEN) ;

	*px = (int)(sx*(float)displaywidth) ;
	*py = (int)((1.-sy)*(float)displayheight) ;

	if( *px < 0 || *px >= displaywidth || *py < 0 || *py >= displayheight ) 
							return (XW_ERROR);

#ifdef  TRACE_GET_PIXEL_SCREENCOORD
if( Xw_get_trace() > 1 ) {
  printf (" Xw_get_pixel_screencoord(%lx,%f,%f,%d,%d)\n",(long ) pdisplay,sx,sy,*px,*py) ;
}
#endif

	return (XW_SUCCESS);
}
