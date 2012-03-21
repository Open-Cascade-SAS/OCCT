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
#define TRACE_SET_INTERNAL_EVENT
#endif

/*
   XW_STATUS Xw_set_internal_event (awindow,eventtype,acallback)
	XW_EXT_WINDOW 	*awindow	
	EVENTTYPE	eventtype must be one of :

			MOUSEBUTTON
			MOUSEBUTTON1
			MOUSEBUTTON2
			MOUSEBUTTON3
			MOUSEMOVE
			MOUSEMOVEWITHBUTTON
			MOUSEMOVEWITHBUTTON1
			MOUSEMOVEWITHBUTTON2
			MOUSEMOVEWITHBUTTON3
			KEYBOARD
			ENTERWINDOW
			RESIZEWINDOW
			MOVEWINDOW
			ICONIFYWINDOW
			EXPOSEWINDOW
			ALLEVENT

	XW_ICALLBACK	acallback Internal CallBack Routine Address	
							(Can be NULL)

	Associate an Internal CallBack routine to the window
	This Routine is call when any Event of type EVENTTYPE is 
	received at Xw_wait_event time and MUST be One of Internal KNOWN routine
	NOTE than if callback is NULL ,this remove the corresponding
	event of type EVENTTYPE .

	Callback programming example :
	{
					  	Internal defined routine 
	    extern XW_ICALLBACK Xw_rubber_band(XW_EVENT* event) ;

	    Xw_set_internal_event(ext_window,MOUSEMOVEWITHBUTTON3,
							Xw_rubber_band) ;

	}

	Returns XW_ERROR if Extended Window address is undefined
	Returns XW_SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_internal_event (XW_EXT_WINDOW *awindow,
		     XW_EVENTTYPE eventtype,
		     XW_ICALLBACK acallback)
#else
XW_STATUS Xw_set_internal_event (awindow,eventtype,acallback)
XW_EXT_WINDOW *awindow ;
XW_EVENTTYPE eventtype ;
XW_ICALLBACK  acallback ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
int function = False ;
long mask=0 ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_set_internal_event",pwindow) ;
            return (XW_ERROR) ;
        }

	switch (eventtype) {
	    case XW_MOUSEBUTTON : 
		pwindow->icbk[XW_MOUSEBUTTON1].function = acallback ;	
		pwindow->icbk[XW_MOUSEBUTTON2].function = acallback ;	
		pwindow->icbk[XW_MOUSEBUTTON3].function = acallback ;	
		function = pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON3].function ||
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		break ;
	    case XW_MOUSEBUTTON1 : 
		pwindow->icbk[XW_MOUSEBUTTON1].function = acallback ;	
		function = pwindow->icbk[XW_MOUSEBUTTON2].function ||
			   pwindow->icbk[XW_MOUSEBUTTON3].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON2].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON3].function ||
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		break ;
	    case XW_MOUSEBUTTON2 : 
		pwindow->icbk[XW_MOUSEBUTTON2].function = acallback ;	
		function = pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON3].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON3].function ||
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		break ;
	    case XW_MOUSEBUTTON3 : 
		pwindow->icbk[XW_MOUSEBUTTON3].function = acallback ;	
		function = pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		break ;
	    case XW_MOUSEMOVE :
		pwindow->icbk[XW_MOUSEMOVE].function = acallback ;	
		function = pwindow->ecbk[XW_MOUSEMOVE].function ||
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = PointerMotionMask ;
		break ;
	    case XW_MOUSEMOVEWITHBUTTON :
		pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function = acallback ;	
		pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function = acallback ;	
		pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function = acallback ;	
		function = pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
		           pwindow->icbk[XW_MOUSEBUTTON3].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON3].function ;
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		function = pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->icbk[XW_MOUSEMOVE].function ||
		           pwindow->ecbk[XW_MOUSEMOVE].function ;
		if( !function ) mask |= ButtonMotionMask ;
		break ;
	    case XW_MOUSEMOVEWITHBUTTON1 :
		pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function = acallback ;	
		function = pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
		           pwindow->icbk[XW_MOUSEBUTTON3].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON3].function ||
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		function = pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->icbk[XW_MOUSEMOVE].function ||
		           pwindow->ecbk[XW_MOUSEMOVE].function ;
		if( !function ) mask |= ButtonMotionMask ;
		break ;
	    case XW_MOUSEMOVEWITHBUTTON2 :
		pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function = acallback ;	
		function = pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
		           pwindow->icbk[XW_MOUSEBUTTON3].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON3].function ||
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		function = pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->icbk[XW_MOUSEMOVE].function ||
		           pwindow->ecbk[XW_MOUSEMOVE].function ;
		if( !function ) mask |= ButtonMotionMask ;
		break ;
	    case XW_MOUSEMOVEWITHBUTTON3 :
		pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function = acallback ;	
		function = pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
		           pwindow->icbk[XW_MOUSEBUTTON3].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON3].function ||
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		function = pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->icbk[XW_MOUSEMOVE].function ||
		           pwindow->ecbk[XW_MOUSEMOVE].function ;
		if( !function ) mask |= ButtonMotionMask ;
		break ;

	    default :
		/*Unimplemented Internal Event*/
		Xw_set_error(96,"Xw_set_internal_event",&eventtype) ;
		return (XW_ERROR) ;
	}

	if( mask ) {
	    if( acallback ) _EVENT_MASK |= mask;
	    else	   _EVENT_MASK &= ~mask ; 
	    XSelectInput(_DISPLAY,_WINDOW,_EVENT_MASK) ;
	}

#ifdef TRACE_SET_INTERNAL_EVENT
if( Xw_get_trace() ) {
    printf (" Xw_set_internal_event(%lx,%d,%lx)\n",
			(long ) pwindow,eventtype,(long ) acallback) ;
}
#endif
	return (XW_SUCCESS);

}
