
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_SOFT_CURSOR
#endif

/*
   XW_STATUS Xw_set_soft_cursor (awindow,type,button)
	XW_EXT_WINDOW 	*awindow	
	XW_CURSORTYPE	type	
	int		button		Mouse Button number (1 to 3)

	Associate the soft cursor to the window and a mouse button
	Soft cursor is activated when Mouse Button is press
		    drawn when mouse move with button press
		    and Unactivated when Mouse Button is Release

	Returns XW_ERROR if button is out of range
	Returns XW_SUCCESS if successful

*/

static int Start = False ;
static XPoint points[100] ;

#ifdef XW_PROTOTYPE
static XW_STATUS Xw_rubberline_cursor(XW_EVENT *event) ;
static XW_STATUS Xw_rubberband_cursor(XW_EVENT *event) ;
static XW_STATUS Xw_userdefined_cursor(XW_EVENT *event) ;
static void DrawLines(XW_EXT_WINDOW *pwindow,XPoint *points,int npoint) ;
XW_STATUS Xw_set_soft_cursor (void *awindow,XW_CURSORTYPE type,int button)
#else
static XW_STATUS Xw_rubberline_cursor() ;
static XW_STATUS Xw_rubberband_cursor() ;
static XW_STATUS Xw_userdefined_cursor() ;
static void DrawLines() ;
XW_STATUS Xw_set_soft_cursor (awindow,type,button)
void *awindow ;
XW_CURSORTYPE type;
int button ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_STATUS status ;
XW_EVENTTYPE mask1=(XW_EVENTTYPE ) 0 ,mask2=(XW_EVENTTYPE ) 0 ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_set_soft_cursor",pwindow) ;
            return (XW_ERROR) ;
        }

	if( button <= 0 || button > 3 ) {
	    /*WARNING*Bad Mouse button number*/
	    Xw_set_error(70,"Xw_set_soft_cursor",&button) ;
	    return (XW_ERROR) ;
	}

	if( button == 1 ) {
	    mask1 = XW_MOUSEBUTTON1 ; mask2 = XW_MOUSEMOVEWITHBUTTON1 ;
	} else if( button == 2 ) {
	    mask1 = XW_MOUSEBUTTON2 ; mask2 = XW_MOUSEMOVEWITHBUTTON2 ;
	} else if( button == 3 ) {
	    mask1 = XW_MOUSEBUTTON3 ; mask2 = XW_MOUSEMOVEWITHBUTTON3 ;
	}

	switch (type) {

	    case XW_WITHOUT_CURSOR :
		status = Xw_set_internal_event(pwindow,mask1,NULL) ;
		status = Xw_set_internal_event(pwindow,mask2,NULL) ;
		break ;
	    case XW_RUBBERLINE_CURSOR :
		status = Xw_set_internal_event(pwindow,mask1,
							Xw_rubberline_cursor) ;
		status = Xw_set_internal_event(pwindow,mask2,
							Xw_rubberline_cursor) ;
		break ;
	    case XW_RUBBERBAND_CURSOR :
		status = Xw_set_internal_event(pwindow,mask1,
							Xw_rubberband_cursor) ;
		status = Xw_set_internal_event(pwindow,mask2,
							Xw_rubberband_cursor) ;
		break ;
	    case XW_USERDEFINED_CURSOR :
		status = Xw_set_internal_event(pwindow,mask1,
							Xw_userdefined_cursor) ;
		status = Xw_set_internal_event(pwindow,mask2,
							Xw_userdefined_cursor) ;
		break ;
#ifndef DEB
	default:
		break ;
#endif
   	}


#ifdef TRACE_SET_SOFT_CURSOR
if( Xw_get_trace() ) {
	printf (" Xw_set_soft_cursor(%lx,%d,%d)\n",(long ) pwindow,type,button) ;
}
#endif
	return (XW_SUCCESS);

}

#ifdef XW_PROTOTYPE
static XW_STATUS Xw_rubberline_cursor(XW_EVENT *event) 
#else
static XW_STATUS Xw_rubberline_cursor(event) 
XW_EVENT *event ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) event->any.awindow ;

#ifdef TRACE_SET_SOFT_CURSOR
if( Xw_get_trace() > 2 ) {
	printf (" Xw_rubberline_cursor(%lx,%d)\n",
				(long ) event->any.awindow,event->any.type) ;
}
#endif

	switch (event->any.type) {

	    case XW_MOUSEBUTTON1 :
	    case XW_MOUSEBUTTON2 :
	    case XW_MOUSEBUTTON3 :
		if( Start ) {
		    Start = False ;
		    DrawLines(pwindow,points,2) ;
		} else {
		    points[0].x = event->mousebutton.x ;
		    points[0].y = event->mousebutton.y ;
		}
		break ;

	    case XW_MOUSEMOVEWITHBUTTON1 :
	    case XW_MOUSEMOVEWITHBUTTON2 :
	    case XW_MOUSEMOVEWITHBUTTON3 :
		if( Start ) {
		    DrawLines(pwindow,points,2) ;
		}
		Start = True ;
		points[1].x = event->mousemovewithbutton.x - points[0].x ;
		points[1].y = event->mousemovewithbutton.y - points[0].y ;
		DrawLines(pwindow,points,2) ;
		XFlush(_DISPLAY) ;
		break ;
#ifndef DEB
	default:
		break ;
#endif
	}

	return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
static XW_STATUS Xw_rubberband_cursor(XW_EVENT *event) 
#else
static XW_STATUS Xw_rubberband_cursor(event) 
XW_EVENT *event ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) event->any.awindow ;

#ifdef TRACE_SET_SOFT_CURSOR
if( Xw_get_trace() > 2 ) {
	printf (" Xw_rubberband_cursor(%lx,%d)\n",
				(long ) event->any.awindow,event->any.type) ;
}
#endif

	switch (event->any.type) {

	    case XW_MOUSEBUTTON1 :
	    case XW_MOUSEBUTTON2 :
	    case XW_MOUSEBUTTON3 :
		if( Start ) {
		    Start = False ;
		    DrawLines(pwindow,points,5) ;
		} else {
		    points[0].x = event->mousebutton.x ;
		    points[0].y = event->mousebutton.y ;
		}
		break ;

	    case XW_MOUSEMOVEWITHBUTTON1 :
	    case XW_MOUSEMOVEWITHBUTTON2 :
	    case XW_MOUSEMOVEWITHBUTTON3 :
		if( Start ) {
		    DrawLines(pwindow,points,5) ;
		}
		Start = True ;
		points[1].x = 0 ;
		points[1].y = event->mousemovewithbutton.y - points[0].y ;
		points[2].x = event->mousemovewithbutton.x - points[0].x ;
		points[2].y = 0 ;
		points[3].x = 0 ;
		points[3].y = -points[1].y ;
		points[4].x = -points[2].x ;
		points[4].y = 0 ;
		DrawLines(pwindow,points,5) ;
		XFlush(_DISPLAY) ;
		break ;
#ifndef DEB
	default:
		break ;
#endif
	}

	return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
static XW_STATUS Xw_userdefined_cursor(XW_EVENT *event) 
#else
static XW_STATUS Xw_userdefined_cursor(event) 
XW_EVENT *event ;
#endif /*XW_PROTOTYPE*/
{
#ifdef TRACE_SET_SOFT_CURSOR
if( Xw_get_trace() > 2 ) {
	printf (" Xw_userdefined_cursor(%lx,%d)\n",
				(long ) event->any.awindow,event->any.type) ;
}
#endif
	Xw_set_error(97,"Xw_userdefined_cursor",&event->any.type) ;

	return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
static void DrawLines(XW_EXT_WINDOW *pwindow,XPoint *points,int npoint)
#else
static void DrawLines(pwindow,points,npoint)
XW_EXT_WINDOW *pwindow ;
XPoint *points ;
int npoint ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_DISPLAY *pdisplay = pwindow->connexion ;

    if( _DGRAB ) {
	int xs = points[0].x,ys = points[0].y ;

	points[0].x += _X ; points[0].y += _Y ;
        XDrawLines(_DISPLAY,_DROOT,_DGC,points,npoint,CoordModePrevious) ;
	points[0].x = xs ; points[0].y = ys ;
    } else {
        XDrawLines(_DISPLAY,_WINDOW,pwindow->qgwind.gchigh,
					points,npoint,CoordModePrevious) ;
    }
}
