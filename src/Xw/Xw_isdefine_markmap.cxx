
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_MARKMAP
#endif

/*
   XW_STATUS Xw_isdefine_markmap (amarkmap):
   XW_EXT_MARKMAP *amarkmap


	Returns XW_ERROR if Extended Markmap address is not properly defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_markmap (void *amarkmap)
#else
XW_STATUS Xw_isdefine_markmap (amarkmap)
void *amarkmap;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_MARKMAP *pmarkmap = (XW_EXT_MARKMAP*) amarkmap;
XW_STATUS status = XW_ERROR ;

	if( pmarkmap && (pmarkmap->type == MARKMAP_TYPE) ) {
	    status = XW_SUCCESS ;
	}


#ifdef  TRACE_ISDEFINE_MARKMAP
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_isdefine_markmap(%lx)\n",status,(long ) pmarkmap) ;
}
#endif

	return (status);
}
