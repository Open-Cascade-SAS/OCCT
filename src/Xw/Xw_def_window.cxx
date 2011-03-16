#define PURIFY	/*GG_220596 
		Liberation memoire lors de la destruction d'une fenetre*/

#define S3593	/*GG130398
		OPTIMISATION MFT -> Desactiver temporairement le clipping
*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DEF_WINDOW
#define TRACE_CLOSE_WINDOW
#endif

/*
   XW_EXT_WINDOW* Xw_def_window (adisplay,window,istransparent);
	XW_EXT_DISPLAY *adisplay ;
	Aspect_Handle window	XWindow ID
	int istransparent

	Create the Window Extension info
	and save Window attributes

	Returns ERROR if the connection failed.
	Returns extended data window structure pointer 
					if the connection done.


   XW_STATUS Xw_close_window (awindow,destroy):
   XW_EXT_WINDOW *awindow
   int destroy

        Close the window(s) and free all structures attached to
        NOTE than this Close all windows if awindow is NULL
	     Destroy the XWindow if destroy is TRUE;

        returns ERROR  NO Extended Window address has been defined
        returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
void* Xw_def_window (void *adisplay,Aspect_Handle window,int istransparent)
#else
void* Xw_def_window (adisplay,window,istransparent)
void *adisplay ;
Aspect_Handle window ;
int istransparent ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;
XW_EXT_WINDOW *pwindow = NULL ;
XW_WINDOWSTATE state ;
//XWindowAttributes attributes ;
XGCValues gc_values;
int mask,maskext ;
//XEvent event ;
//int i,j,status,xc,yc,width,height ;
int i,xc,yc,width,height ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_def_window",pdisplay) ;
            return (NULL) ;
        }

	if( !(pwindow = Xw_add_window_structure(sizeof(XW_EXT_WINDOW))) ) 
								return (NULL) ;

	_DRAWABLE = _WINDOW = window ;
	_PIXMAP = 0 ;
	_NWBUFFER = 0 ;
	_FWBUFFER = 0 ;
	_BWBUFFER = 0 ;
	_COLORMAP = NULL ;
	_WIDTHMAP = NULL ;
	_TYPEMAP = NULL ;
	_FONTMAP = NULL ;
	_TILEMAP = NULL ;
	_MARKMAP = NULL ;

	pwindow->connexion = pdisplay ;

	state = Xw_get_window_position(pwindow,&xc,&yc,&width,&height) ; 

	if( state == XW_WS_UNKNOWN ) {
	/*ERROR*Bad Window Attributes*/
	    Xw_del_window_structure(pwindow) ;
	    Xw_set_error(54,"Xw_def_window",&window) ;
	    return (NULL) ;
	}

	pwindow->axleft = xc - width/2 ;
	pwindow->axright = xc + width/2 ;
	pwindow->aytop = yc - height/2 ;
	pwindow->aybottom = yc + height/2 ;
	pwindow->rwidth = 0 ;
	pwindow->rheight = 0 ;

	pwindow->lineindex = 0 ;
	pwindow->polyindex = 0 ;
	pwindow->textindex = 0 ;
	pwindow->markindex = 0 ;

	if( istransparent ) {
	  pwindow->backindex = -1 ;
	} else {
	  pwindow->backindex = 0 ;
	}

	pwindow->clipflag = True;

	for( i=0 ; i<MAXEVENT ; i++ ) {
	    pwindow->ecbk[i].function = NULL ;
	    pwindow->ecbk[i].userdata = NULL ;
	    pwindow->icbk[i].function = NULL ;
	}

        gc_values.function = GXcopy ; mask = GCFunction ;
        gc_values.plane_mask = AllPlanes ; mask |= GCPlaneMask ;
        gc_values.background = BlackPixelOfScreen(_SCREEN) ;
						mask |= GCBackground ;
        gc_values.foreground = WhitePixelOfScreen(_SCREEN) ; 
						mask |= GCForeground ;
        pwindow->qgwind.gc = XCreateGC(_DISPLAY,_WINDOW, mask, &gc_values) ;
        gc_values.foreground = BlackPixelOfScreen(_SCREEN) ;
        pwindow->qgwind.gcclear = XCreateGC(_DISPLAY,_WINDOW, mask, &gc_values);
	gc_values.graphics_exposures = False ; 	mask |= GCGraphicsExposures ;
        gc_values.foreground = WhitePixelOfScreen(_SCREEN) ;
        pwindow->qgwind.gccopy = XCreateGC(_DISPLAY,_WINDOW, mask, &gc_values) ;
	gc_values.function = GXxor ;
	gc_values.graphics_exposures = True ;
        pwindow->qgwind.gchigh = XCreateGC(_DISPLAY,_WINDOW, mask, &gc_values) ;
	gc_values.function = GXcopy ;
        pwindow->qgwind.code = ~0 ;
	pwindow->qgwind.pimage = NULL ;
	pwindow->qgwind.method = Aspect_FM_NONE ;
	pwindow->qgwind.pixmap = 0 ;

	_BINDEX = 0 ;
	for( i=0 ; i<MAXBUFFERS ; i++ ) {
 	    _BUFFER(i).bufferid = 0 ;	    
 	    _BUFFER(i).gcf = NULL ;	    
 	    _BUFFER(i).gcb = NULL ;	    
 	    _BUFFER(i).code = 0 ;
	    _BUFFER(i).isdrawn = False ;
	    _BUFFER(i).isempty = True ;
	    _BUFFER(i).isretain = False ;
	    _BUFFER(i).isupdated = False ;
	    _BUFFER(i).rxmin = _WIDTH ;
	    _BUFFER(i).rymin = _HEIGHT ;
	    _BUFFER(i).rxmax = 0 ;
	    _BUFFER(i).rymax = 0 ;
	    _BUFFER(i).xpivot = 0 ;
	    _BUFFER(i).ypivot = 0 ;
	    _BUFFER(i).dxpivot = 0 ;
	    _BUFFER(i).dypivot = 0 ;
	    _BUFFER(i).xscale = 1. ;
	    _BUFFER(i).yscale = 1. ;
	    _BUFFER(i).dxscale = 1. ;
	    _BUFFER(i).dyscale = 1. ;
	    _BUFFER(i).angle = 0. ;
	    _BUFFER(i).dangle = 0. ;
	    _BUFFER(i).ptextdesc = NULL ;
	    _BUFFER(i).plinedesc = NULL ;
	    _BUFFER(i).ppntlist = NULL ;
	    _BUFFER(i).pseglist = NULL ;
	    _BUFFER(i).plinelist = NULL ;
	    _BUFFER(i).ppolylist = NULL ;
	    _BUFFER(i).plarclist = NULL ;
	    _BUFFER(i).pparclist = NULL ;
	    _BUFFER(i).pltextlist = NULL ;
	    _BUFFER(i).pptextlist = NULL ;
	    _BUFFER(i).plmarklist = NULL ;
	    _BUFFER(i).ppmarklist = NULL ;
	    _BUFFER(i).pimaglist = NULL ;
	}

        for( i=0 ; i<MAXQG ; i++ ) {
            gc_values.line_width = 0 ; maskext = GCLineWidth ;
            gc_values.line_style = LineSolid ; maskext |= GCLineStyle ;
            gc_values.dash_offset = 0 ; maskext |= GCDashOffset ;
            gc_values.join_style = JoinRound ; maskext |= GCJoinStyle ;
            pwindow->qgline[i].code = ~0 ;
            pwindow->qgline[i].count = 0 ;
            pwindow->qgline[i].gc = XCreateGC(_DISPLAY,_WINDOW,
                                                mask | maskext, &gc_values) ;
            pwindow->qgmark[i].code = ~0;
            pwindow->qgmark[i].count = 0 ;
            pwindow->qgmark[i].gc = XCreateGC(_DISPLAY,_WINDOW,
                                                mask | maskext, &gc_values) ;
            gc_values.fill_style = FillSolid ; maskext = GCFillStyle ;
            gc_values.fill_rule = EvenOddRule ; maskext |= GCFillRule ;
            pwindow->qgpoly[i].code = ~0 ;
            pwindow->qgpoly[i].count = 0 ;
            pwindow->qgpoly[i].gc = XCreateGC(_DISPLAY,_WINDOW,
                                                mask | maskext, &gc_values) ;
            maskext = 0 ;
            pwindow->qgtext[i].code = ~0;
            pwindow->qgtext[i].count = 0 ;
            pwindow->qgtext[i].gc = XCreateGC(_DISPLAY,_WINDOW,
                                                mask | maskext, &gc_values) ;
        }

	XFlush(_DISPLAY) ;

	Xw_set_window_ratio(pwindow,0.001 METER) ;

#ifdef TRACE_DEF_WINDOW
if( Xw_get_trace() ) {
    printf (" Xw_def_window(%lx,%lx,%d)\n",(long ) pdisplay,(long ) window,istransparent) ;
}
#endif

	return (pwindow);

}

static XW_EXT_WINDOW *PwindowList =NULL ;

#ifdef XW_PROTOTYPE
XW_EXT_WINDOW* Xw_add_window_structure(int size)
#else
XW_EXT_WINDOW* Xw_add_window_structure(size)
int size ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert one Extended window structure in the
        EXtended Window List

        returns Extended window address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) Xw_malloc(size) ;

        if( pwindow ) {
	    pwindow->type = WINDOW_TYPE ;
            pwindow->link = PwindowList ;
	    pwindow->other = NULL ;
            PwindowList = pwindow ;
        } else {
	    /*ERROR*EXT_WINDOW allocation failed*/
	    Xw_set_error(69,"Xw_add_window_structure",0) ;
        }

        return (pwindow) ;
}

#ifdef XW_PROTOTYPE
XW_EXT_WINDOW* Xw_get_window_structure(Window window)
#else
XW_EXT_WINDOW* Xw_get_window_structure(window)
Window window ;
#endif /*XW_PROTOTYPE*/
/*
        Return the Extended window address corresponding to the required
        Window Id or NULL if not found
*/
{
XW_EXT_WINDOW *pwindow = PwindowList ;

        while (pwindow) {
            if( pwindow->window == window ) {
                break ;
            }
            pwindow = (XW_EXT_WINDOW*) pwindow->link ;
        }

        return (pwindow) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_window (void* awindow,int destroy)
#else
XW_STATUS Xw_close_window (awindow,destroy)
void *awindow;
int destroy;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
int i ;

        if( !pwindow ) {
            while( Xw_del_window_structure(NULL) ) ;
        } else {
	    if( destroy ) {
#ifdef PURIFY
              XFreeGC(_DISPLAY,pwindow->qgwind.gc);
              XFreeGC(_DISPLAY,pwindow->qgwind.gcclear);
              XFreeGC(_DISPLAY,pwindow->qgwind.gccopy);
              XFreeGC(_DISPLAY,pwindow->qgwind.gchigh);

              for( i=0 ; i<MAXQG ; i++ ) {
                XFreeGC(_DISPLAY,pwindow->qgline[i].gc);
                XFreeGC(_DISPLAY,pwindow->qgmark[i].gc);
                XFreeGC(_DISPLAY,pwindow->qgpoly[i].gc);
                XFreeGC(_DISPLAY,pwindow->qgtext[i].gc);
              }
#endif
	      XDestroyWindow(_DISPLAY,_WINDOW) ;
	    }
            Xw_del_window_structure(pwindow) ;
        }

#ifdef  TRACE_CLOSE_WINDOW
if( Xw_get_trace() ) {
    printf (" Xw_close_window(%lx,%d)\n",(long ) pwindow,destroy) ;
}
#endif

        return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_window_structure(XW_EXT_WINDOW* pwindow)
#else
XW_STATUS Xw_del_window_structure(pwindow)
XW_EXT_WINDOW *pwindow;
#endif /*XW_PROTOTYPE*/
/*
        Remove the Extended window address or the next from the Extended List 
	and Free the Extended Window

        returns ERROR if the window address is not Found in the list
		      or NO more Window exist
        returns SUCCESS if successful
*/
{
XW_EXT_WINDOW *qwindow = PwindowList ;
int i ;

        if( !qwindow ) return (XW_ERROR) ;

        if( pwindow == qwindow ) {
            PwindowList = (XW_EXT_WINDOW*) pwindow->link ;
        } else if( !pwindow ) {
            if( !PwindowList ) return (XW_ERROR) ;
	    pwindow = PwindowList ;
            PwindowList = (XW_EXT_WINDOW*) PwindowList->link ;
        } else {
            for( ; qwindow ; qwindow = (XW_EXT_WINDOW*) qwindow->link ) {
                if( qwindow->link == pwindow ) {
                    qwindow->link = pwindow->link ;
                    break ;
                }
            }
        }

	if( _PIXMAP ) {
	    Xw_close_pixmap(pwindow) ;
	}

	if( _BIMAGE ) {
	    Xw_close_image(_BIMAGE) ;
	    _BIMAGE = NULL;
	}

        if( _BPIXMAP ) {
	    Xw_close_background_pixmap(pwindow) ;
	    _BPIXMAP = 0;
	}

        Xw_close_buffer(pwindow,0) ;

        for( i=1 ; i<MAXBUFFERS ; i++ ) {
	    Xw_close_buffer(pwindow,-i) ;
        }

	if( pwindow->other ) Xw_free(pwindow->other) ;

	if( _COLORMAP && (_COLORMAP->maxwindow > 0) ) {
	  _COLORMAP->maxwindow -= 1;
	}

	Xw_free(pwindow) ;

        return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_reset_attrib(XW_EXT_WINDOW* awindow)
#else
XW_STATUS Xw_reset_attrib(awindow)
XW_EXT_WINDOW *awindow ;
#endif /*XW_PROTOTYPE*/
/*
	Reset ALL Window attributes of a particular Window or ALL Windows
	if NULL
*/
{
XW_EXT_WINDOW *pwindow ;
unsigned long highpixel,backpixel ;
int i ;

	if( !(pwindow = PwindowList) ) return (XW_ERROR) ;

        while (pwindow) {
            if( !awindow || (pwindow == awindow) ) {
    	      for( i=0 ; i<MAXQG ; i++ ) {
                pwindow->qgline[i].code = ~0 ;
                pwindow->qgpoly[i].code = ~0 ;
                pwindow->qgtext[i].code = ~0 ;
                pwindow->qgmark[i].code = ~0 ;
    	      }
  	      if( _COLORMAP ) {
                highpixel = _COLORMAP->highpixel ;
	        if( pwindow->backindex < 0 ) {
	          backpixel = _COLORMAP->backpixel ;
		} else {
	          backpixel = _COLORMAP->pixels[pwindow->backindex] ;
		}
                XSetForeground(_DISPLAY, pwindow->qgwind.gccopy, highpixel) ;
        	highpixel ^= backpixel ; 
        	XSetForeground(_DISPLAY, pwindow->qgwind.gchigh, highpixel) ;
	      }
            }
            pwindow = (XW_EXT_WINDOW*) pwindow->link ;
        }

        return (XW_SUCCESS) ;
}
       
#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_color_attrib(XW_EXT_WINDOW* awindow,
		XW_DRAWMODE mode,int color,unsigned long* hcolor,
		int* function,unsigned long* planemask)
#else
XW_STATUS Xw_get_color_attrib(awindow,mode,color,hcolor,function,planemask)
XW_EXT_WINDOW *awindow ;
XW_DRAWMODE mode ;
int color ;
int *function ;
unsigned long *planemask ;
unsigned long *hcolor ;
#endif /*XW_PROTOTYPE*/
/*
        Returns the best drawing attributs color,function and planemask
        depending of write mode and colormap visual class attached to this
        window .

        returns ERROR if the Visual class is not authorized
        returns SUCCESS if successfull
*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;

    switch ( _CLASS ) {

        case TrueColor :

            switch (mode) {
            case XW_ERASE :
		if( pwindow->backindex >= 0 )
                	*hcolor = _COLORMAP->pixels[pwindow->backindex] ;
		else 	*hcolor = _COLORMAP->backpixel;
                *function = GXcopy ;
                *planemask = AllPlanes ;
                break ;
            case XW_REPLACE :
                *hcolor = _COLORMAP->pixels[color] ;
                *function = GXcopy ;
                *planemask = AllPlanes ;
                break ;
            case XW_XORBACK :
		if( pwindow->backindex >= 0 )
                  *hcolor = _COLORMAP->pixels[pwindow->backindex]
						 ^ _COLORMAP->pixels[color] ;
		else *hcolor = _COLORMAP->backpixel
						 ^ _COLORMAP->pixels[color] ;
                *function = GXxor ;
                *planemask = AllPlanes ;
                break ;
            case XW_XORLIGHT :
            case XW_HIGHLIGHT :
            case XW_UNHIGHLIGHT :
                *hcolor = _COLORMAP->highpixel ^ _COLORMAP->pixels[color] ;
                *function = GXxor ;
                *planemask = AllPlanes ;
                break ;
            }
            break ;

        case PseudoColor :

            switch (mode) {
            case XW_ERASE :
		if( pwindow->backindex >= 0 )
                	*hcolor = _COLORMAP->pixels[pwindow->backindex] ;
		else 	*hcolor = _COLORMAP->backpixel;
                *function = GXcopy ;
                *planemask = AllPlanes ;
                break ;
            case XW_REPLACE :
                *hcolor = _COLORMAP->pixels[color] ;
                *function = GXcopy ;
                *planemask = AllPlanes ;
                break ;
            case XW_XORBACK :
		if( pwindow->backindex >= 0 )
                  *hcolor = _COLORMAP->pixels[pwindow->backindex]
						 ^ _COLORMAP->pixels[color] ;
		else *hcolor = _COLORMAP->backpixel
						 ^ _COLORMAP->pixels[color] ;
                *function = GXxor ;
                *planemask = AllPlanes ;
                break ;
            case XW_XORLIGHT :
            case XW_HIGHLIGHT :
            case XW_UNHIGHLIGHT :
                *hcolor = _COLORMAP->highpixel ^ _COLORMAP->pixels[color] ;
                *function = GXxor ;
                *planemask = AllPlanes ;
                break ;
            }
            break ;

        case StaticColor :

            switch (mode) {
            case XW_ERASE :
		if( pwindow->backindex >= 0 )
                	*hcolor = _COLORMAP->pixels[pwindow->backindex] ;
		else 	*hcolor = _COLORMAP->backpixel;
                *function = GXcopy ;
                *planemask = AllPlanes ;
                break ;
            case XW_REPLACE :
                *hcolor = _COLORMAP->pixels[color] ;
                *function = GXcopy ;
                *planemask = AllPlanes ;
                break ;
            case XW_XORBACK :
		if( pwindow->backindex >= 0 )
                  *hcolor = _COLORMAP->pixels[pwindow->backindex]
						 ^ _COLORMAP->pixels[color] ;
		else *hcolor = _COLORMAP->backpixel
						 ^ _COLORMAP->pixels[color] ;
                *function = GXxor ;
                *planemask = AllPlanes ;
                break ;
            case XW_XORLIGHT :
            case XW_HIGHLIGHT :
            case XW_UNHIGHLIGHT :
                *hcolor = _COLORMAP->highpixel ^ _COLORMAP->pixels[color] ;
                *function = GXxor ;
                *planemask = AllPlanes ;
                break ;
            }
            break ;

        case StaticGray :
        case GrayScale :
        case DirectColor :
            /*ERROR*Unimplemented Visual class*/
            Xw_set_error(5,"Xw_get_color_attrib",&_CLASS) ;
            return (XW_ERROR) ;
    }

    return (XW_SUCCESS) ;
}

#ifdef S3593
#ifdef XW_PROTOTYPE
void Xw_set_clipping(void* awindow, int flag)
#else
void Xw_set_clipping(awindow,flag)
void* awindow;
int flag;
#endif
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) awindow;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_set_clipping",pwindow) ;
        return;
    }

    pwindow->clipflag = flag;
}

#ifdef XW_PROTOTYPE
int Xw_get_clipping(void* awindow)
#else
int Xw_get_clipping(awindow)
void* awindow;
#endif
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) awindow;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_get_clipping",pwindow) ;
        return (True) ;
    }

    return pwindow->clipflag;
}
#endif	/*S3593*/
