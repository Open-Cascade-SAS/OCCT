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
#define TRACE_SET_BACKGROUND_INDEX
#endif

/*
   XW_STATUS Xw_set_background_index(awindow,index):
   XW_EXT_WINDOW *awindow
   int index 		Background Index in the colormap

	Update Background Color index  .

	Returns XW_ERROR if Extended Window or Colormap is wrong
			or Index is not defined .
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_background_index (void *awindow,int index)
#else
XW_STATUS Xw_set_background_index (awindow,index)
void *awindow;
int index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
unsigned long backpixel,highpixel ;
int i ;

	if ( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXT_WINDOW Address*/
	    Xw_set_error(24,"Xw_set_background_index",pwindow) ;
	    return (XW_ERROR) ;
	}

	if( index >= 0 ) {
	  if ( !Xw_isdefine_colorindex(_COLORMAP,index) ) {
	    /*ERROR*Bad Defined Color Index*/
	    Xw_set_error(41,"Xw_set_background_index",&index) ;
	    return (XW_ERROR) ;
	  }
	  backpixel = _COLORMAP->pixels[index] ;
	  _COLORMAP->define[index] = BACKCOLOR ;
	} else {
	  backpixel = _COLORMAP->backpixel ;
	}
	highpixel = _COLORMAP->highpixel ;
	XSetForeground(_DISPLAY, pwindow->qgwind.gccopy, highpixel) ;
	highpixel ^= backpixel ;

        XSetWindowBackground (_DISPLAY, _WINDOW, backpixel) ;
        XSetBackground(_DISPLAY, pwindow->qgwind.gc, backpixel) ;
        XSetBackground(_DISPLAY, pwindow->qgwind.gccopy, backpixel) ;
        XSetBackground(_DISPLAY, pwindow->qgwind.gcclear, backpixel) ;
        XSetBackground(_DISPLAY, pwindow->qgwind.gchigh, backpixel) ;
        XSetForeground(_DISPLAY, pwindow->qgwind.gcclear, backpixel) ;
	XSetForeground(_DISPLAY, pwindow->qgwind.gchigh, highpixel) ;
        for( i=0 ; i<MAXQG ; i++ ) {
            XSetBackground(_DISPLAY, pwindow->qgline[i].gc, backpixel);
            XSetBackground(_DISPLAY, pwindow->qgpoly[i].gc, backpixel);
            XSetBackground(_DISPLAY, pwindow->qgtext[i].gc, backpixel);
            XSetBackground(_DISPLAY, pwindow->qgmark[i].gc, backpixel);
        }
	pwindow->backindex = index ;

#ifdef TRACE_SET_BACKGROUND_INDEX
if( Xw_get_trace() ) {
    printf(" Xw_set_background_index(%lx,%d)\n",(long ) pwindow,index) ;
}
#endif
	return (XW_SUCCESS);
}
