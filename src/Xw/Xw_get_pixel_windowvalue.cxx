
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_PIXEL_WINDOWVALUE
#endif

/*
   int Xw_get_pixel_windowvalue (awindow,uv):
   XW_EXT_WINDOW *awindow
   float uv	User Value defined in DWU space

	Returns  Pixel value from transformed User Value 

	NOTE than the current mapping defined by Xw_set_mapping is apply

*/

#ifdef XW_PROTOTYPE
int Xw_get_pixel_windowvalue(void *awindow,float uv)
#else
int Xw_get_pixel_windowvalue(awindow,uv)
void *awindow;
float uv ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
  int pv ;  

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_pixel_windowvalue",pwindow) ;
            return (0) ;
        }

//OCC186
	pv = PVALUE( uv, pwindow->xratio, pwindow->yratio );
//OCC186
	
#ifdef  TRACE_GET_PIXEL_WINDOWVALUE
if( Xw_get_trace() > 1 ) {
	printf (" %d = Xw_get_pixel_windowvalue(%lx,%f)\n",pv,(long ) pwindow,uv) ;
}
#endif
	return (pv) ;
}
