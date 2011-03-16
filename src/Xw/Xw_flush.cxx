
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_FLUSH
#endif

/*
   STATUS Xw_flush (awindow,synchro)
   XW_EXT_WINDOW *awindow
				
	Flush all primitives in the previous window . 

	Returns SUCCESS always

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_flush (void* awindow,int synchro)
#else
XW_STATUS Xw_flush (awindow,synchro)
void *awindow;
int synchro;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;

	if( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXT_WINDOW Address*/
	    Xw_set_error(24,"Xw_flush",pwindow) ;
	    return (XW_ERROR) ;
	}

	if( synchro ) {
	    XSync(_DISPLAY,False) ;
	} else {
            XFlush(_DISPLAY) ;
	}

        if( (_DRAWABLE == _PIXMAP) || (_DRAWABLE == _BWBUFFER) ) 
                                                Xw_restore_window (pwindow) ;
 

#ifdef  TRACE_FLUSH
if( Xw_get_trace() ) {
    printf (" Xw_flush(%lx)\n",(long ) pwindow) ;
}
#endif

        return (XW_SUCCESS);
}
