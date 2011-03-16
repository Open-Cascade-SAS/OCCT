#define CTS50024        /*GG_090997
//              Attention avec les nouvelles machines SGI (O2),
//              le pitch vertical n'est pas identique au pitch horizontal.
*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_WINDOW_RATIO
#endif

/*
   XW_STATUS Xw_set_window_ratio (awindow, ratio):
   XW_EXT_WINDOW *awindow
   float ratio ;

	Update the window ratio.Is defined as the ratio between 
		   the Window User Size and the Window Pixel size
		   DEFAULT is METER  

	returns XW_SUCCESS always
*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_window_ratio (void *awindow , float ratio)
#else
XW_STATUS Xw_set_window_ratio (awindow , ratio)
void *awindow;
float ratio ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_DISPLAY *pdisplay = pwindow->connexion ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_set_window_ratio",pwindow) ;
            return (XW_ERROR) ;
        }

#ifdef CTS50024
	pwindow->xratio = (MMPXVALUE(1) + MMPYVALUE(1)) * ratio / 2. ;
	pwindow->yratio = pwindow->xratio;
#else
	pwindow->xratio = MMPXVALUE(1) * ratio ;
	pwindow->yratio = MMPYVALUE(1) * ratio ;
#endif

#ifdef  TRACE_SET_WINDOW_RATIO
if( Xw_get_trace() ) {
    printf (" Xw_set_window_ratio(%lx,%f)\n",(long ) pwindow,ratio) ;
}
#endif

	return (XW_SUCCESS);
}
