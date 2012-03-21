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
#define TRACE_GET_WINDOW_INFO
#endif

/*
   XW_STATUS Xw_get_window_info (awindow,window,pixmap,root,colormap,
							vclass,depth,visualid):
   XW_EXT_WINDOW *awindow	Extended Window structure
   Aspect_Handle *window		Window XID
   Aspect_Handle *pixmap		Pixmap XID or 0 if DoubleBuffering is DISABLE
   Aspect_Handle *root			Root XID
   Aspect_Handle *colormap		Colormap XID	
   Xw_TypeOfVisual *vclass	Visual Class
   int *depth			Planes depth
   int *visualid		visualid

	Returns the X window attributes of an existing window

	returns XW_ERROR if something is wrong
	returns XW_SUCCESS else

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_window_info (void *awindow,Aspect_Handle *window,Aspect_Handle *pixmap,
	Aspect_Handle *root,Aspect_Handle *colormap,Xw_TypeOfVisual  *vclass,int *depth,int *visualid)
#else
XW_STATUS Xw_get_window_info (awindow,window,pixmap,root,colormap,vclass,depth,visualid)
void *awindow;
Aspect_Handle *window;
Aspect_Handle *pixmap;
Aspect_Handle *root;
Aspect_Handle *colormap;
Xw_TypeOfVisual  *vclass;
int *depth;
int *visualid;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XWindowAttributes attributes ;

    	if ( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_window_info",pwindow) ;
            return (XW_ERROR) ;
    	}

        if( !XGetWindowAttributes(_DISPLAY,_WINDOW,&attributes) ) {
        /*ERROR*Bad Window Attributes*/
            Xw_set_error(54,"Xw_get_window_info",&_WINDOW) ;
            return (XW_ERROR) ;
        }

	*window = _WINDOW ;
	*pixmap = _PIXMAP ;
	*root = attributes.root ;
#if defined(__cplusplus) || defined(c_plusplus)
	*vclass = (Xw_TypeOfVisual) attributes.visual->c_class ;
#else
	*vclass = (Xw_TypeOfVisual) attributes.visual->class ;
#endif
	*depth = attributes.depth ;
	*colormap = attributes.colormap ;
   	*visualid = attributes.visual->visualid ;

#ifdef  TRACE_GET_WINDOW_INFO
if( Xw_get_trace() > 1 ) {
    printf (" Xw_get_window_info(%lx,%lx,%lx,%lx,%lx,%d,%d,%d)\n",
		(long ) awindow,(long ) *window,(long ) *pixmap,(long ) *root,(long ) *colormap,*vclass,*depth,*visualid) ;
}
#endif

	return (XW_SUCCESS) ;
}
