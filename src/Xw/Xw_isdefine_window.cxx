
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_WINDOW
#endif

/*
   XW_STATUS Xw_isdefine_window (awindow):
   XW_EXT_WINDOW *awindow


	Returns XW_ERROR if Extended Window address is not properly defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_window (void *awindow)
#else
XW_STATUS Xw_isdefine_window (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_STATUS status = XW_ERROR ;

	if( pwindow && (pwindow->type == WINDOW_TYPE) ) {
	    status = XW_SUCCESS ;
	}


#ifdef  TRACE_ISDEFINE_WINDOW
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_window(%lx)\n",status,(long ) pwindow) ;
}
#endif

	return (status);
}
