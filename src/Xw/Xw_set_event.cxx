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
#define TRACE_SET_EVENT
#endif

/*
   XW_STATUS Xw_set_event (awindow,eventtype,acallback,auserdata)
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

	XW_CALLBACK	acallback CallBack Routine Address	(Can be NULL)
	XW_USERDATA	auserdata  Associated UserData Address 	(Can be NULL)

	Associate an CallBack routine to the window
	This Routine is call when any Event of type EVENTTYPE is 
	received at Xw_wait_event time
	NOTE than if callback is NULL ,this remove the corresponding
	event of type EVENTTYPE .

	Callback programming example :
	{
	    static XW_CALLBACK any_callback(event,data)
	    XW_EVENT *event ;
	    XW_USERDATA *data ;
	    {
            printf(" This trap ANY Xw_event of type %d from Ext_window %x\n",
						event->type,event->any.awindow); 
	    } 

	    Xw_set_event(ext_window,XW_ALLEVENT,any_callback,user_data) ;

	}

	Returns XW_ERROR if Extended Window address is undefined
	Returns XW_SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_event (void *awindow,
		     XW_EVENTTYPE eventtype,
		     XW_CALLBACK acallback,
		     XW_USERDATA *auserdata)
#else
XW_STATUS Xw_set_event (awindow,eventtype,acallback,auserdata)
void *awindow ;
XW_EVENTTYPE eventtype ;
XW_CALLBACK acallback ;
XW_USERDATA  *auserdata ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
int i,function = False ;
long mask = 0 ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_set_event",pwindow) ;
            return (XW_ERROR) ;
        }

	switch (eventtype) {
	    case XW_MOUSEBUTTON : 
		pwindow->ecbk[XW_MOUSEBUTTON1].function = acallback ;	
		pwindow->ecbk[XW_MOUSEBUTTON1].userdata = auserdata ;	
		pwindow->ecbk[XW_MOUSEBUTTON2].function = acallback ;	
		pwindow->ecbk[XW_MOUSEBUTTON2].userdata = auserdata ;	
		pwindow->ecbk[XW_MOUSEBUTTON3].function = acallback ;	
		pwindow->ecbk[XW_MOUSEBUTTON3].userdata = auserdata ;	
		function = pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
		           pwindow->icbk[XW_MOUSEBUTTON3].function ||
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		break ;
	    case XW_MOUSEBUTTON1 : 
		pwindow->ecbk[XW_MOUSEBUTTON1].function = acallback ;	
		pwindow->ecbk[XW_MOUSEBUTTON1].userdata = auserdata ;	
		function = pwindow->ecbk[XW_MOUSEBUTTON2].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON3].function ||
		           pwindow->icbk[XW_MOUSEBUTTON2].function ||
			   pwindow->icbk[XW_MOUSEBUTTON3].function ||
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		break ;
	    case XW_MOUSEBUTTON2 : 
		pwindow->ecbk[XW_MOUSEBUTTON2].function = acallback ;	
		pwindow->ecbk[XW_MOUSEBUTTON2].userdata = auserdata ;	
		function = pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON3].function ||
		           pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON3].function ||
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		break ;
	    case XW_MOUSEBUTTON3 : 
		pwindow->ecbk[XW_MOUSEBUTTON3].function = acallback ;	
		pwindow->ecbk[XW_MOUSEBUTTON3].userdata = auserdata ;	
		function = pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
		           pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		break ;
	    case XW_MOUSEMOVE :
		pwindow->ecbk[XW_MOUSEMOVE].function = acallback ;	
		pwindow->ecbk[XW_MOUSEMOVE].userdata = auserdata ;	
		function = pwindow->icbk[XW_MOUSEMOVE].function ||
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = PointerMotionMask ;
		break ;
	    case XW_MOUSEMOVEWITHBUTTON :
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function = acallback ;	
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].userdata = auserdata ;	
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function = acallback ;	
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].userdata = auserdata ;	
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function = acallback ;	
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].userdata = auserdata ;	
		function = pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON3].function ||
		           pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
		           pwindow->icbk[XW_MOUSEBUTTON3].function ;
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		function = pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->ecbk[XW_MOUSEMOVE].function ||
		           pwindow->icbk[XW_MOUSEMOVE].function ;
		if( !function ) mask |= ButtonMotionMask ;
		break ;
	    case XW_MOUSEMOVEWITHBUTTON1 :
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function = acallback ;	
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].userdata = auserdata ;	
		function = pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON3].function ||
		           pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
		           pwindow->icbk[XW_MOUSEBUTTON3].function ||
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		function = pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->ecbk[XW_MOUSEMOVE].function ||
		           pwindow->icbk[XW_MOUSEMOVE].function ;
		if( !function ) mask |= ButtonMotionMask ;
		break ;
	    case XW_MOUSEMOVEWITHBUTTON2 :
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function = acallback ;	
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].userdata = auserdata ;	
		function = pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON3].function ||
		           pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
		           pwindow->icbk[XW_MOUSEBUTTON3].function ||
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		function = pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->ecbk[XW_MOUSEMOVE].function ||
		           pwindow->icbk[XW_MOUSEMOVE].function ;
		if( !function ) mask |= ButtonMotionMask ;
		break ;
	    case XW_MOUSEMOVEWITHBUTTON3 :
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function = acallback ;	
		pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].userdata = auserdata ;	
		function = pwindow->ecbk[XW_MOUSEBUTTON1].function ||
			   pwindow->ecbk[XW_MOUSEBUTTON2].function ||
		           pwindow->ecbk[XW_MOUSEBUTTON3].function ||
		           pwindow->icbk[XW_MOUSEBUTTON1].function ||
			   pwindow->icbk[XW_MOUSEBUTTON2].function ||
		           pwindow->icbk[XW_MOUSEBUTTON3].function ||
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ;	
		if( !function ) mask = ButtonPressMask | ButtonReleaseMask ;
		function = pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
		           pwindow->ecbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
		           pwindow->icbk[XW_MOUSEMOVEWITHBUTTON1].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON2].function ||	
			   pwindow->icbk[XW_MOUSEMOVEWITHBUTTON3].function ||	
		           pwindow->ecbk[XW_MOUSEMOVE].function ||
		           pwindow->icbk[XW_MOUSEMOVE].function ;
		if( !function ) mask |= ButtonMotionMask ;
		break ;
	    case XW_KEYBOARD :
		pwindow->ecbk[XW_KEYBOARD].function = acallback ;	
		pwindow->ecbk[XW_KEYBOARD].userdata = auserdata ;	
		mask = KeyPressMask | KeyReleaseMask ;
		break ;
	    case XW_ENTERWINDOW :
		pwindow->ecbk[XW_ENTERWINDOW].function = acallback ;	
		pwindow->ecbk[XW_ENTERWINDOW].userdata = auserdata ;	
		mask = EnterWindowMask ;
		break ;
	    case XW_MOVEWINDOW :
		pwindow->ecbk[XW_MOVEWINDOW].function = acallback ;	
		pwindow->ecbk[XW_MOVEWINDOW].userdata = auserdata ;	
		if( !pwindow->ecbk[XW_RESIZEWINDOW].function ) 
						mask = StructureNotifyMask ;
		break ;
	    case XW_RESIZEWINDOW :
		pwindow->ecbk[XW_RESIZEWINDOW].function = acallback ;	
		pwindow->ecbk[XW_RESIZEWINDOW].userdata = auserdata ;	
		if( !pwindow->ecbk[XW_MOVEWINDOW].function ) 
						mask = StructureNotifyMask ;
		break ;
	    case XW_ICONIFYWINDOW :
		pwindow->ecbk[XW_ICONIFYWINDOW].function = acallback ;	
		pwindow->ecbk[XW_ICONIFYWINDOW].userdata = auserdata ;	
		break ;
	    case XW_EXPOSEWINDOW :
		pwindow->ecbk[XW_EXPOSEWINDOW].function = acallback ;	
		pwindow->ecbk[XW_EXPOSEWINDOW].userdata = auserdata ;	
		mask = ExposureMask ;
		break ;

	    case XW_ALLEVENT :
		for( i=0 ; i<MAXEVENT ; i++ ) {
		    pwindow->ecbk[i].function = acallback ;
		    pwindow->ecbk[i].userdata = auserdata ;
		}
		pwindow->ecbk[XW_ALLEVENT].function = NULL ;	
		pwindow->ecbk[XW_ALLEVENT].userdata = NULL ;	
		mask = ButtonPressMask | ButtonReleaseMask ;
		mask |= PointerMotionMask | ButtonMotionMask ;
		mask |= KeyPressMask | KeyReleaseMask ;
		mask |= EnterWindowMask ;
		mask |= StructureNotifyMask ;
		mask |= ExposureMask ;
		break ;
	}

	if( mask ) {
	    if( acallback ) _EVENT_MASK |= mask;
	    else	   _EVENT_MASK &= ~mask ; 
	    XSelectInput(_DISPLAY,_WINDOW,_EVENT_MASK) ;
	}

#ifdef TRACE_SET_EVENT
if( Xw_get_trace() ) {
    printf (" Xw_set_event(%lx,%d,%lx,%lx)\n",
			(long ) pwindow,eventtype,(long ) acallback,(long ) auserdata) ;
}
#endif
	return (XW_SUCCESS);

}
