
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DEFINE_WIDTH
#endif

/*
   XW_STATUS Xw_isdefine_width (awidthmap,index):
   XW_EXT_WIDTHMAP *awidthmap
   int index			Line width index

	Returns XW_ERROR if BadWidth Index or Width is not defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_width (void *awidthmap,int index)
#else
XW_STATUS Xw_isdefine_width (awidthmap,index)
void *awidthmap;
int index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap;
XW_STATUS status = XW_ERROR ;

    	if( index ) {
	    if ( pwidthmap && (index < pwidthmap->maxwidth) &&
				(pwidthmap->widths[index] > 0) ) {
	        status = XW_SUCCESS ;
	    }
	} else status = XW_SUCCESS ;

#ifdef  TRACE_DEFINE_WIDTH
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_width(%lx,%d)\n",status,(long ) pwidthmap,index) ;
}
#endif

	return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_widthindex  (XW_EXT_WIDTHMAP *awidthmap,int index)
#else
XW_STATUS Xw_isdefine_widthindex  (awidthmap,index)
XW_EXT_WIDTHMAP *awidthmap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Verify Width index range Only
*/
{
XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap;
XW_STATUS status = XW_ERROR ;

	if ( pwidthmap && (index > 0) && (index < pwidthmap->maxwidth) ) {
	    status = XW_SUCCESS ;
	}

#ifdef  TRACE_DEFINE_WIDTH
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_widthindex(%lx,%d)\n",status,(long ) pwidthmap,index) ;
}
#endif

	return (status);
}
