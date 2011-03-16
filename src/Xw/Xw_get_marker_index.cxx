
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_MARKER_INDEX
#endif

/*
   XW_STATUS Xw_get_marker_index(amarkmap,length,spoint,xpoint,ypoint,index):

   XW_EXT_MARKMAP *amarkmap	Markmap extension structure
   int length ;		Style Descriptor length
   int *spoint;		Marker status point vector
   float *xpoint;	Marker x coordinate vector
   float *ypoint;	Marker y coordinate vector
   int *index ;		Return Marker index 0 >= x < MAXMARKER

	Gets the nearest marker index from Style Descriptor values.

	Returns XW_ERROR if BadMarker Index or marker is not defined
	Returns XW_SUCCESS if Successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_marker_index(void *amarkmap,int length,
			int *spoint, float *xpoint, float *ypoint, int *index)
#else
XW_STATUS Xw_get_marker_index(amarkmap,length,spoint,xpoint,ypoint,index)
void *amarkmap ;
int length ;
int *spoint;
float *xpoint;
float *ypoint;
int *index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_MARKMAP *pmarkmap = (XW_EXT_MARKMAP*) amarkmap ;
XW_STATUS status = XW_ERROR ;
int i,j,k,l ;

    if( !pmarkmap ) {
	/*ERROR*Bad EXT_MARKMAP Address*/
	Xw_set_error( 46,"Xw_get_marker_index",pmarkmap ) ;
	return( XW_ERROR ) ;
    }

    if( length > 0 ) {
        for( i=k=0 ; i<pmarkmap->maxmarker ; i++ ) {
	    l = pmarkmap->npoint[i];
	    if( l == length ) {
	      int *s = pmarkmap->spoint[i];
	      float *x = pmarkmap->xpoint[i];
	      float *y = pmarkmap->ypoint[i];
	      for( j=0 ; j<l ; j++,s++,x++,y++ ) {
		if( *s != spoint[j] || 
			*x != xpoint[j] || *y != ypoint[j] ) break;
	      }
	      if( j >= l ) break;
	    } else if( !l && !k ) k = i ;
        }

	if( i<pmarkmap->maxmarker ) {
	    *index = i ;
	    status = XW_SUCCESS ;
	} else {
	    *index = k ;
	    status = Xw_def_marker (pmarkmap,k,length,spoint,xpoint,ypoint) ;
	}
    } else {
	*index = 0 ;
	status = XW_SUCCESS ;
    }

#ifdef TRACE_GET_MARKER_INDEX
if( Xw_get_trace() > 1 ) {
    printf(" %d = Xw_get_marker_index(%lx,%d,%d)\n",
			status,(long ) pmarkmap,length,*index) ;
}
#endif

	return( status ) ;
}
