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

/*
// GG IMP060200	Xw_get_background_image signatuire change 
*/
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_BACKGROUND_IMAGE
#endif

/*
   XW_STATUS Xw_get_background_image (awindow,xc,yc,width,height):
   XW_EXT_WINDOW *awindow
   int xc,yc			Image center location in pixels 
   int width,height		Image size in pixels

	Get the window Background from the actual Window
	at the required Center location 

	returns  ERROR if Image is outside of the window
	returns  SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_background_image (void* awindow,
			int xc,int yc,int width,int height)
#else
XW_STATUS Xw_get_background_image (awindow,xc,yc,width,height)
void *awindow;
int xc,yc ;
int width,height ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_DISPLAY *pdisplay = pwindow->connexion ;
XW_STATUS status ;
XImage *pimage ;
int wx,wy ;

    	if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_background_image",pwindow) ;
            return (XW_ERROR) ;
    	}

    	wx = xc - width/2 ;
    	wy = yc - height/2 ;

	if( _DGRAB ) {
	    wx += _X ; wy += _Y ;
    	    if( (wx + width) < 0 || wx  > _DWIDTH || 
				(wy + height) < 0 || wy > _DHEIGHT ) {
	        /*WARNING*Try to Get Image from outside the display*/
	        Xw_set_error(45,"Xw_get_background_image",0) ;
                return (XW_ERROR) ;
    	    }
    	    pimage = XGetImage(_DISPLAY,_DROOT,wx,wy,
					width,height,AllPlanes,ZPixmap) ;
	} else {
    	    if( (wx + width) < 0 || wx  > _WIDTH || 
				(wy + height) < 0 || wy > _HEIGHT ) {
	        /*WARNING*Try to Get Image from outside the window*/
	        Xw_set_error(45,"Xw_get_background_image",0) ;
                return (XW_ERROR) ;
    	    }
    	    pimage = XGetImage(_DISPLAY,_WINDOW,wx,wy,
					width,height,AllPlanes,ZPixmap) ;
	}

        if( _BPIXMAP ) Xw_close_background_pixmap(pwindow) ;

        if( _PIXMAP || _NWBUFFER > 0 ) {
	    status = Xw_open_background_pixmap(pwindow) ;
	    if( !status ) return (XW_ERROR) ;
            for( wx=0 ; wx<_WIDTH ; wx += width ) {
              for( wy=0 ; wy<_HEIGHT ; wy += height ) {
                XPutImage(_DISPLAY,_BPIXMAP,pwindow->qgwind.gccopy,
                        		pimage,0,0,wx,wy,width,height) ;
              }
            }
        } else {
            Pixmap pixmap = XCreatePixmap(_DISPLAY,_WINDOW,
                                                width,height,_DEPTH);
	    if( !pixmap ) return (XW_ERROR) ;
            XPutImage(_DISPLAY,pixmap,pwindow->qgwind.gccopy,
                        pimage,0,0,wx,wy,width,height) ;
            XSetWindowBackgroundPixmap(_DISPLAY,_WINDOW,pixmap) ;
            XFlush(_DISPLAY) ;
            XFreePixmap(_DISPLAY,pixmap) ;
        }

	Xw_erase_window(pwindow) ;

#ifdef  TRACE_GET_BACKGROUND_IMAGE
if( Xw_get_trace() ) {
    printf (" Xw_get_background_image(%lx,%d,%d,%d,%d)\n",
					(long ) pwindow,xc,yc,width,height);
}
#endif

    return (XW_SUCCESS);
}
