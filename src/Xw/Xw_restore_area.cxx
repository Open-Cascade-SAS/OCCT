
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_RESTORE_AREA
#endif

/*
   STATUS Xw_restore_area (awindow,px,py,pwidth,pheight):
   XW_EXT_WINDOW *awindow
   int px,py			area center given in PIXELS
   int pwidth,pheight     	area size given in PIXELS 

	Redraw the window area from the attached pixmap   

	returns XW_ERROR if NO attached pixmap exist or
			 area is out of window
	returns XW_SUCCESS if successful
*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_restore_area (void *awindow,
			int px,int py,int pwidth,int pheight)
#else
XW_STATUS Xw_restore_area (awindow,px,py,pwidth,pheight)
void *awindow;
int px,py,pwidth,pheight ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
//XGCValues gc_values ;
//int function,color,mask = 0 ;
//unsigned long planemask,hcolor ;


        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_restore_area",pwindow) ;
            return (XW_ERROR) ;
        }

        if( (px + pwidth/2) < 0 || (px - pwidth/2) > _WIDTH ||
            (py + pheight/2) < 0 || (py - pheight/2)  > _HEIGHT ) {
	    /*WARNING*Bad AREA position*/
	    Xw_set_error(68,"Xw_restore_area",0) ;
	    return (XW_ERROR) ;
	}

	px -= pwidth/2 ; py -= pheight/2 ;
	XFlush(_DISPLAY) ;
	if( _NWBUFFER > 0 ) {
            XCopyArea(_DISPLAY,_BWBUFFER,_FWBUFFER,pwindow->qgwind.gccopy,
                    	px,py,pwidth,pheight,px,py) ;
        } else if( _PIXMAP ) {
            XCopyArea(_DISPLAY,_PIXMAP,_WINDOW,pwindow->qgwind.gccopy,
                    	px,py,pwidth,pheight,px,py) ;
	} else return (XW_ERROR) ;

	XFlush(_DISPLAY) ;

#ifdef  TRACE_RESTORE_AREA
if( Xw_get_trace() ) {
	printf (" Xw_restore_area(%lx,%d,%d,%d,%d)\n",
				(long ) pwindow,px,py,pwidth,pheight) ;
}
#endif

	return (XW_SUCCESS);
}
