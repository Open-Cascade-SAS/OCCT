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
#define TRACE_RESIZE_WINDOW
#endif

/*
   Aspect_TypeOfResize Xw_resize_window (awindow)
   XW_EXT_WINDOW *awindow

	returns TypeOfResize method ,can be :
*/

#ifdef XW_PROTOTYPE
XW_RESIZETYPE Xw_resize_window (void *awindow)
#else
XW_RESIZETYPE Xw_resize_window (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
int xc,yc,width,height ;
XW_WINDOWSTATE state ;
XW_DOUBLEBUFFERMODE dbmode ;
XW_STATUS status ;
XW_RESIZETYPE mode = XW_TOR_UNKNOWN ;
int xleft,ytop,xright,ybottom,mask = 0 ;

	state = Xw_get_window_position(pwindow,&xc,&yc,&width,&height) ;

	if( state != XW_ICONIFY ) {
				/* Get Border Resize method */
	    xleft = xc - width/2 ;
	    xright = xc + width/2 ;
	    ytop = yc - height/2 ;
	    ybottom = yc + height/2 ;
	
	    if( abs(xleft - pwindow->axleft) > 2 ) 	mask |= 1 ; 
	    if( abs(xright - pwindow->axright) > 2 ) 	mask |= 2 ; 
	    if( abs(ytop - pwindow->aytop) > 2 ) 	mask |= 4 ; 
	    if( abs(ybottom - pwindow->aybottom) > 2 ) 	mask |= 8 ; 

	    switch (mask) {
		case 0 :
		    mode = XW_NO_BORDER ;
		    break ;
		case 1 :
		    mode = XW_LEFT_BORDER ;
		    break ;
		case 2 :
		    mode = XW_RIGHT_BORDER ;
		    break ;
		case 4 :
		    mode = XW_TOP_BORDER ;
		    break ;
		case 5 :
		    mode = XW_LEFT_AND_TOP_BORDER ;
		    break ;
		case 6 :
		    mode = XW_TOP_AND_RIGHT_BORDER ;
		    break ;
		case 8 :
		    mode = XW_BOTTOM_BORDER ;
		    break ;
		case 9 :
		    mode = XW_BOTTOM_AND_LEFT_BORDER ;
		    break ;
		case 10 :
		    mode = XW_RIGHT_AND_BOTTOM_BORDER ;
		    break ;
		default :
		    mode = XW_TOR_UNKNOWN ;
		    break ;
	    }

	    pwindow->axleft = xleft ;
	    pwindow->axright = xright ;
	    pwindow->aytop = ytop ;
	    pwindow->aybottom = ybottom ;

	} 

	if( mode != XW_NO_BORDER ) {
	    dbmode = Xw_get_double_buffer(pwindow) ;
	    _DRAWABLE = 0 ;
	    if( _PIXMAP  || _NWBUFFER > 0 ) Xw_close_pixmap(awindow) ;
	    status = Xw_set_double_buffer(pwindow,dbmode) ;
	} 

#ifdef  TRACE_RESIZE_WINDOW
if( Xw_get_trace() ) {
    printf(" %d = Xw_resize_window(%lx)\n",mode,(long ) pwindow) ;
}
#endif

	return (mode);
}
