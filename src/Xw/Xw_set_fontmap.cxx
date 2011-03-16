
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_FONTMAP
#endif

/*
   XW_STATUS Xw_set_fontmap(awindow,afontmap):
   XW_EXT_WINDOW *awindow
   XW_EXT_FONTMAP *afontmap

	Set a fontmap to an existing window 

	Returns XW_ERROR if Bad Fontmap
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_fontmap (void *awindow,void *afontmap)
#else
XW_STATUS Xw_set_fontmap (awindow,afontmap)
void *awindow;
void *afontmap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_FONTMAP *pfontmap = (XW_EXT_FONTMAP*)afontmap ;
    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_set_fontmap",pwindow) ;
        return (XW_ERROR) ;
    }

    if( !Xw_isdefine_fontmap(pfontmap) ) {
        /*ERROR*Bad EXT_FONTMAP Address*/
        Xw_set_error(44,"Xw_set_fontmap",pfontmap) ;
        return (XW_ERROR) ;
    }

    _FONTMAP = pfontmap ;
    _FONTMAP->maxwindow++ ;

#ifdef TRACE_SET_FONTMAP
if( Xw_get_trace() ) {
    printf(" Xw_set_fontmap(%lx,%lx)\n",(long ) pwindow,(long ) pfontmap) ;
}
#endif

	return (XW_SUCCESS);
}
