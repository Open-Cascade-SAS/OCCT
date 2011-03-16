#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Xw_Extension.h>

#ifdef HAVE_X11_EXTENSIONS_MULTIBUF_H
# include <X11/extensions/multibuf.h>
#endif

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_OPEN_PIXMAP
#define TRACE_OPEN_BACKGROUND_PIXMAP
#define TRACE_CLOSE_PIXMAP
#define TRACE_CLOSE_BACKGROUND_PIXMAP
#endif

/*
   XW_STATUS Xw_open_pixmap (awindow)
   XW_EXT_WINDOW *awindow

	Open a DoubleBuffer PIXMAP depending of the Window size

	returns ERROR if NO More PIXMAP is available on this Window
	returns SUCCESS if successful

   XW_STATUS Xw_open_background_pixmap (awindow)
   XW_EXT_WINDOW *awindow

	Open a Background PIXMAP depending of the Window size

	returns ERROR if NO More PIXMAP is available on this Window
	returns SUCCESS if successful

   XW_STATUS Xw_close_pixmap (awindow)
   XW_EXT_WINDOW *awindow

	Close the DoubleBuffer PIXMAP attached to this Window

	returns ERROR if NO More PIXMAP exist
	returns SUCCESS if successful

   XW_STATUS Xw_close_background_pixmap (awindow)
   XW_EXT_WINDOW *awindow

	Close the Background PIXMAP attached to this Window

	returns ERROR if NO More PIXMAP exist
	returns SUCCESS if successful
*/

static char SetDoubleBuffer = '\0' ;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_open_pixmap (void *awindow)
#else
XW_STATUS Xw_open_pixmap (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_DISPLAY *pdisplay = pwindow->connexion;
int error,gravity ;
char svalue[4] ;
XW_STATUS status = XW_ERROR;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_open_pixmap",pwindow) ;
        return (XW_ERROR) ;
    }

    if( SetDoubleBuffer == '\0' ) {
        SetDoubleBuffer = 'U' ;
        if( Xw_get_env("Xw_SET_DOUBLE_BUFFER",svalue,sizeof(svalue)) ) {
            if( strlen(svalue) ) {
                SetDoubleBuffer = svalue[0] ;
            }
            printf(" Xw_SET_DOUBLE_BUFFER is '%c'\n",SetDoubleBuffer) ;
        }
    }

    if( (SetDoubleBuffer == 'B') && 
		(pdisplay->server == XW_SERVER_IS_SUN) && (_DEPTH != 24) ) {
	SetDoubleBuffer = 'P';	/* BUG sur SUN */
        /*WARNING*MultiBufferingExtension is not accessible here*/
        Xw_set_error(126,"Xw_open_pixmap",pwindow) ;
    }

#ifndef HAVE_X11_EXTENSIONS_MULTIBUF_H
    if( SetDoubleBuffer == 'B' ) {
	SetDoubleBuffer = 'P';	/* MULTIBUF unavailable under SGI IRIX 6.5 */
        /*WARNING*MultiBufferingExtension is not accessible here*/
        Xw_set_error(126,"Xw_open_pixmap",pwindow) ;
    }
#else
    if( SetDoubleBuffer == 'B' ) {
      if( _NWBUFFER == 0 ) {
        int mOptCode,fEvent,fError,maxmono,maxstereo ;
	XmbufBufferInfo *pmono,*pstereo ;
        if( XQueryExtension(_DISPLAY,"Multi-Buffering",
				&mOptCode,&fEvent,&fError) && 
            	XmbufGetScreenInfo(_DISPLAY,_WINDOW,
				&maxmono,&pmono,&maxstereo,&pstereo) ) {
          _NWBUFFER = XmbufCreateBuffers(_DISPLAY,_WINDOW,2,
                                        MultibufferUpdateActionUntouched,
                                        MultibufferUpdateHintFrequent,
                                        &_FWBUFFER) ;
	} 
	if( _NWBUFFER == 2 ) {
          XFillRectangle(_DISPLAY,_BWBUFFER,pwindow->qgwind.gcclear,
                                                0,0,_WIDTH,_HEIGHT) ;
	} else {
	  if( _NWBUFFER > 0 ) XmbufDestroyBuffers(_DISPLAY,_WINDOW) ;
          _NWBUFFER = 0 ;
          _BWBUFFER = 0 ;
          _FWBUFFER = 0 ;
	}
	SetDoubleBuffer = 'P';
        /*WARNING*MultiBufferingExtension is not accessible here*/
        Xw_set_error(126,"Xw_open_pixmap",pwindow) ;
      }
    }
#endif
 
    if( (SetDoubleBuffer == 'U') || 
		(SetDoubleBuffer == 'Y') || 
			(SetDoubleBuffer == 'P') ) { 
      if( !_PIXMAP ) {
	Xw_print_error();
        if( !Xw_get_trace() ) Xw_set_synchronize(_DISPLAY,True) ;
        _PIXMAP = XCreatePixmap(_DISPLAY,_WINDOW,_WIDTH,_HEIGHT,_DEPTH) ;
        if( !Xw_get_trace() ) Xw_set_synchronize(_DISPLAY,False) ;

        Xw_get_error(&error,&gravity);
        if( _PIXMAP && (error < 1000) ) {
          XFillRectangle(_DISPLAY,_PIXMAP,pwindow->qgwind.gcclear,
                                                0,0,_WIDTH,_HEIGHT) ;
        } else {
	  /*ERROR*DISABLE Double Buffer*/
	  _PIXMAP = 0 ;
	  Xw_set_error(71,"Xw_open_pixmap",0) ;
        }
      }
    } 

    if( _PIXMAP || _NWBUFFER ) status = XW_SUCCESS;

#ifdef TRACE_OPEN_PIXMAP
if( Xw_get_trace() ) {
    printf(" %d = Xw_open_pixmap(%lx)\n",status,(long ) pwindow) ;
}
#endif
	
    return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_pixmap (void *awindow)
#else
XW_STATUS Xw_close_pixmap (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_open_pixmap",pwindow) ;
        return (XW_ERROR) ;
    }

#ifdef HAVE_X11_EXTENSIONS_MULTIBUF_H 
    if( _NWBUFFER > 0 ) XmbufDestroyBuffers(_DISPLAY,_WINDOW) ;
#endif
    if( _PIXMAP ) XFreePixmap(_DISPLAY,_PIXMAP) ;
    XFlush(_DISPLAY) ;
    _PIXMAP = 0 ;
    _NWBUFFER = 0 ;
    _BWBUFFER = 0 ;
    _FWBUFFER = 0 ;
    _DRAWABLE = _WINDOW ;
 
#ifdef TRACE_CLOSE_PIXMAP
if( Xw_get_trace() ) {
    printf(" Xw_close_pixmap(%lx)\n",(long ) pwindow) ;
}
#endif

    return XW_SUCCESS ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_open_background_pixmap (void *awindow)
#else
XW_STATUS Xw_open_background_pixmap (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
int error,gravity ;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_open_background_pixmap",pwindow) ;
        return (XW_ERROR) ;
    }

    if( _BPIXMAP ) Xw_close_background_pixmap(awindow) ; 

    Xw_print_error();
    if( !Xw_get_trace() ) Xw_set_synchronize(_DISPLAY,True) ;
    _BPIXMAP = XCreatePixmap(_DISPLAY,_WINDOW,_WIDTH,_HEIGHT,_DEPTH) ;
    if( !Xw_get_trace() ) Xw_set_synchronize(_DISPLAY,False) ;

    Xw_get_error(&error,&gravity);
    if( !_BPIXMAP || (error >= 1000) ) {
	/*ERROR*DISABLE Double Buffer*/
	_BPIXMAP = 0 ;
	Xw_set_error(71,"Xw_open_background_pixmap",0) ;
	return (XW_ERROR) ;
    }

    XFillRectangle(_DISPLAY,_BPIXMAP,pwindow->qgwind.gcclear,0,0,_WIDTH,_HEIGHT);

#ifdef TRACE_OPEN_BACKGROUND_PIXMAP
if( Xw_get_trace() ) {
    printf(" Xw_open_background_pixmap(%lx)\n",(long ) pwindow) ;
}
#endif
	
    return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_background_pixmap (void *awindow)
#else
XW_STATUS Xw_close_background_pixmap (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_close_background_pixmap",pwindow) ;
        return (XW_ERROR) ;
    }

    if( _BPIXMAP ) {
        XFreePixmap(_DISPLAY,_BPIXMAP) ;
        XFlush(_DISPLAY) ;
        _BPIXMAP = 0 ;
    } else {
        return (XW_ERROR) ;
    }
 
#ifdef TRACE_CLOSE_BACKGROUND_PIXMAP
if( Xw_get_trace() ) {
    printf(" Xw_close_background_pixmap(%lx)\n",(long ) pwindow) ;
}
#endif
return (XW_SUCCESS) ;
}
