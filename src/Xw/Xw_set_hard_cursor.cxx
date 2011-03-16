
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_HARD_CURSOR
#endif

/*
   XW_STATUS Xw_set_hard_cursor (awindow,type,grab,r,g,b)
	XW_EXT_WINDOW 	*awindow	
	unsigned int	type	Cursor shape id
	float	r,g,b	Cursor Color components in [0,1] space
	int	grab    Grabbing flag .

	Associate the cursor to the window
	cursor must be one of "/usr/include/X11/cursorfont.h"
	NOTE than :
	1)	If grabbing flag is True,the cursor is grabbed to this Window,
		  You can received Mouse Events outside of this Window .
	2)	If type is 0,this deactivated the hard cursor .

	Returns XW_ERROR if cursorId is out of range
	Returns XW_SUCCESS if successful

*/

static Cursor CursorList[MAXCURSOR] ;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_hard_cursor (void *awindow,int type,int grab,float r,float g,float b)
#else
XW_STATUS Xw_set_hard_cursor (awindow,type,grab,r,g,b)
void *awindow ;
int type;
float r,g,b ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_DISPLAY *pdisplay = pwindow->connexion ;
XColor fcolor,bcolor ;
XW_STATUS status ;
int index,grabstatus ;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_set_hard_cursor",pwindow) ;
        return (XW_ERROR) ;
    }

    if( type < 0 || type >= MAXCURSOR ) {
        /*WARNING*Bad Cursor type*/
        Xw_set_error(70,"Xw_set_hard_cursor",&type) ;
        return (XW_ERROR) ;
    }

    if( type ) {
	if( !CursorList[type] ) {
	    CursorList[type] = XCreateFontCursor(_DISPLAY,type) ;
	}

	status = Xw_get_color_index(_COLORMAP,r,g,b,&index) ;
	if( !status ) return (XW_ERROR) ;
	fcolor.pixel = _COLORMAP->pixels[index] ;
	fcolor.red = (short unsigned int )( r * 0xFFFF) ;
	fcolor.green = (short unsigned int )( g * 0xFFFF) ;
	fcolor.blue = (short unsigned int )( b * 0xFFFF) ;
	if( _CLASS != TrueColor ) {
	    XQueryColor(_DISPLAY,_COLORMAP->info.colormap,&fcolor) ;	
	}

	status = Xw_get_color_index(_COLORMAP,1.-r,1.-g,1.-b,&index) ;
	if( !status ) return (XW_ERROR) ;
	bcolor.pixel = _COLORMAP->pixels[index] ;
	bcolor.red = (short unsigned int )( (1.-r) * 0xFFFF) ;
	bcolor.green = (short unsigned int )( (1.-g) * 0xFFFF) ;
	bcolor.blue = (short unsigned int )( (1.-b) * 0xFFFF) ;
	if( _CLASS != TrueColor ) {
	    XQueryColor(_DISPLAY,_COLORMAP->info.colormap,&bcolor) ;	
	}

	XRecolorCursor(_DISPLAY,CursorList[type],&fcolor,&bcolor) ;

	if( grab ) {
  	    grabstatus = XGrabPointer(_DISPLAY,_WINDOW,False,_EVENT_MASK,
              			GrabModeAsync,GrabModeAsync,
					_DROOT,CursorList[type],CurrentTime);
	    if( grabstatus == GrabSuccess ) {
		_DGRAB = _WINDOW ;
	    } else {
		/*Cann't GRAB Cursor id on this Window*/
		Xw_set_error(98,"Xw_set_hard_cursor",&type) ;
		return (XW_ERROR) ;
	    }
	} else {
	    if( _DGRAB ) {
		_DGRAB = 0 ;
		XUngrabPointer(_DISPLAY,CurrentTime) ;
	    }
	    XDefineCursor(_DISPLAY,_WINDOW,CursorList[type]) ;
	}
    } else {
	if( grab ) {
  	    grabstatus = XGrabPointer(_DISPLAY,_WINDOW,False,_EVENT_MASK,
                        	GrabModeAsync,GrabModeAsync,
					_DROOT,None,CurrentTime);
	    if( grabstatus == GrabSuccess ) {
		_DGRAB = _WINDOW ;
	    } else {
		/*Cann't GRAB Cursor id on this Window*/
		Xw_set_error(98,"Xw_set_hard_cursor",&type) ;
		return (XW_ERROR) ;
	    }
	} else {
	    if( _DGRAB ) {
		_DGRAB = 0 ;
		XUngrabPointer(_DISPLAY,CurrentTime) ;
	    }
	    XUndefineCursor(_DISPLAY,_WINDOW) ;
	}
    }

#ifdef TRACE_SET_HARD_CURSOR
if( Xw_get_trace() ) {
	printf (" Xw_set_hard_cursor(%lx,%d,%d,%f,%f,%f)\n",
						(long ) pwindow,type,grab,r,g,b) ;
}
#endif
	return (XW_SUCCESS);

}
