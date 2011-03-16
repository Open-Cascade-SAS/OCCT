
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_DISPLAY_INFO
#endif

/*
   XW_STATUS Xw_get_display_info (adisplay,display,root,colormap,tclass,depth):
   XW_EXT_DISPLAY *adisplay	Extended Display structure
   Aspect_Handle **display		Display XID 
   Aspect_Handle *root			Root XID
   Aspect_Handle *colormap		Default Colormap XID	
   Xw_TypeOfVisual *tclass	Default Visual Class
   int *depth			Default Depth

	Returns the X display attributes of a connexion

	returns XW_ERROR if something is wrong
	returns XW_SUCCESS else

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_display_info (void *adisplay,Aspect_Handle **display,Aspect_Handle *root,Aspect_Handle *colormap,Xw_TypeOfVisual  *tclass,int *depth)
#else
XW_STATUS Xw_get_display_info (adisplay,display,root,colormap,tclass,depth)
void   *adisplay ;
Aspect_Handle **display;
Aspect_Handle *root;
Aspect_Handle *colormap;
Xw_TypeOfVisual  *tclass;
int *depth ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_display_info",pdisplay) ;
            return (XW_ERROR) ;
        }

	*display = (Aspect_Handle*) _DDISPLAY ;
	*root = _DROOT ;
	*tclass = (Xw_TypeOfVisual) _DCLASS ;
	*colormap = _DCOLORMAP ;
	*depth = DefaultDepthOfScreen(_DSCREEN) ;

#ifdef  TRACE_GET_DISPLAY_INFO
if( Xw_get_trace() > 1 ) {
    printf (" Xw_get_display_info(%lx,%lx,%lx,%lx,%d,%d)\n",
			(long ) adisplay,(long ) *display,*root,*colormap,*tclass,*depth) ;
}
#endif

	return (XW_SUCCESS) ;
}
