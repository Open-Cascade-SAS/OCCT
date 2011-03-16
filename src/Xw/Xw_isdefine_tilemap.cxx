
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_TILEMAP
#endif

/*
   XW_STATUS Xw_isdefine_tilemap (atilemap):
   XW_EXT_TILEMAP *atilemap


	Returns XW_ERROR if Extended Tilemap address is not properly defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_tilemap (void *atilemap)
#else
XW_STATUS Xw_isdefine_tilemap (atilemap)
void *atilemap;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*)atilemap;
XW_STATUS status = XW_ERROR ;

	if( ptilemap && (ptilemap->type == TILEMAP_TYPE) ) {
	    status = XW_SUCCESS ;
	}


#ifdef  TRACE_ISDEFINE_TILEMAP
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_isdefine_tilemap(%lx)\n",status,(long ) ptilemap) ;
}
#endif

	return (status);
}
