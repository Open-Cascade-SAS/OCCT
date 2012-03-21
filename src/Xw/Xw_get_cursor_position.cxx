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
#define TRACE_GET_CURSOR_POSITION
#endif

/*
   XW_STATUS Xw_get_cursor_position (awindow,x,y)
	XW_EXT_WINDOW 	*awindow	
	int *x,*y	Returned Window relative Cursor position

	Get the Window cursor position

	Returns XW_ERROR if the cursor is out of the window
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_cursor_position (void *awindow,int *x,int *y)
#else
XW_STATUS Xw_get_cursor_position (awindow,x,y)
void *awindow ;
int *x,*y ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
//XW_EXT_DISPLAY *pdisplay = pwindow->connexion ;
int status ;
Window root,child ;
int root_x,root_y ;
unsigned int key ;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_get_cursor_position",pwindow) ;
        return (XW_ERROR) ;
    }

    status = XQueryPointer(_DISPLAY,_WINDOW,&root,&child,
					&root_x,&root_y,x,y,&key) ;
    if( *x < 0 || *y < 0 || *x > _WIDTH || *y > _HEIGHT ) status = XW_ERROR ;

#ifdef TRACE_GET_CURSOR_POSITION
if( Xw_get_trace() > 2 ) {
	printf (" Xw_get_cursor_position(%lx,%d,%d)\n",(long ) pwindow,*x,*y) ;
}
#endif
	return (XW_STATUS) status;

}
