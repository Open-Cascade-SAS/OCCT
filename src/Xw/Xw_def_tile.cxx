
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DEF_TILE
#endif

/*
   STATUS Xw_def_tile (atilemap,index,width,height,cdata):
   XW_EXT_TILEMAP *atilemap
   int index			Tile index	0 >= x < MAXTILE
   int width			Width of tile in pixels
   int height			Height of tile in pixels
   char *cdata			Tile description 

	Update Polygon tile Extended tilemap index with the specified 
	bitmap cdata . 
	Datas must be defined as one char by bit cdata with value 0 or 1
	      so,cdata array must be defined as char cdata[width][height]

	NOTE than tile index 0 is the default Tile (SOLID) 
					and cann't be REDEFINED .

	Returns ERROR if BadTile Index or  Bad tile datas 
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_def_tile (void* atilemap,
			int index,int width,int height,char* cdata)
#else
XW_STATUS Xw_def_tile (atilemap,index,width,height,cdata)
void *atilemap;
int index ;
int width,height ;
char *cdata ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TILEMAP *ptilemap = (XW_EXT_TILEMAP*)atilemap ;
Window root ;
Screen *screen ;
unsigned char *pattern ;
int i,j,l,m,n ;

	if ( !Xw_isdefine_tileindex(ptilemap,index) ) {
	    /*Bad Tile Index*/
	    Xw_set_error(13,"Xw_def_tile",&index) ;
	    return (XW_ERROR) ;
	}

	if( width <= 0 || height <= 0 ) {
	    /*Bad Tile size*/
	    Xw_set_error(14,"Xw_def_tile",&index) ;
	    return (XW_ERROR) ;
	}

	if( !cdata ) {
	    /*Bad Tile data address*/
	    Xw_set_error(15,"Xw_def_tile",&index) ;
	    return (XW_ERROR) ;
	}

	if( ptilemap->tiles[index] ) 
			XFreePixmap(_PDISPLAY,ptilemap->tiles[index]) ;
	ptilemap->tiles[index] = 0 ;

	screen = ScreenOfDisplay(_PDISPLAY,DefaultScreen(_PDISPLAY)) ;
	root = RootWindowOfScreen(screen) ;

	pattern = (unsigned char*) Xw_malloc(width*height/8) ;
	pattern[0] = 0x00 ;
	for( i=n=0,l=8 ; i<width ; i++ ) {
	    for( j=0 ; j<height ; j++ ) {
		m = i*width + j ;
		pattern[n] |= (cdata[m] & 1) << --l ;
		if( !l ) {
		    l = 8 ; 
		    n++ ; 
		    pattern[n] = 0x00 ;
		}
	    }
	}

	ptilemap->tiles[index] = XCreateBitmapFromData(_PDISPLAY,root,
					 (char*)pattern,width,height) ;
	Xw_free(pattern) ;

	if( !ptilemap->tiles[index] ) {
	    /*Tile Bitmap Allocation failed*/
	    Xw_set_error(16,"Xw_def_tile",&index) ;
	    return (XW_ERROR) ;
	}

#ifdef  TRACE_DEF_TILE
if( Xw_get_trace() ) {
    printf (" Xw_def_tile(%lx,%d,%d,%d,%lx)\n",(long ) ptilemap,index,width,height,(long ) cdata);
}
#endif

	return (XW_SUCCESS);
}
