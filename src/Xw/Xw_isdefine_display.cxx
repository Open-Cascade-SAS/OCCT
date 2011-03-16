
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_DISPLAY
#endif

/*
   XW_STATUS Xw_isdefine_display (adisplay):
   XW_EXT_DISPLAY *adisplay


	Returns XW_ERROR if Extended Window address is not properly defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_display (void *adisplay)
#else
XW_STATUS Xw_isdefine_display (adisplay)
void *adisplay;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay;
XW_STATUS status = XW_ERROR ;

	if( pdisplay && (pdisplay->type == DISPLAY_TYPE) ) {
	    status = XW_SUCCESS ;
	}


#ifdef  TRACE_ISDEFINE_DISPLAY
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_isdefine_display(%lx)\n",status,(long ) pdisplay) ;
}
#endif

	return (status);
}
