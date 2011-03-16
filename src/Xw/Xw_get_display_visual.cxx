
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_DISPLAY_VISUAL
#endif

/*
   Xw_TypeOfVisual Xw_get_display_visual (adisplay,pclass):
   XW_EXT_DISPLAY *adisplay	Extended Display structure
   Xw_TypeOfVisual pclass	Preferred Visual Class

	Returns the X display attributes of a connexion

	returns XW_DEFAULT_VISUAL if something is wrong
	returns the best Visual class if Successfull

*/

#ifdef XW_PROTOTYPE
Xw_TypeOfVisual Xw_get_display_visual (void *adisplay,Xw_TypeOfVisual pclass)
#else
Xw_TypeOfVisual Xw_get_display_visual (adisplay,pclass)
void   *adisplay ;
Xw_TypeOfVisual  pclass;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;
Xw_TypeOfVisual rclass = Xw_TOV_DEFAULT ;
XVisualInfo *ginfo ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_display_visual",pdisplay) ;
            return (rclass) ;
        }

        if( !(ginfo = Xw_get_visual_info(pdisplay,pclass)) ) return (rclass) ;

#if defined(__cplusplus) || defined(c_plusplus)
    	rclass = (Xw_TypeOfVisual) ginfo->visual->c_class ;
#else
    	rclass = (Xw_TypeOfVisual) ginfo->visual->class ;
#endif

    	XFree((char*)ginfo) ;


#ifdef  TRACE_GET_DISPLAY_VISUAL
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_display_visual(%lx,%d)\n",rclass,(long ) adisplay,pclass) ;
}
#endif

	return (rclass) ;
}
