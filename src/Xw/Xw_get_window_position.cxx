// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <Xw_Extension.h>

/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_WINDOW_POSITION
#define TRACE_SET_WINDOW_POSITION
#define TRACE_GET_WINDOW_SIZE
#define TRACE_GET_WINDOW_STATE
#endif

/*
   WINDOWSTATE Xw_get_window_position (awindow,xc,yc,width,height):
   XW_EXT_WINDOW *awindow
   int *xc,*yc,*width,*height

	Returns the window Center position & Size in Pixel space 
	of the X window

	returns Window status,must be :

		POP	The window is at the top of the screen
		PUSH	The window is at the bottom of the screen
		ICONIFY	The window is not mapped at the screen 
		UNKNOWN The window state is Unknown


   STATUS Xw_set_window_position (awindow,xc,yc,width,height):
   XW_EXT_WINDOW *awindow
   int xc,yc,width,height

	Sets the window Center position & Size in Pixel space 
	of the X window

	returns ERROR if ExtendedWindowAddress is missing 
	returns SUCCESS if Successfull

   STATUS Xw_get_window_size (awindow,width,height):
   XW_EXT_WINDOW *awindow
   int *width,*height

	Returns the window Size in Pixel space 
	of the X window

   WINDOWSTATE Xw_get_window_state (awindow):
   XW_EXT_WINDOW *awindow

  	Returns the Window status,must be :

		POP	The window is at the top of the screen
		PUSH	The window is at the bottom of the screen
		ICONIFY	The window is not mapped at the screen 
		UNKNOWN The window state is Unknown
*/

#ifdef XW_PROTOTYPE
XW_WINDOWSTATE Xw_get_window_position (void *awindow,int *xc,int *yc,int *width,int *height)
#else
XW_WINDOWSTATE Xw_get_window_position (awindow,xc,yc,width,height)
void *awindow;
int *xc,*yc,*width,*height ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
  XW_WINDOWSTATE state ;
  Window child ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_window_position",pwindow) ;
	    return (XW_WS_UNKNOWN) ;
        }

	XFlush(_DISPLAY) ;

	if( !XGetWindowAttributes(_DISPLAY,_WINDOW,&_ATTRIBUTES) ) {
	    /*ERROR*Bad Window Attributes*/
            Xw_set_error(54,"Xw_get_window_position",&_WINDOW) ;
	    return (XW_WS_UNKNOWN) ;
	}
	XTranslateCoordinates(_DISPLAY,_ROOT,_WINDOW,0,0,&_X,&_Y,&child) ;
    	_X = -_X ; _Y = -_Y ;
	*xc = _X + _WIDTH/2 ;
	*yc = _Y + _HEIGHT/2 ;
	*width = _WIDTH ;
	*height = _HEIGHT ;
	switch (_STATE) {
	    case IsUnmapped :
		state = XW_ICONIFY ;
		break ;
	    case IsUnviewable :
		state = XW_PUSH ;
		break ;
	    case IsViewable :
		state = XW_MAP ;
		break ;

	    default :
		state = XW_WS_UNKNOWN ;
	}

#ifdef  TRACE_GET_WINDOW_POSITION
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_window_position(%lx,%d,%d,%d,%d)\n",
				state,(long ) pwindow,*xc,*yc,*width,*height) ;
}
#endif

	return (state) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_window_position (void *awindow,int xc,int yc,int width,int height)
#else
XW_STATUS Xw_set_window_position (awindow,xc,yc,width,height)
void *awindow ;
int xc,yc,width,height ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
  int awidth,aheight,xleft,ytop,mask = 0 ;
  XWindowChanges values ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_set_window_position",pwindow) ;
	    return (XW_ERROR) ;
        }

	xleft = xc - width/2 ;
	if( abs(xleft - pwindow->axleft) > 2 ) {
	    values.x = xleft ; mask |= CWX ;
	} 

	ytop = yc - height/2 ;
	if( abs(ytop - pwindow->aytop) > 2 ) {
	    values.y = ytop ; mask |= CWY ;
	} 

	awidth = pwindow->axright - pwindow->axleft + 1 ;
	if( abs(width - awidth) > 2 ) {
	    values.width = width ; mask |= CWWidth ;
	} 

	aheight = pwindow->aybottom - pwindow->aytop + 1 ;
	if( abs(height - aheight) > 2 ) {
	    values.height = height ; mask |= CWHeight ;
	} 

	if( mask ) {	    
	    XConfigureWindow(_DISPLAY,_WINDOW,mask,&values) ;
	    XSync(_DISPLAY,True) ;
	}

#ifdef  TRACE_SET_WINDOW_POSITION
if( Xw_get_trace() > 1 ) {
    printf (" Xw_set_window_position(%lx,%d,%d,%d,%d)\n",
					(long ) pwindow,xc,yc,width,height) ;
}
#endif

	return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_window_size (void *awindow,int *width,int *height)
#else
XW_STATUS Xw_get_window_size (awindow,width,height)
void *awindow;
int *width,*height ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_window_size",pwindow) ;
	    return (XW_ERROR) ;
        }

	if( (_WIDTH > 0) && (_HEIGHT > 0) ) {
	  *width = _WIDTH ;
	  *height = _HEIGHT ;
	} else {
	  int xc,yc;
	  Xw_get_window_position (pwindow,&xc,&yc,width,height);
	}

#ifdef  TRACE_GET_WINDOW_SIZE
if( Xw_get_trace() > 1 ) {
    printf (" Xw_get_window_size(%lx,%d,%d)\n",(long ) pwindow,*width,*height) ;
}
#endif

	return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XW_WINDOWSTATE Xw_get_window_state (void *awindow)
#else
XW_WINDOWSTATE Xw_get_window_state (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
  XW_WINDOWSTATE state ;

  if( (_WIDTH > 0) && (_HEIGHT > 0) ) {
    switch (_STATE) {
      case IsUnmapped :
	state = XW_ICONIFY ;
	break ;
      case IsUnviewable :
	state = XW_PUSH ;
	break ;
      case IsViewable :
	state = XW_MAP ;
	break ;

      default :
	state = XW_WS_UNKNOWN ;
    }
  } else {
    int xc,yc,w,h;
    state = Xw_get_window_position (pwindow,&xc,&yc,&w,&h);
  }

#ifdef  TRACE_GET_WINDOW_STATE
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_window_state(%lx)\n",state,(long ) pwindow) ;
}
#endif

  return state;
}
