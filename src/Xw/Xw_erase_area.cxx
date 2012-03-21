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
#define TRACE_ERASE_AREA
#endif

/*
   STATUS Xw_erase_area (awindow,px,py,pwidth,pheight)
   XW_EXT_WINDOW *awindow
   int px,py            area center given in PIXELS
   int pwidth,pheight   area size given in PIXELS 

	Erase the attached Drawable area .

        returns ERROR if NO attached pixmap exist or
                         area is out of window
        returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_erase_area (void* awindow,
			int px,int py,int pwidth,int pheight)
#else
XW_STATUS Xw_erase_area (awindow,px,py,pwidth,pheight)
void *awindow;
int px,py,pwidth,pheight ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
int i ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_erase_area",pwindow) ;
            return (XW_ERROR) ;
        }

        if( (px + pwidth/2) < 0 || (px - pwidth/2) > _WIDTH ||
            (py + pheight/2) < 0 || (py - pheight/2) > _HEIGHT ) {
	    /*WARNING*Try to Erase outside the window*/
	    Xw_set_error(40,"Xw_erase_area",pwindow) ;
          return (XW_ERROR) ;
        }

	px -= pwidth/2 ; py -= pheight/2 ;

        if( _BPIXMAP ) {
            XCopyArea(_DISPLAY,_BPIXMAP,_DRAWABLE,pwindow->qgwind.gccopy,
                                                px,py,pwidth,pheight,px,py) ;
	} else {
	    if( _DRAWABLE == _WINDOW ) { 
	      	XClearArea(_DISPLAY,_WINDOW,px,py,pwidth,pheight,False) ;
	    } else {
	        XFillRectangle(_DISPLAY,_DRAWABLE,pwindow->qgwind.gcclear,
							px,py,pwidth,pheight) ;
	    }
	}

        for( i=1 ; i<MAXBUFFERS ; i++ ) {
	  if( _BUFFER(i).isdrawn ) {
	    int xmin,ymin,xmax,ymax ;
	    if( _BUFFER(i).isupdated ) {
	      xmin = _BUFFER(i).uxmin ;
	      ymin = _BUFFER(i).uymin ;
	      xmax = _BUFFER(i).uxmax ;
	      ymax = _BUFFER(i).uymax ;
	    } else {
	      xmin = _BUFFER(i).rxmin ;
	      ymin = _BUFFER(i).rymin ;
	      xmax = _BUFFER(i).rxmax ;
	      ymax = _BUFFER(i).rymax ;
	    }
	    if( xmax < px || xmin > px+pwidth ) continue ;
	    if( ymax < py || ymin > py+pheight ) continue ;
            Xw_erase_buffer(pwindow,-i) ;
	  }
        }

	XFlush(_DISPLAY) ;

#ifdef  TRACE_ERASE_AREA
if( Xw_get_trace() > 1 ) {
	printf(" Xw_erase_area(%lx,%d,%d,%d,%d)\n",(long ) pwindow,px,py,pwidth,pheight);
}
#endif

	return (XW_SUCCESS);
}
