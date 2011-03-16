
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_MARKMAP
#endif

/*
   XW_STATUS Xw_set_markmap(awindow,amarkmap):
   XW_EXT_WINDOW *awindow
   XW_EXT_MARKMAP *amarkmap

	Set a markmap to an existing window 

	Returns XW_ERROR if Bad Markmap
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_markmap (void *awindow,void *amarkmap)
#else
XW_STATUS Xw_set_markmap (awindow,amarkmap)
void *awindow;
void *amarkmap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_MARKMAP *pmarkmap = (XW_EXT_MARKMAP*) amarkmap ;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_set_markmap",pwindow) ;
        return (XW_ERROR) ;
    }

    if( !Xw_isdefine_markmap(pmarkmap) ) {
        /*ERROR*Bad EXT_MARKMAP Address*/
        Xw_set_error(46,"Xw_set_markmap",pmarkmap) ;
        return (XW_ERROR) ;
    }

    _MARKMAP = pmarkmap ;
    _MARKMAP->maxwindow++ ;

#ifdef TRACE_SET_MARKMAP
if( Xw_get_trace() ) {
    printf(" Xw_set_markmap(%lx,%lx)\n",(long ) pwindow,(long ) pmarkmap) ;
}
#endif

	return (XW_SUCCESS);
}
