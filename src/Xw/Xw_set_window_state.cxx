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

#define TEST	/*GG_050198
//		Synchroniser le changement d'etat d'une fentre
*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_WINDOW_STATE
#endif

/*
   XW_STATUS Xw_set_window_state (awindow, state):
   XW_EXT_WINDOW *awindow
   WINDOWSTATE state ;

	Update the window state ,must be :

		POP	if the window must be viewable above all others .
		PUSH	if the window must be behind all others .
		ICONIFY	if the window must be iconify .
		UNKNOWN if the window must be only Mapped as his old state

	returns XW_SUCCESS always
*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_window_state (void *awindow , XW_WINDOWSTATE rstate)
#else
XW_STATUS Xw_set_window_state (awindow , rstate)
void *awindow;
XW_WINDOWSTATE rstate ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
  int xc,yc,width,height ;


  /*
   * wait_event ajoute par CAL le 21/03/95 pour pouvoir faire des tests
   * sur SGI. En effet, lorsque le display est une SGI, on est bloque
   * sur XWindowEvent. A analyser avec GG.
   */
  XW_WINDOWSTATE state ;

#ifndef TEST
  int wait_event = False ;
  char svalue[4] ;
	if( Xw_get_env("Xw_WAIT_EVENT",svalue,sizeof(svalue)) ) {
	    wait_event = True ;
	    printf(" Xw_WAIT_EVENT is %d\n",wait_event) ;
	}
#endif

	switch (rstate) {
	    case XW_MAP :
		if( Xw_get_window_position(pwindow,&xc,&yc,&width,&height) 
								== XW_ICONIFY ){
//		    XEvent event ;

		    XMapWindow(_DISPLAY,_WINDOW) ;
		    XFlush(_DISPLAY) ;
#ifdef TEST
		    do {
		      state = 
			Xw_get_window_position(pwindow,&xc,&yc,&width,&height) ;
		    } while ( state == XW_ICONIFY );
#else
		    if( _EVENT_MASK & ExposureMask ) {
			if( wait_event )
		        XWindowEvent(_DISPLAY,_WINDOW,ExposureMask,&event) ;
		    }
		    Xw_get_window_position(pwindow,&xc,&yc,&width,&height) ;
#endif
		    pwindow->axleft = xc - width/2 ;
		    pwindow->axright = xc + width/2 ;
		    pwindow->aytop = yc - height/2 ;
		    pwindow->aybottom = yc + height/2 ;
		}
		break ;

	    case XW_POP :
		if( Xw_get_window_position(pwindow,&xc,&yc,&width,&height) 
								!= XW_ICONIFY ){
		    XRaiseWindow(_DISPLAY,_WINDOW) ;
		} else {
//		    XEvent event ;

		    XMapRaised(_DISPLAY,_WINDOW) ;
		    XFlush(_DISPLAY) ;
#ifdef TEST
		    do {
		      state = 
			Xw_get_window_position(pwindow,&xc,&yc,&width,&height) ;
		    } while ( state == XW_ICONIFY );
#else
		    if( _EVENT_MASK & ExposureMask ) {
			if( wait_event )
		        XWindowEvent(_DISPLAY,_WINDOW,ExposureMask,&event) ;
		    }
		    Xw_get_window_position(pwindow,&xc,&yc,&width,&height) ;
#endif
		    pwindow->axleft = xc - width/2 ;
		    pwindow->axright = xc + width/2 ;
		    pwindow->aytop = yc - height/2 ;
		    pwindow->aybottom = yc + height/2 ;
		}
		break ;

	    case XW_PUSH :
		XLowerWindow(_DISPLAY,_WINDOW) ;
		break ;
	
	    case XW_ICONIFY :
		XIconifyWindow(_DISPLAY,_WINDOW,DefaultScreen(_DISPLAY)) ;
		break ;
#ifndef DEB
	default:
                break ;
#endif
	}
	XFlush(_DISPLAY) ;

#ifdef  TRACE_SET_WINDOW_STATE
if( Xw_get_trace() ) {
    printf (" Xw_set_window_state(%lx,%d)\n",(long ) pwindow,rstate) ;
}
#endif

	return (XW_SUCCESS);
}
