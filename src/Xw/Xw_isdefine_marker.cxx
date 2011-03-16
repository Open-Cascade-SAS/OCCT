
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_MARKER
#endif

/*
   XW_STATUS Xw_isdefine_marker (amarkmap,index):
   XW_EXT_MARKMAP *amarkmap
   int index			marker index

	Returns XW_ERROR if BadMarker Index or marker is not defined
	Returns XW_SUCCESS if Successful      

*/
#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_marker (void *amarkmap,int index)
#else
XW_STATUS Xw_isdefine_marker (amarkmap,index)
void *amarkmap;
int index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_MARKMAP *pmarkmap = (XW_EXT_MARKMAP*) amarkmap;
XW_STATUS status = XW_ERROR ;

	if( index > 0 ) {
	    if ( pmarkmap && (index < pmarkmap->maxmarker) &&
				(pmarkmap->npoint[index] > 0) ) {
	        return (XW_SUCCESS) ;
	    } 
	} else status = XW_SUCCESS ;


#ifdef  TRACE_ISDEFINE_MARKER
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_marker(%lx,%d)\n",status,(long ) pmarkmap,index) ;
}
#endif

	return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_markerindex (XW_EXT_MARKMAP *amarkmap,int index)
#else
XW_STATUS Xw_isdefine_markerindex (amarkmap,index)
XW_EXT_MARKMAP *amarkmap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Verify Marker range index Only
*/
{
XW_EXT_MARKMAP *pmarkmap = amarkmap;
XW_STATUS status = XW_ERROR ;

	if( pmarkmap && (index > 0) && (index < pmarkmap->maxmarker) ) {
	    return (XW_SUCCESS) ;
	}

#ifdef  TRACE_ISDEFINE_MARKER
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_markerindex(%lx,%d)\n",status,(long ) pmarkmap,index) ;
}
#endif
	return (status) ;
}
