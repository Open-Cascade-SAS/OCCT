
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_COLORMAP
#endif

/*
   XW_STATUS Xw_isdefine_colormap (acolormap):
   XW_EXT_COLORMAP *acolormap


	Returns XW_ERROR if Extended Colormap address is not properly defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_colormap (void* acolormap)
#else
XW_STATUS Xw_isdefine_colormap (acolormap)
void* acolormap;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XW_STATUS status = XW_ERROR ;

	if( pcolormap && (pcolormap->type == COLORMAP_TYPE) ) {
	    status = XW_SUCCESS ;
	}


#ifdef  TRACE_ISDEFINE_COLORMAP
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_isdefine_colormap(%lx)\n",status,(long ) pcolormap) ;
}
#endif

	return (status);
}
