
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_FONTMAP
#endif

/*
   XW_STATUS Xw_isdefine_fontmap (afontmap):
   XW_EXT_FONTMAP *afontmap


	Returns XW_ERROR if Extended Fontmap address is not properly defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_fontmap (void* afontmap)
#else
XW_STATUS Xw_isdefine_fontmap (afontmap)
void* afontmap;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_FONTMAP *pfontmap = (XW_EXT_FONTMAP*)afontmap;
XW_STATUS status = XW_ERROR ;

	if( pfontmap && (pfontmap->type == FONTMAP_TYPE) ) {
	    status = XW_SUCCESS ;
	}


#ifdef  TRACE_ISDEFINE_FONTMAP
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_isdefine_fontmap(%lx)\n",status,(long ) pfontmap) ;
}
#endif

	return (status);
}
