
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_TILEMAP
#endif

/*
   XW_STATUS Xw_set_tilemap(awindow,atilemap):
   XW_EXT_WINDOW *awindow
   XW_EXT_TILEMAP *atilemap

	Set a tilemap to an existing window 

	Returns XW_ERROR if Bad Tilemap
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_tilemap (void *awindow,void *atilemap)
#else
XW_STATUS Xw_set_tilemap (awindow,atilemap)
void *awindow;
void *atilemap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*)atilemap ;
    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_set_colormap",pwindow) ;
        return (XW_ERROR) ;
    }

    if( !Xw_isdefine_tilemap(ptilemap) ) {
        /*ERROR*Bad EXT_TILEMAP Address*/
        Xw_set_error(49,"Xw_set_tilemap",ptilemap) ;
        return (XW_ERROR) ;
    }

    _TILEMAP = ptilemap ;
    _TILEMAP->maxwindow++ ;

#ifdef TRACE_SET_TILEMAP
if( Xw_get_trace() ) {
    printf(" Xw_set_tilemap(%lx,%lx)\n",(long ) pwindow,(long ) ptilemap) ;
}
#endif

	return (XW_SUCCESS);
}
