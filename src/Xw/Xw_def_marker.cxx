
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DEF_MARKER
#endif

/*
   STATUS Xw_def_marker (amarkmap,index,npoint,spoint,xpoint,ypoint):
   XW_EXT_MARKMAP *amarkmap
   int index			Marker index	0 > x < MAXMARKER
   int npoint			Marker length
   int   *spoint		Marker status point (move-draw boolean)
   float *xpoint		Marker X coords in space -1,+1
   float *ypoint		Marker Y coords in space -1,+1

	Update Marker Extended markmap index with the specified line marker
	descriptor . 
	Marker point must be defined in the float space -1,+1 

	NOTE that marker index 0 is the default marker (.) 
						and cann't be REDEFINED .
	NOTE that the marker size in Meter is decide at the draw_marker time.

	Returns ERROR if BadMarker Index or  Bad marker coords 
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_def_marker (void* amarkmap,
		int index,int npoint,int* spoint,float *xpoint,float *ypoint)
#else
XW_STATUS Xw_def_marker (amarkmap,index,npoint,spoint,xpoint,ypoint)
void *amarkmap;
int index,npoint,*spoint;
float *xpoint,*ypoint;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_MARKMAP *pmarkmap = (XW_EXT_MARKMAP*) amarkmap ;
int i ;

	if ( !Xw_isdefine_markerindex(pmarkmap,index) ) {
	    /*Bad Marker Index*/
	    Xw_set_error(10,"Xw_def_marker",&index) ;
	    return (XW_ERROR) ;
	}

	if( pmarkmap->npoint[index] ) {
	  pmarkmap->npoint[index] = 0;
	  Xw_free(pmarkmap->spoint[index]);
   	  pmarkmap->spoint[index] = NULL;	
	  Xw_free(pmarkmap->xpoint[index]);
   	  pmarkmap->xpoint[index] = NULL;	
	  Xw_free(pmarkmap->ypoint[index]);
   	  pmarkmap->ypoint[index] = NULL;	
	}

	if( npoint > 0  ) {
	  int *s = pmarkmap->spoint[index] = 
			(int*) Xw_malloc(npoint*sizeof(int));
	  float *x = pmarkmap->xpoint[index] = 
			(float*) Xw_malloc(npoint*sizeof(float));
	  float *y = pmarkmap->ypoint[index] = 
			(float*) Xw_malloc(npoint*sizeof(float));
	  if( s && x && y ) { 
	    pmarkmap->npoint[index] = npoint;
	    for( i=0 ; i<npoint ; i++ ) {
	      *s++ = spoint[i];
	      *x++ = xpoint[i];
	      *y++ = ypoint[i];
	    }
	  } else {
	    /*Marker Allocation failed*/
	    Xw_set_error(11,"Xw_def_marker",NULL) ;
	    return (XW_ERROR) ;
	  }
	}

#ifdef  TRACE_DEF_MARKER
if( Xw_get_trace() ) {
  printf(" Xw_def_marker(%lx,%d,%d)\n",(long ) pmarkmap,index,npoint);
  if( Xw_get_trace() > 1 ) {
    for( i=0 ; i<npoint ; i++ ) {
      printf("   point(%d) = {%d,%f,%f}\n",i,spoint[i],xpoint[i],ypoint[i]);
    }
  }
}
#endif

	return (XW_SUCCESS);
}
