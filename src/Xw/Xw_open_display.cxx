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

#define TEST	/* GG_220998
//		Ne pas detruire ls structure display qui peut etre
//		utilisee par plusieurs windows d'ou plantage
//		a la destruction (pb de liberation du static destructeur)
*/

#define RIC120302	/* GG Add Xw_set_display function to pass
//			Display structure directly
//			Add Xw_get_display_name function to retrieve
//			the display string.
*/

#include <Xw_Extension.h>
#include <string.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_OPEN_DISPLAY
#define TRACE_CLOSE_DISPLAY
#define TRACE_GET_DISPLAY
#define TRACE_GET_DISPLAY_NAME
#define TRACE_SET_DISPLAY
#endif

/*

   XW_EXT_DISPLAY* Xw_open_display(displayname)

   char *displayname ;	Display Name must be "NODE:[:]server.screen"
					     or NULL if default screen.
	Return the ext display address corresponding to the required displayname
	or NULL if name is wrong or Display is protected

   XW_EXT_DISPLAY* Xw_get_display(connexion)

   char *connexion ;	Display Name must be "NODE:[:]server.screen"
					     or NULL if default screen.
	Return the ext display address corresponding to the required displayname
	or NULL if Display is no more open

   char* Xw_get_display_name(adisplay)

   XW_EXT_DISPLAY* adisplay ;	

	Returns the display name.
   

   XW_EXT_DISPLAY* Xw_set_display(vdisplay)

   Display* vdisplay  ;	Display structure pointer. 
	Return the ext display address corresponding to the required displayname
	or NULL if vdisplay is NULL 

   XW_STATUS Xw_close_display(adisplay)

   XW_EXT_DISPLAY* adisplay ;	

	Close ALL ressources attached to this display

   int Xw_get_plane_layer(adisplay,visualid);

   XW_EXT_DISPLAY* adisplay
   int visualid		

  	Returns the plane layer ID from the Visual ID.

*/

#ifdef XW_PROTOTYPE
int Xw_get_plane_layer(void* adisplay,int visualid)
#else
int Xw_get_plane_layer(adisplay,visualid)
void* adisplay;
int visualid;
#endif /*XW_PROTOTYPE*/
/*
	GETS the plane layer ID from visual ID
*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*) adisplay ;
static Atom oatom = 0;
int layer = 0;

	if( !oatom ) {
	  oatom = XInternAtom(_DDISPLAY, "SERVER_OVERLAY_VISUALS", True);
	}
        if( oatom ) {
          unsigned long nitem,left;
	  int aformat;
          Status status;
          union {
            unsigned char *string;
            XOverlayVisualInfo *oinfo;
          } prop;
          Atom aatom;
          status = XGetWindowProperty(_DDISPLAY, _DROOT,
                                oatom, 0,  100, False,
                                AnyPropertyType, &aatom, &aformat,
                                &nitem, &left, &prop.string);
          nitem /= 4;
          if( (status == Success) && (aformat == 32) && (nitem > 0) ) {
            int i;
            for( i=0 ; (unsigned int ) i < nitem ; i++ ) {
	      if( prop.oinfo[i].overlay_visual == visualid ) {
		layer = prop.oinfo[i].layer;
		break;
	      }
	    }
          }
        } else if( pdisplay->server == XW_SERVER_IS_SUN ) {
          int mOptCode,fEvent,fError;
          if( XQueryExtension(_DDISPLAY,"SUN_OVL",&mOptCode,&fEvent,&fError) ) {
	    if( visualid == 0x2a ) layer = 1;
          }
        }

        return layer;
}

extern int Xw_error_handler() ;

#ifdef XW_PROTOTYPE
void* Xw_open_display(char* connexion)
#else
void* Xw_open_display(connexion)
char *connexion ;
#endif /*XW_PROTOTYPE*/
/*
	OPEN the display and
	Return the display address corresponding to the required displayname
	or NULL if name is wrong or Display is protected
*/
{
XW_EXT_DISPLAY *pdisplay ;
//XW_STATUS status ;
char *vendor ;

	pdisplay = (XW_EXT_DISPLAY*) Xw_get_display(connexion) ;

	if( !pdisplay ) { 
	    pdisplay = Xw_add_display_structure(sizeof(XW_EXT_DISPLAY)) ;
	}

	if( !pdisplay ) return (NULL) ;

	if( !_DDISPLAY ) {
	    _DDISPLAY = XOpenDisplay(connexion) ;
	    if( _DDISPLAY ) {
		vendor = ServerVendor(_DDISPLAY) ;
	        if( !strncmp(vendor,"DEC",3) ) 
				pdisplay->server = XW_SERVER_IS_DEC ;
		else if( !strncmp(vendor,"Sil",3) ) 
				pdisplay->server = XW_SERVER_IS_SGI ;
		else if( !strncmp(vendor,"Sun",3) ) 
				pdisplay->server = XW_SERVER_IS_SUN ;
		else if( !strncmp(vendor,"Hew",3) ) 
				pdisplay->server = XW_SERVER_IS_HP ;
		else pdisplay->server = XW_SERVER_IS_UNKNOWN ;
	        pdisplay->gname = (char*)strdup(connexion) ;

	        _DSCREEN  	= DefaultScreenOfDisplay(_DDISPLAY) ;
	        _DVISUAL  	= DefaultVisualOfScreen(_DSCREEN) ;
	        _DCOLORMAP 	= DefaultColormapOfScreen(_DSCREEN) ;
	        _DROOT		= RootWindowOfScreen(_DSCREEN) ;
		_DWIDTH 	= WidthOfScreen(_DSCREEN) ;
		_DHEIGHT	= HeightOfScreen(_DSCREEN) ;
		_DGC		= DefaultGCOfScreen(_DSCREEN) ;
		XSetFunction(_DDISPLAY,_DGC,GXxor) ;

		if( Xw_get_trace() > 0 ) {
	            Xw_set_synchronize(_DDISPLAY,True) ;
		} else {
	            Xw_set_synchronize(_DDISPLAY,False) ;
		    XSetErrorHandler(Xw_error_handler) ;
		}

#ifdef TRACE_OPEN_DISPLAY
if( Xw_get_trace() ) {
    		printf (" Xw_Server_Vendor is '%s' on Display '%s->%s'\n",
			vendor,connexion,DisplayString(_DDISPLAY));
		printf(" Xw_ProtocolVersion is %d,Revision is %d\n",
			ProtocolVersion(_DDISPLAY),ProtocolRevision(_DDISPLAY));
}
#endif
	    } else {
	    /*ERROR*Bad Display connexion*/
	        Xw_set_error(66,"Xw_open_display",connexion) ;
		Xw_del_display_structure(pdisplay) ;
		pdisplay = NULL ;
	    }
	}

#ifdef TRACE_OPEN_DISPLAY
if( Xw_get_trace() ) {
    printf (" %lx = Xw_open_display('%s')\n",(long ) pdisplay,connexion) ;
}
#endif

	return (pdisplay) ;
}

/*RIC120302*/
#ifdef XW_PROTOTYPE
char* Xw_get_display_name(void* adisplay)
#else
char* Xw_get_display_name(aDisplay)
void* adisplay;
#endif /*XW_PROTOTYPE*/
/*
	Retrieve the display name 
*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;
char *connexion;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_get_display_name",pdisplay) ;
            return (NULL) ;
        }

	connexion = DisplayString(_DDISPLAY);

#ifdef TRACE_GET_DISPLAY_NAME
if( Xw_get_trace() ) {
    printf (" '%s' = Xw_get_display_name(%x)\n", connexion, (long ) pdisplay) ;
}
#endif
	return connexion;
}

#ifdef XW_PROTOTYPE
void* Xw_set_display(void *vdisplay)
#else
void* Xw_open_display(vdisplay)
void *vdisplay ;
#endif /*XW_PROTOTYPE*/
/*
	REGISTER the display and
	Return the ext display address
	or NULL if the vdisplay is wrong
*/
{
XW_EXT_DISPLAY *pdisplay ;
//XW_STATUS status ;
char *vendor,*connexion ;

	if( !vdisplay ) return (NULL) ;

	connexion = DisplayString((Display*)vdisplay);
	pdisplay = (XW_EXT_DISPLAY*) Xw_get_display(connexion) ;

	if( !pdisplay ) { 
	    pdisplay = Xw_add_display_structure(sizeof(XW_EXT_DISPLAY)) ;
	}

	if( !pdisplay ) return (NULL) ;

	if( !_DDISPLAY ) {
	    _DDISPLAY = (Display*)vdisplay ;
	    vendor = ServerVendor(_DDISPLAY) ;
	    if( !strncmp(vendor,"DEC",3) ) 
				pdisplay->server = XW_SERVER_IS_DEC ;
	    else if( !strncmp(vendor,"Sil",3) ) 
				pdisplay->server = XW_SERVER_IS_SGI ;
	    else if( !strncmp(vendor,"Sun",3) ) 
				pdisplay->server = XW_SERVER_IS_SUN ;
    	    else if( !strncmp(vendor,"Hew",3) ) 
				pdisplay->server = XW_SERVER_IS_HP ;
	    else pdisplay->server = XW_SERVER_IS_UNKNOWN ;
	    pdisplay->gname = (char*)strdup(connexion) ;

	    _DSCREEN  	= DefaultScreenOfDisplay(_DDISPLAY) ;
	    _DVISUAL  	= DefaultVisualOfScreen(_DSCREEN) ;
	    _DCOLORMAP 	= DefaultColormapOfScreen(_DSCREEN) ;
	    _DROOT		= RootWindowOfScreen(_DSCREEN) ;
	    _DWIDTH 	= WidthOfScreen(_DSCREEN) ;
	    _DHEIGHT	= HeightOfScreen(_DSCREEN) ;
	    _DGC		= DefaultGCOfScreen(_DSCREEN) ;
	    XSetFunction(_DDISPLAY,_DGC,GXxor) ;

	    if( Xw_get_trace() > 0 ) {
	        Xw_set_synchronize(_DDISPLAY,True) ;
	    } else {
	        Xw_set_synchronize(_DDISPLAY,False) ;
	     XSetErrorHandler(Xw_error_handler) ;
	    }

#ifdef TRACE_SET_DISPLAY
if( Xw_get_trace() ) {
    		printf (" Xw_Server_Vendor is '%s' on Display '%s->%s'\n",
			vendor,connexion,DisplayString(_DDISPLAY));
		printf(" Xw_ProtocolVersion is %d,Revision is %d\n",
			ProtocolVersion(_DDISPLAY),ProtocolRevision(_DDISPLAY));
}
#endif
	}

#ifdef TRACE_SET_DISPLAY
if( Xw_get_trace() ) {
    printf (" %lx = Xw_set_display(%x = '%s')\n",
			(long ) pdisplay,vdisplay,connexion) ;
}
#endif

	return (pdisplay) ;
}
/*RIC120302*/

static XW_EXT_DISPLAY *PdisplayList = NULL ;

#ifdef XW_PROTOTYPE
void* Xw_get_display(char* uname)
#else
void* Xw_get_display(uname)
char *uname ;
#endif /*XW_PROTOTYPE*/
/*
	Return the display address corresponding to the required displayname
	or NULL if Display is no more open
*/
{
XW_EXT_DISPLAY *pdisplay ;

	for( pdisplay=PdisplayList ; pdisplay ; 
			pdisplay = (XW_EXT_DISPLAY*) pdisplay->link ) {
	    if( pdisplay->display ) {
	        if( !uname || !strlen(uname) || 
				!strcmp(pdisplay->gname,uname) ) break ;
	    }
	}

	return (pdisplay) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_display(void* adisplay)
#else
XW_STATUS Xw_close_display(adisplay)
void *adisplay ;
#endif /*XW_PROTOTYPE*/
/*
	Close the Display
*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*)adisplay ;

        if( !Xw_isdefine_display(pdisplay) ) {
            /*ERROR*Bad EXT_DISPLAY Address*/
            Xw_set_error(96,"Xw_close_display",pdisplay) ;
            return (XW_ERROR) ;
        }

        if( !pdisplay ) {
            while( Xw_del_display_structure(NULL) ) ;
        } else {
#ifndef TEST
            Xw_del_display_structure(pdisplay) ;
#endif
        }

#ifdef  TRACE_CLOSE_DISPLAY
if( Xw_get_trace() ) {
    printf (" Xw_close_display(%lx)\n",(long ) pdisplay) ;
}
#endif

	return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
void Xw_set_synchronize(Display* display,int state)
#else
void Xw_set_synchronize(display,state)
Display *display ;
int state ;
#endif /*XW_PROTOTYPE*/
/*
	Set Synchronization on display or ALL display if NULL
*/
{
XW_EXT_DISPLAY *pdisplay ;
//int i,sync = (state > 0) ? True : False ;
int sync = (state > 0) ? True : False ;

	if( display ) {
	    XSynchronize(display,sync) ;
	} else {
	    for( pdisplay=PdisplayList ; pdisplay ; 
			pdisplay = (XW_EXT_DISPLAY*) pdisplay->link) {
		XSynchronize(_DDISPLAY,sync) ;
	    }
	}
}

#ifdef XW_PROTOTYPE
XW_EXT_DISPLAY* Xw_add_display_structure(int size)
#else
XW_EXT_DISPLAY* Xw_add_display_structure(size)
int size ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert one Extended display structure in the
        EXtended Display List

        returns Extended display address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_DISPLAY *pdisplay = (XW_EXT_DISPLAY*) Xw_malloc(size) ;

        if( pdisplay ) {
            pdisplay->type = DISPLAY_TYPE ;
            pdisplay->link = PdisplayList ;
	    pdisplay->display = NULL ;
	    pdisplay->screen = NULL ;
	    pdisplay->visual = NULL ;
	    pdisplay->colormap = 0 ;
	    pdisplay->rootwindow = 0 ;
	    pdisplay->grabwindow = 0 ;
	    pdisplay->gamma = 0. ;
	    pdisplay->gname = NULL ;
            PdisplayList = pdisplay ;
        } else {
            /*ERROR*EXT_DISPLAY allocation failed*/
            Xw_set_error(94,"Xw_add_display_structure",0) ;
        }

        return (pdisplay) ;
}

#ifdef XW_PROTOTYPE
XW_EXT_DISPLAY* Xw_get_display_structure(Display *display)
#else
XW_EXT_DISPLAY* Xw_get_display_structure(display)
Display *display ;
#endif /*XW_PROTOTYPE*/
/*
        Return the Extended display address corresponding to the required
        Display Id or NULL if not found
*/
{
XW_EXT_DISPLAY *pdisplay = PdisplayList ;

        while (pdisplay) {
            if( _DDISPLAY == display ) {
                break ;
            }
            pdisplay = (XW_EXT_DISPLAY*) pdisplay->link ;
        }

        return (pdisplay) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_display_structure(XW_EXT_DISPLAY* pdisplay)
#else
XW_STATUS Xw_del_display_structure(pdisplay)
XW_EXT_DISPLAY *pdisplay;
#endif /*XW_PROTOTYPE*/
/*
        Remove the Extended display address or the next from the Extended List
        and Free the Extended Display

        returns ERROR if the display address is not Found in the list
                      or NO more Display exist
        returns SUCCESS if successful
*/
{
XW_EXT_DISPLAY *qdisplay = PdisplayList ;
//int i ;

        if( !qdisplay ) return (XW_ERROR) ;

        if( pdisplay == qdisplay ) {
            PdisplayList = (XW_EXT_DISPLAY*) pdisplay->link ;
        } else if( !pdisplay ) {
            if( !PdisplayList ) return (XW_ERROR) ;
            pdisplay = PdisplayList ;
            PdisplayList = (XW_EXT_DISPLAY*) PdisplayList->link ;
        } else {
            for( ; qdisplay ; qdisplay = (XW_EXT_DISPLAY*) qdisplay->link ) {
                if( qdisplay->link == pdisplay ) {
                    qdisplay->link = pdisplay->link ;
                    break ;
                }
            }
        }
#ifdef BUG
        if( _DDISPLAY ) XCloseDisplay(_DDISPLAY) ;
#endif

	if( pdisplay->gname ) Xw_free(pdisplay->gname);
        Xw_free(pdisplay) ;

        return (XW_SUCCESS) ;
}

#ifndef Xw_malloc
void* Xw_malloc(size)
int size ;
/*
	Dynamic Memory allocation of char size
*/
{
void *address ;

	address = malloc(size) ;

	return (address) ;
}
#endif

#ifndef Xw_calloc
void* Xw_calloc(length,size)
int length,size ;
/*
	Dynamic Memory allocation of char length*size and set memory to 0
*/
{
void *address ;

	address = calloc(length,size) ;

	return (address) ;
}
#endif

#ifndef Xw_free
void Xw_free(address)
void *address ;
/*
	Free Dynamic memory
*/
{
	free(address) ;
}
#endif

#ifdef XW_PROTOTYPE
void Xw_longcopy (register long* from,register long* to,register unsigned n)
#else
void Xw_longcopy (from,to,n)
    register long *from,*to ;
    register unsigned n;
#endif
/*
    Copy long from to ...
*/
{
    while ( n ) {
        *to++ = *from++ ;
	--n ;
    }
}

#ifdef XW_PROTOTYPE
void Xw_shortcopy (register short* from,register short* to,register unsigned n)
#else
void Xw_shortcopy (from,to,n)
    register short *from,*to ;
    register unsigned n;
#endif
/*
    Copy short from to ...
*/
{
    while ( n ) {
        *to++ = *from++ ;
	--n ;
    }
}

#ifdef XW_PROTOTYPE
void Xw_bytecopy (register char* from,register char* to,register unsigned n)
#else
void Xw_bytecopy (from,to,n)
    register char *from,*to ;
    register unsigned n;
#endif
/*
    Copy byte from to ...
*/
{
    while ( n ) {
        *to++ = *from++ ;
	--n ;
    }
}
