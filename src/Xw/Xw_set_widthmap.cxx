
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_WIDTHMAP
#endif

/*
   XW_STATUS Xw_set_widthmap(awindow,awidthmap):
   XW_EXT_WINDOW *awindow
   XW_EXT_WIDTHMAP *awidthmap

	Set a widthmap to an existing window 

	Returns XW_ERROR if Bad Widthmap
	Returns XW_SUCCESS if successful

*/
#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_widthmap (void *awindow,void *awidthmap)
#else
XW_STATUS Xw_set_widthmap (awindow,awidthmap)
void *awindow;
void *awidthmap ;
#endif /*XW_PROTOTYPE*/
{

  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
  XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap ;
    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_set_colormap",pwindow) ;
        return (XW_ERROR) ;
    }

    if( !Xw_isdefine_widthmap(pwidthmap) ) {
        /*ERROR*Bad EXT_WIDTHMAP Address*/
        Xw_set_error(23,"Xw_set_widthmap",pwidthmap) ;
        return (XW_ERROR) ;
    }

    _WIDTHMAP = pwidthmap ;
    _WIDTHMAP->maxwindow++ ;

#ifdef TRACE_SET_WIDTHMAP
if( Xw_get_trace() ) {
    printf(" Xw_set_widthmap(%lx,%lx)\n",(long ) pwindow,(long ) pwidthmap) ;
}
#endif

	return (XW_SUCCESS);
}
