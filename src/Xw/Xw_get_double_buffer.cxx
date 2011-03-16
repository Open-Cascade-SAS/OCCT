
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_DOUBLE_BUFFER
#endif

/*
   DOUBLEBUFFERMODE Xw_get_double_buffer (awindow)
   XW_EXT_WINDOW *awindow

	Returns the double_buffering status for this window .

	returns SUCCESS always
*/

#ifdef XW_PROTOTYPE
XW_DOUBLEBUFFERMODE Xw_get_double_buffer (void* awindow)
#else
XW_DOUBLEBUFFERMODE Xw_get_double_buffer (awindow)
XW_EXT_WINDOW *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_DOUBLEBUFFERMODE state ;

	if( (_PIXMAP && (_DRAWABLE == _PIXMAP)) ||
	    (_NWBUFFER > 0 && (_DRAWABLE == _BWBUFFER)) ) state = XW_ENABLE ;
	else state = XW_DISABLE ;

#ifdef TRACE_GET_DOUBLE_BUFFER
if( Xw_get_trace() ) {
    printf(" %d = Xw_get_double_buffer(%lx)\n",state,(long ) pwindow) ;
}
#endif
	
    return (state);
}
