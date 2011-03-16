
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_TILE
#endif

/*
   XW_STATUS Xw_isdefine_tile (atilemap,index):
   XW_EXT_TILEMAP *atilemap
   int index			tile index

	Returns XW_ERROR if BadTile Index or tile is not defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_tile (void *atilemap,int index)
#else
XW_STATUS Xw_isdefine_tile (atilemap,index)
void *atilemap;
int index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*)atilemap;
XW_STATUS status = XW_ERROR ;

	if( index ) {
	    if ( ptilemap && (index < ptilemap->maxtile) 
					&& ptilemap->tiles[index] ) {
	        status = XW_SUCCESS ;
	    }
	} else status = XW_SUCCESS ;


#ifdef  TRACE_ISDEFINE_TILE
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_tile(%lx,%d)\n",status,(long ) ptilemap,index) ;
}
#endif

	return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_tileindex (XW_EXT_TILEMAP *atilemap,int index)
#else
XW_STATUS Xw_isdefine_tileindex (atilemap,index)
XW_EXT_TILEMAP *atilemap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Verify Tile index Range Only
*/
{
XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*)atilemap;
XW_STATUS status = XW_ERROR ;

	if( ptilemap && (index > 0 ) && (index < ptilemap->maxtile) ) {
	   status = XW_SUCCESS ;
	}

#ifdef  TRACE_ISDEFINE_TILE
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_tileindex(%lx,%d)\n",status,(long ) ptilemap,index) ;
}
#endif

	return (status) ;
}
