
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_WAIT_EVENT
#endif

/*
   STATUS Xw_wait_event (awindow)
	XW_EXT_WINDOW 	*awindow	

	Wait any event activated by Xw_set_event in any Window
	and CALL the associated call_back routine if defined
	NOTE than if call_back returns ERROR this go outside the WAIT
	event loop .

	Returns ERROR always 

*/

static XW_CALLBACK ecbk ;
static XW_ICALLBACK icbk ;

static char keystring[32] ;
static KeySym modifier = 0 ;

static XW_EVENT xwevent ;

static Window LastEventWindow = 0 ;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_wait_event (void* awindow)
#else
XW_STATUS Xw_wait_event (awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XEvent xevent ;
XW_STATUS status = XW_SUCCESS ;
XW_USERDATA *userdata ;
int lstring,xbutton=0,ybutton=0,bbutton = 0,mbutton = False ; 
KeySym keysym ;
XComposeStatus key_status ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_wait_event",pwindow) ;
            return (XW_ERROR) ;
        }

	while ( status == XW_SUCCESS ) {
	    XNextEvent(_DISPLAY,&xevent) ;

	    xwevent.any.type = XW_ALLEVENT ;
	    if( LastEventWindow != xevent.xany.window ) {
		LastEventWindow = xevent.xany.window ;
	        xwevent.any.awindow = pwindow =  
			Xw_get_window_structure(LastEventWindow) ;
	    }

#ifdef TRACE_WAIT_EVENT
if( Xw_get_trace() > 3 ) {
	printf (" Xw_event(%lx,%d)\n",(long ) pwindow,xevent.type) ;
}
#endif
	    if( pwindow ) {
	      switch (xevent.type) {
		case KeyPress :
                    lstring = XLookupString( &xevent.xkey, 
		      keystring, sizeof(keystring), &keysym , &key_status ) ;
		    if( lstring ) {
		    	xwevent.any.type = XW_KEYBOARD ;
			xwevent.keyboard.modifier = modifier ;
			xwevent.keyboard.keycode = 0 ;
		        xwevent.keyboard.state = True ;
		        xwevent.keyboard.string = &keystring[0] ;
		        xwevent.keyboard.x = xevent.xkey.x ;
		        xwevent.keyboard.y = xevent.xkey.y ;
		    } else {
			if( IsModifierKey(keysym) ) {
			    modifier = keysym ;
			} else {
		    	    xwevent.any.type = XW_KEYBOARD ;
			    xwevent.keyboard.modifier = modifier ;
			    xwevent.keyboard.keycode = keysym ;
		            xwevent.keyboard.state = True ;
		            xwevent.keyboard.string = NULL ;
		            xwevent.keyboard.x = xevent.xkey.x ;
		            xwevent.keyboard.y = xevent.xkey.y ;
			}
		    }
		    break ;
		case KeyRelease :
                    lstring = XLookupString( &xevent.xkey, 
		      keystring, sizeof(keystring), &keysym , &key_status ) ;
		    if( IsModifierKey(keysym) ) {
			modifier = 0 ;
		    }
		    break ;
		case ButtonPress :
		    bbutton = xevent.xbutton.button ;
		    xbutton = xevent.xbutton.x ;
		    ybutton = xevent.xbutton.y ;
		    mbutton = False ;
		    if( xevent.xbutton.button == 1 ) 
					xwevent.any.type = XW_MOUSEBUTTON1 ;
		    else if( xevent.xbutton.button == 2 ) 
					xwevent.any.type = XW_MOUSEBUTTON2 ;
		    else if( xevent.xbutton.button == 3 ) 
					xwevent.any.type = XW_MOUSEBUTTON3 ;
		    xwevent.mousebutton.button = bbutton ;
		    xwevent.mousebutton.state = True ;
		    xwevent.mousebutton.modifier = modifier ;
		    xwevent.mousebutton.x = xbutton ;
		    xwevent.mousebutton.y = ybutton ;
		    break ;
		case ButtonRelease :
		    if( mbutton ) {
		        if( xevent.xbutton.button == 1 ) 
					xwevent.any.type = XW_MOUSEBUTTON1 ;
		        else if( xevent.xbutton.button == 2 ) 
					xwevent.any.type = XW_MOUSEBUTTON2 ;
		        else if( xevent.xbutton.button == 3 ) 
					xwevent.any.type = XW_MOUSEBUTTON3 ;
		        xwevent.mousebutton.button = bbutton ;
		        xwevent.mousebutton.state = False ;
		        xwevent.mousebutton.modifier = modifier ;
		        xwevent.mousebutton.x = xevent.xbutton.x ;
		        xwevent.mousebutton.y = xevent.xbutton.y ;
		    }
		    bbutton = 0 ;
		    mbutton = False ;
		    break ;
		case MotionNotify :
		    if( bbutton ) {
			int dx,dy ;

		        while( XCheckTypedWindowEvent(_DISPLAY,LastEventWindow,
					MotionNotify,&xevent) ) ;
			if( !mbutton ) {
			    dx = xevent.xmotion.x - xbutton ;
		    	    dy = xevent.xmotion.y - ybutton ;
		            if( (abs(dx) + abs(dy)) > 6 ) {
				mbutton = True ;
			    }
			} 

			if( mbutton ) {
		    	    if( bbutton == 1 ) 
				xwevent.any.type = XW_MOUSEMOVEWITHBUTTON1 ;
		    	    else if( bbutton == 2 ) 
				xwevent.any.type = XW_MOUSEMOVEWITHBUTTON2 ;
		    	    else if( bbutton == 3 ) 
				xwevent.any.type = XW_MOUSEMOVEWITHBUTTON3 ;
		            xwevent.mousemovewithbutton.button = bbutton ;
		            xwevent.mousemovewithbutton.state = True ;
		    	    xwevent.mousemovewithbutton.modifier = modifier ;
		    	    xwevent.mousemovewithbutton.x = xevent.xmotion.x;
		    	    xwevent.mousemovewithbutton.y = xevent.xmotion.y;
			}
		    } else {
		        while( XCheckTypedWindowEvent(_DISPLAY,LastEventWindow,
					MotionNotify,&xevent) ) ;
		        xwevent.any.type = XW_MOUSEMOVE ;
		    	xwevent.mousemove.modifier = modifier ;
		    	xwevent.mousemove.x = xevent.xmotion.x ;
		    	xwevent.mousemove.y = xevent.xmotion.y ;
		    }
		    break ;
		case EnterNotify  :             
		    xwevent.any.type = XW_ENTERWINDOW ;
		    break ;
		case Expose       :             
		    if( !xevent.xexpose.count ) {
			int x1 = xevent.xexpose.x ;
			int y1 = xevent.xexpose.y ;
			int x2 = x1 + xevent.xexpose.width ;
			int y2 = y1 + xevent.xexpose.height ;

		        xwevent.any.type = XW_EXPOSEWINDOW ;
			xwevent.windowexpose.xc = (x1+x2)/2 ;
			xwevent.windowexpose.yc = (y1+y2)/2 ;
			xwevent.windowexpose.width = (x2-x1) ;
			xwevent.windowexpose.height = (y2-y1) ;
		        while( XCheckWindowEvent(_DISPLAY,LastEventWindow,
						    ExposureMask,&xevent) ) {
		    	  if( !xevent.xexpose.count ) {
			    int nx1 = xevent.xexpose.x ;
			    int ny1 = xevent.xexpose.y ;
			    int nx2 = nx1 + xevent.xexpose.width ;
			    int ny2 = ny1 + xevent.xexpose.height ;

			    x1 = min(x1,nx1) ; y1 = min(y1,ny1) ;
			    x2 = max(x2,nx2) ; y2 = max(y2,ny2) ;
			    xwevent.windowexpose.xc = (x1+x2)/2 ;
			    xwevent.windowexpose.yc = (y1+y2)/2 ;
			    xwevent.windowexpose.width = (x2-x1) ;
			    xwevent.windowexpose.height = (y2-y1) ;
			  }
			}
		    }
		    break ;
		case ConfigureNotify  :             
		    { 
			int xc = _X + _WIDTH/2 ;
		    	int yc = _Y + _HEIGHT/2 ;
		    	int width = _WIDTH ;
		    	int height = _HEIGHT ;

			pwindow->rwidth = xevent.xconfigure.width ;
			pwindow->rheight = xevent.xconfigure.height ;
		        Xw_get_window_position(pwindow,
					&xwevent.windowresize.xc,
					&xwevent.windowresize.yc,
					&xwevent.windowresize.width,
					&xwevent.windowresize.height) ;
		        if( (width != xwevent.windowresize.width) ||
			    (height != xwevent.windowresize.height) ) 
					xwevent.any.type = XW_RESIZEWINDOW ;
		        else if( (xc != xwevent.windowresize.xc) ||
			    	 (yc != xwevent.windowresize.yc) ) 
					xwevent.any.type = XW_MOVEWINDOW ;
		    }
		    break ;
		case ResizeRequest    :             
		    { 
			int xc = _X + _WIDTH/2 ;
		    	int yc = _Y + _HEIGHT/2 ;
		    	int width = _WIDTH ;
		    	int height = _HEIGHT ;

			pwindow->rwidth = xevent.xresizerequest.width ;
			pwindow->rheight = xevent.xresizerequest.height ;
		        Xw_get_window_position(pwindow,
					&xwevent.windowresize.xc,
					&xwevent.windowresize.yc,
					&xwevent.windowresize.width,
					&xwevent.windowresize.height) ;
		        if( (width != xwevent.windowresize.width) ||
			    (height != xwevent.windowresize.height) ) 
					xwevent.any.type = XW_RESIZEWINDOW ;
		        else if( (xc != xwevent.windowresize.xc) ||
			    	 (yc != xwevent.windowresize.yc) ) 
					xwevent.any.type = XW_MOVEWINDOW ;
		    }
		    break ;
		case UnmapNotify      :             
		    xwevent.any.type = XW_ICONIFYWINDOW ;
		    break ;
		case MapNotify        :             
		case ReparentNotify   :             
		case NoExpose         :             
		case ClientMessage    :             
		case GraphicsExpose   :             
		case DestroyNotify    :             
		case CreateNotify     :             
		    break ;
		case LeaveNotify      :             
		case FocusIn          :             
		case FocusOut         :             
		case KeymapNotify     :             
		case ConfigureRequest :             
		case GravityNotify    :             
		case CirculateNotify  :             
		case CirculateRequest :             
		case PropertyNotify   :             
		case SelectionClear   :             
		case SelectionRequest :             
		case SelectionNotify  :             
		case ColormapNotify   :             
		case MappingNotify    :             
		case VisibilityNotify :             
		case MapRequest       :             
		    /*WARNING*Unimplemented EVENT type*/
		    Xw_set_error(86,"Xw_wait_event",&xevent.type) ;
		    break ;
	    }

	    if((  icbk = pwindow->icbk[xwevent.any.type].function )) { 
	        status = (*icbk)(&xwevent) ;
	    }
	    if((  ecbk = pwindow->ecbk[xwevent.any.type].function )) { 
		userdata = pwindow->ecbk[xwevent.any.type].userdata ;
	        status = (*ecbk)(&xwevent,userdata) ;
	    }
	  }
	}

#ifdef TRACE_WAIT_EVENT
if( Xw_get_trace() ) {
	printf (" Xw_wait_event(%lx)\n",(long ) pwindow) ;
}
#endif
	return (status);

}
