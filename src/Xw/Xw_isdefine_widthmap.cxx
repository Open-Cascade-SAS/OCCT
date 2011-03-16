
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_WIDTHMAP
#endif

/*
   XW_STATUS Xw_isdefine_widthmap (awidthmap):
   XW_EXT_WIDTHMAP *awidthmap


	Returns XW_ERROR if Extended Widthmap address is not properly defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_widthmap (void *awidthmap)
#else
XW_STATUS Xw_isdefine_widthmap (awidthmap)
void *awidthmap;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap;
XW_STATUS status = XW_ERROR ;

	if( pwidthmap && (pwidthmap->type == WIDTHMAP_TYPE) ) {
	    status = XW_SUCCESS ;
	}


#ifdef  TRACE_ISDEFINE_WIDTHMAP
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_isdefine_widthmap(%lx)\n",status,(long ) pwidthmap) ;
}
#endif

	return (status);
}
