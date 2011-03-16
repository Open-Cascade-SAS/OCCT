
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ERASE_WINDOW
#endif

/*
   STATUS Xw_erase_window (awindow)
   XW_EXT_WINDOW *awindow

	Erase the entire drawable.

	returns SUCCESS always

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_erase_window (void* awindow)
#else
XW_STATUS Xw_erase_window (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
int i ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_erase_window",pwindow) ;
            return (XW_ERROR) ;
        }

	if( _BPIXMAP ) {
            XCopyArea(_DISPLAY,_BPIXMAP,_DRAWABLE,pwindow->qgwind.gccopy,
                                                0,0,_WIDTH,_HEIGHT,0,0) ;
	} else {
	    if( _DRAWABLE == _WINDOW ) { 
		XClearWindow(_DISPLAY,_WINDOW) ;
	    } else {
	        XFillRectangle(_DISPLAY,_DRAWABLE,pwindow->qgwind.gcclear,
						0,0,_WIDTH,_HEIGHT) ;
	    }
	}

	for( i=0 ; i<MAXBUFFERS ; i++ ) {
	  _BUFFER(i).isdrawn = False ;
	}

	XFlush(_DISPLAY) ;

#ifdef  TRACE_ERASE_WINDOW
if( Xw_get_trace() > 1 ) {
    printf(" Xw_erase_window(%lx)\n",(long ) pwindow) ;
}
#endif

	return (XW_SUCCESS);
}
