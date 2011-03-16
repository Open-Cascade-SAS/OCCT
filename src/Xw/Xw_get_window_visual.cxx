
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_WINDOW_VISUAL
#endif

/*
   Xw_TypeOfVisual Xw_get_window_visual (awindow):
   XW_EXT_WINDOW *awindow

	Returns the window  Visual class ,must be one of :

		Xw_TOV_STATICGRAY,
		Xw_TOV_GRAYSCALE,
		Xw_TOV_STATICCOLOR,
		Xw_TOV_PSEUDOCOLOR,
		Xw_TOV_TRUECOLOR,
		Xw_TOV_DIRECTCOLOR
*/

#ifdef XW_PROTOTYPE
Xw_TypeOfVisual Xw_get_window_visual (void *awindow)
#else
Xw_TypeOfVisual Xw_get_window_visual (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_window_visual",pwindow) ;
            return (Xw_TOV_DEFAULT) ;
        }

#ifdef  TRACE_GET_WINDOW_VISUAL
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_window_visual(%lx)\n",_CLASS,(long ) pwindow) ;
}
#endif

	return (Xw_TypeOfVisual) _CLASS;
}
