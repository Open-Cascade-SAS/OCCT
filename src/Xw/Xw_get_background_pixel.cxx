
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_BACKGROUND_PIXEL
#endif

/*
   STATUS Xw_get_background_pixel(awindow,pixel):
   XW_EXT_WINDOW *awindow	Window extension structure
   unsigned long *pixel ;	Return Color pixel depending of Visual Class 

	Returns ERROR if Window or background pixel is not defined properly
	Returns SUCCESS if Successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_background_pixel(void* awindow, unsigned long* pixel)
#else
XW_STATUS Xw_get_background_pixel(awindow,pixel)
void *awindow ;
unsigned long *pixel ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_STATUS status = XW_SUCCESS ;

    *pixel = 0;

    if( !pwindow ) {
	/*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error( 24,"Xw_get_background_pixel",pwindow ) ;
        return( XW_ERROR ) ;
    }
    if( pwindow->backindex < 0 ) {
      *pixel = _COLORMAP->backpixel;
    } else if( Xw_isdefine_color(_COLORMAP,pwindow->backindex) ) {
      *pixel = _COLORMAP->pixels[pwindow->backindex];
    } else {
      status = XW_ERROR;
    }

#ifdef TRACE_GET_BACKGROUND_PIXEL
if( Xw_get_trace() > 3 ) {
    printf(" %d = Xw_get_background_pixel(%lx,%lx)\n",
                        status,(long ) pwindow,*pixel) ;
}
#endif

    return( status ) ;
}
