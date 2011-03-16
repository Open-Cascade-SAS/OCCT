
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DEF_TILEMAP
#endif

/*
   XW_EXT_TILEMAP* Xw_def_tilemap(adisplay,ntile):
   XW_EXT_DISPLAY *adisplay Extended Display structure

   int ntile		Number of tile cells to be allocated

	Create a tilemap extension 
        allocate the tile cells in the tilemap as if possible
	depending of the MAXTILE define .

	Returns Tilemap extension address if successuful
		or NULL if ERROR

*/

#ifdef XW_PROTOTYPE
void* Xw_def_tilemap (void* adisplay,int ntile)
#else
void* Xw_def_tilemap (adisplay,ntile)
void *adisplay ;
int ntile ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;
XW_EXT_TILEMAP *ptilemap = NULL ;
//int i,tile ;
int i ;

    if( !Xw_isdefine_display(pdisplay) ) {
        /*ERROR*Bad EXT_DISPLAY Address*/
        Xw_set_error(96,"Xw_def_tilemap",pdisplay) ;
        return (NULL) ;
    }

    if( !(ptilemap = Xw_add_tilemap_structure(sizeof(XW_EXT_TILEMAP))) ) 
								return (NULL) ;

    if( ntile <= 0 ) ntile = MAXTILE ;

    ptilemap->connexion = pdisplay ;
    ptilemap->maxtile = min(ntile,MAXTILE) ; 

    for( i=0 ; i<ptilemap->maxtile ; i++ ) {
        ptilemap->tiles[i] = 0 ;
    }

#ifdef TRACE_DEF_TILEMAP
if( Xw_get_trace() ) {
    printf(" %lx = Xw_def_tilemap(%lx,%d)\n", (long ) ptilemap,(long ) adisplay,ntile) ;
}
#endif

    return (ptilemap);
}

static XW_EXT_TILEMAP *PtilemapList =NULL ;

#ifdef XW_PROTOTYPE
XW_EXT_TILEMAP* Xw_add_tilemap_structure(int size)
#else
XW_EXT_TILEMAP* Xw_add_tilemap_structure(size)
int size ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert one Extended tilemap structure in the
        EXtended tilemap List

        returns Extended tilemap address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*) Xw_malloc(size) ;
int i ;

        if( ptilemap ) {
	    ptilemap->type = TILEMAP_TYPE ;
            ptilemap->link = PtilemapList ;
            PtilemapList = ptilemap ;
	    ptilemap->connexion = NULL ;
	    ptilemap->maxtile = 0 ;
	    ptilemap->maxwindow = 0 ;
	    for( i=0 ; i<MAXTILE ; i++ ) {
		ptilemap->tiles[i] = 0 ;
	    }
        } else {
	    /*EXT_TILEMAP allocation failed*/
	    Xw_set_error(17,"Xw_add_tilemap_structure",0) ;
        }

        return (ptilemap) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_tilemap(void* atilemap)
#else
XW_STATUS Xw_close_tilemap(atilemap)
void* atilemap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*)atilemap ;
XW_STATUS status ;

    if( !Xw_isdefine_tilemap(ptilemap) ) {
        /*Bad EXT_TILEMAP Address*/
        Xw_set_error(49,"Xw_close_tilemap",ptilemap) ;
        return (XW_ERROR) ;
    }

    status = Xw_del_tilemap_structure(ptilemap) ;

#ifdef TRACE_DEF_TILEMAP
if( Xw_get_trace() ) {
    printf(" %d = Xw_close_tilemap(%lx)\n",status,(long ) ptilemap) ;
}
#endif

    return (status) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_tilemap_structure(XW_EXT_TILEMAP* atilemap)
#else
XW_STATUS Xw_del_tilemap_structure(atilemap)
XW_EXT_TILEMAP *atilemap;
#endif /*XW_PROTOTYPE*/
/*
        Remove the Extended tilemap address from the Extended List

        returns ERROR if the tilemap address is not Found in the list
        returns SUCCESS if successful
*/
{
XW_EXT_TILEMAP *ptilemap = PtilemapList ;
int i ;

    if( !atilemap ) return (XW_ERROR) ;

    if( atilemap->maxwindow ) --atilemap->maxwindow ;

    if( atilemap->maxwindow ) {
	return (XW_ERROR) ;
    } else {
	for( i=0 ; i<MAXTILE ; i++) {
	    if( atilemap->tiles[i] ) XFreePixmap(_PDISPLAY,atilemap->tiles[i]) ;
	}

        if( atilemap == ptilemap ) {
            PtilemapList = (XW_EXT_TILEMAP*) atilemap->link ;
        } else {
            for( ; ptilemap ; ptilemap = (XW_EXT_TILEMAP*) ptilemap->link ) {
                if( ptilemap->link == atilemap ) {
                    ptilemap->link = atilemap->link ;
                    break ;
                }
            }
        }
	Xw_free(atilemap) ;
    }
    return (XW_SUCCESS) ;
}
