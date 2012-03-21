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

/*
// GG PRO4521 05/08/96 
//		Capture an image across all the window memory plans.
// GG PRO6633 21/08/97
//		Avoid crash when image size is NULL.
// GG IMP060200
//		Capture an image from any pixmap.
*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_X11_EXTENSIONS_READDISPLAY_H
#include <X11/extensions/readdisplay.h>
#endif

#ifdef HAVE_X11_EXTENSIONS_TRANSOLV_H
#include <X11/extensions/transovl.h>
#endif

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_IMAGE
#endif

/*
   XW_EXT_IMAGEDATA* Xw_get_image (awindow,aimageinfo,xc,yc,width,height):
   XW_EXT_WINDOW *awindow
   XW_USERDATA *aimageinfo		Image Informations Address
   int xc,yc			Image center location in  pixels
   int width,height		Image size in pixels

	Get an image from the window at the required Center location 
	NOTE than current Window position and size are taken.

	returns  The image descriptor address if successful
	returns  NULL if BadAllocation or Image is outside of the window

   XW_EXT_IMAGEDATA* Xw_get_image_from_pixmap (awindow,aimageinfo,pixmap,xc,yc,width,height):
   XW_EXT_WINDOW *awindow
   XW_USERDATA *aimageinfo		Image Informations Address
   Aspect_Handle pixmap,
   int xc,yc			Image center location in  pixels
   int width,height		Image size in pixels

	Get an image from any pixmap at the required Center location and size

	returns  The image descriptor address if successful
	returns  NULL if BadAllocation or Image is outside of the pixmap 

*/

#ifdef XW_PROTOTYPE
void* Xw_get_image (void* awindow, void *aimageinfo,
		int xc,int yc,int width,int height)
#else
void* Xw_get_image (awindow,aimageinfo,xc,yc,width,height)
void *awindow;
void *aimageinfo ;
int xc,yc ;
int width,height ;
#endif // XW_PROTOTYPE
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_DISPLAY *pdisplay = pwindow->connexion ;
XW_EXT_IMAGEDATA *pimage ;
int wx,wy;
#if defined(HAVE_X11_EXTENSIONS_READDISPLAY_H) || defined(HAVE_X11_EXTENSIONS_TRANSOLV_H)
int mOptCode,fEvent,fError;
#endif

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_get_image",pwindow) ;
        return (NULL) ;
    }

    if( width <= 0 || height <= 0 ) return NULL;

    wx = xc - width/2 ;
    wy = yc - height/2 ;

    if( wx < 0 ) wx = 0;
    if( wy < 0 ) wy = 0;

    if( !(pimage = Xw_add_imagedata_structure(sizeof(XW_EXT_IMAGEDATA))) ) 
								return (NULL) ;

    pimage->pimageinfo = aimageinfo ;

    if( _DGRAB ) {
	wx += _X ; wy += _Y ;
        if( (wx + width) < 0 || wx  > _DWIDTH || 
				(wy + height) < 0 || wy > _DHEIGHT ) {
	    Xw_del_imagedata_structure(pimage) ;
	    /*WARNING*Try to Get Image from outside the display*/
	    Xw_set_error(45,"Xw_get_image",0) ;
            return (NULL) ;
	}

        if( width > _DWIDTH ) width = _DWIDTH;
        if( height > _DHEIGHT ) height = _DHEIGHT;
	if( wx + width > _DWIDTH ) width = _DWIDTH - wx;
	if( wy + height > _DHEIGHT ) height = _DHEIGHT - wy;

	_ICOLORMAP = NULL ;
	_IIMAGE = NULL ;
#ifdef HAVE_X11_EXTENSIONS_READDISPLAY_H
        if( XQueryExtension(_DISPLAY,"ReadDisplay",&mOptCode,&fEvent,&fError) ) {
	  XGrabServer(_DISPLAY);
	  XSync(_DISPLAY,False);
	  _IIMAGE = XReadDisplay(_DISPLAY,_DROOT,wx,wy,width,height,0,0);
	  XUngrabServer(_DISPLAY);
	  XSync(_DISPLAY,False);
	}
#endif	// HAVE_X11_EXTENSIONS_READDISPLAY_H
#ifdef HAVE_X11_EXTENSIONS_TRANSOLV_H
        if( XQueryExtension(_DISPLAY,"SUN_OVL",&mOptCode,&fEvent,&fError) ) {
	  XGrabServer(_DISPLAY);
	  XSync(_DISPLAY,False);
	  _IIMAGE = XReadScreen(_DISPLAY,_DROOT,wx,wy,width,height,False);
	  XUngrabServer(_DISPLAY);
	  XSync(_DISPLAY,False);
	}
#endif 	// HAVE_X11_EXTENSIONS_TRANSOLV_H
	if( !_IIMAGE )
        	_IIMAGE = XGetImage(_DISPLAY,_DROOT,wx,wy,
					width,height,AllPlanes,ZPixmap) ;
    } else {
        if( (wx + width) < 0 || wx  > _WIDTH || 
				(wy + height) < 0 || wy > _HEIGHT ) {
	    Xw_del_imagedata_structure(pimage) ;
	    /*WARNING*Try to Get Image from outside the window*/
	    Xw_set_error(45,"Xw_get_image",0) ;
            return (NULL) ;
	}

        if( width > _WIDTH ) width = _WIDTH;
        if( height > _HEIGHT ) height = _HEIGHT;
	if( wx + width > _WIDTH ) width = _WIDTH - wx;
	if( wy + height > _HEIGHT ) height = _HEIGHT - wy;

        _ICOLORMAP = _COLORMAP ;
	_IIMAGE = NULL;
#ifdef HAVE_X11_EXTENSIONS_READDISPLAY_H
        if( XQueryExtension(_DISPLAY,"ReadDisplay",&mOptCode,&fEvent,&fError) ) 
	{
	  unsigned int flag = XRD_IGNORE_LAYER_1 | XRD_IGNORE_LAYER_2 |
			      XRD_IGNORE_LAYER_MINUS_1;
	  int rx,ry;
	  Window child;
	  XGrabServer(_DISPLAY);
	  XSync(_DISPLAY,False);
	  XTranslateCoordinates(_DISPLAY,_WINDOW,_DROOT,wx,wy,&rx,&ry,&child);
	  _IIMAGE = XReadDisplay(_DISPLAY,_WINDOW,rx,ry,width,height,flag,0);
	  XUngrabServer(_DISPLAY);
	  XSync(_DISPLAY,False);
	}
#endif // HAVE_X11_EXTENSIONS_READDISPLAY_H
#ifdef HAVE_X11_EXTENSIONS_TRANSOLV_H
        if( XQueryExtension(_DISPLAY,"SUN_OVL",&mOptCode,&fEvent,&fError) ) 
	{
	  XGrabServer(_DISPLAY);
	  XSync(_DISPLAY,False);
	  _IIMAGE = XReadScreen(_DISPLAY,_WINDOW,wx,wy,width,height,False);
	  XUngrabServer(_DISPLAY);
	  XSync(_DISPLAY,False);
	}
#endif	// HAVE_X11_EXTENSIONS_TRANSOLV_H
	if( !_IIMAGE )
        	_IIMAGE = XGetImage(_DISPLAY,_WINDOW,wx,wy,
					width,height,AllPlanes,ZPixmap) ;
    }

    if( !_IIMAGE ) {
	Xw_del_imagedata_structure(pimage) ;
	/*WARNING*XImage allocation failed*/
	Xw_set_error(60,"Xw_get_image",0) ;
        return (NULL) ;
    }
    else if( _IIMAGE->depth > 24 ) _IIMAGE->depth = 24;

#ifdef  TRACE_GET_IMAGE
if( Xw_get_trace() ) {
    printf (" %lx = Xw_get_image(%lx,%lx,%d,%d,%d,%d)\n",
				(long ) pimage,(long ) pwindow,(long ) aimageinfo,xc,yc,width,height);
}
#endif

    return (pimage);
}

#ifdef XW_PROTOTYPE
void* Xw_get_image_from_pixmap (void* awindow, void *aimageinfo,
		Aspect_Handle pixmap, int xc,int yc,int width,int height)
#else
void* Xw_get_image_from_pixmap (awindow,aimageinfo,pixmap,xc,yc,width,height)
void *awindow;
void *aimageinfo ;
Aspect_Handle pixmap;
int xc,yc ;
int width,height ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
// XW_EXT_DISPLAY *pdisplay = pwindow->connexion ;
XW_EXT_IMAGEDATA *pimage ;
int wx,wy;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_get_image_from_pixmap",pwindow) ;
        return (NULL) ;
    }

    if( pixmap == 0 || width <= 0 || height <= 0 ) return NULL;

    wx = xc - width/2 ;
    wy = yc - height/2 ;

    if( wx < 0 ) wx = 0;
    if( wy < 0 ) wy = 0;

    if( !(pimage = Xw_add_imagedata_structure(sizeof(XW_EXT_IMAGEDATA))) )
                                                                return (NULL) ;
    pimage->pimageinfo = aimageinfo ;
    
    _IIMAGE = XGetImage(_DISPLAY,pixmap,wx,wy,
                                        width,height,AllPlanes,ZPixmap) ;

    if( !_IIMAGE ) {
        Xw_del_imagedata_structure(pimage) ;
        /*WARNING*XImage allocation failed*/
        Xw_set_error(60,"Xw_get_image_from_pixmap",0) ;
        return (NULL) ;
    } else {
      _IIMAGE->red_mask = _VISUAL->red_mask;
      _IIMAGE->green_mask = _VISUAL->green_mask;
      _IIMAGE->blue_mask = _VISUAL->blue_mask;
      if( _IIMAGE->depth > 24 ) _IIMAGE->depth = 24;
    }

#ifdef  TRACE_GET_IMAGE
if( Xw_get_trace() ) {
    printf (" %lx = Xw_get_image_from_pixmap(%lx,%lx,%lx,%d,%d,%d,%d)\n",
    (long ) pimage,(long ) pwindow,(long ) aimageinfo, pixmap,xc,yc,width,height);
}
#endif

    return (pimage);
}
