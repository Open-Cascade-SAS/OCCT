
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_WIDTH_INDEX
#endif

/*
   XW_STATUS Xw_get_width_index(awidthmap,width,index):

   XW_EXT_WIDTHMAP *awidthmap	Widthmap extension structure
   float width ;	Line Width in MM
   int *index ;		Return Width index 0 >= x < MAXWIDTH

	Gets the nearest width index from width.

	Returns XW_ERROR if BadWidth Index or width is not defined
	Returns XW_SUCCESS if Successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_width_index(void *awidthmap,float width,int *index)
#else
XW_STATUS Xw_get_width_index(awidthmap,width,index)
void *awidthmap ;
float width ;
int *index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap ;
XW_STATUS status = XW_ERROR ;
Screen *screen ;
int i,j,k,pwidth ;

    if( !pwidthmap ) {
	/*ERROR*Bad EXT_WIDTHMAP Address*/
	Xw_set_error( 53,"Xw_get_width_index",pwidthmap ) ;
	return( XW_ERROR ) ;
    }

    screen = ScreenOfDisplay(_WDISPLAY,DefaultScreen(_WDISPLAY)) ;

    pwidth = (int) (0.5 + width * (float)HeightOfScreen(screen)/
                                        (float)HeightMMOfScreen(screen)) ;

    pwidth = (pwidth) ? pwidth : 1 ;

    for( i=j=k=0 ; i<pwidthmap->maxwidth ; i++ ) {
        if( pwidthmap->widths[i] ) {
	   if( pwidth == pwidthmap->widths[i] ) break;
	   else if( abs(pwidth - pwidthmap->widths[i]) < 
			abs(pwidth - pwidthmap->widths[k]) ) k = i;
	} else if( !j ) j = i ;
    }

    if( i<pwidthmap->maxwidth ) {
	*index = i ;
	status = XW_SUCCESS ;
    } else if( j > 0 ) {
	*index = j ;
	status = Xw_def_width (pwidthmap,j,width) ;
    } else {
	*index = k ;
	status = XW_SUCCESS ;
    }

#ifdef TRACE_GET_WIDTH_INDEX
if( Xw_get_trace() > 1 ) {
    printf(" %d = Xw_get_width_index(%lx,%f,%d) is %d\n",
			status,(long ) pwidthmap,width,*index,pwidthmap->widths[*index]) ;
}
#endif

	return( status ) ;
}
