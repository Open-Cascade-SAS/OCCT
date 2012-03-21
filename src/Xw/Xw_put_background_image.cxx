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
#define TRACE_PUT_BACKGROUND_IMAGE
#define TRACE_CLEAR_BACKGROUND_IMAGE
#endif

#define OCC153 // bad memory management

/*
   STATUS Xw_put_background_image (awindow,image,method):
   XW_EXT_WINDOW *awindow
   XW_EXT_IMAGEDATA *image		Image structure
   Aspect_FillMethod method	Image fill method

	Draw an image to the window background according to the fill method. 

	returns ERROR if NO Image is defined
	returns  SUCCESS if successful

   STATUS Xw_clear_background_image (awindow):
   XW_EXT_WINDOW *awindow

	Clear the window background image

	returns  SUCCESS allways 

   STATUS Xw_update_background_image(awindow)
   XW_EXT_WINDOW *awindow

	Updates the window background from an image 
	according to the window size and fill method

	returns  SUCCESS if successful 


   STATUS Xw_set_background_pixmap(awindow,apixmap)
   XW_EXT_WINDOW *awindow
   Pixmap apixmap

	Updates the window background from a user pixmap


	returns  SUCCESS if successful 

   STATUS Xw_get_background_pixmap(awindow,apixmap)
   XW_EXT_WINDOW *awindow
   Pixmap *apixmap

	Returns the current background pixmap coming from
	an image or a user pixmap
*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_put_background_image (void* awindow,void* aimage,Aspect_FillMethod method)
#else
XW_STATUS Xw_put_background_image (awindow,aimage,method)
void *awindow;
void *aimage ;
Aspect_FillMethod method;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*) aimage ;
XW_STATUS status ;

	if( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXT_WINDOW Address*/
	    Xw_set_error(24,"Xw_put_background_image",pwindow) ;
	    return (XW_ERROR) ;
	}

	if( !Xw_isdefine_image(pimage) ) {
	    /*ERROR*Bad EXT_IMAGEDATA Address*/
	    Xw_set_error(25,"Xw_put_background_image",pimage) ;
	    return (XW_ERROR) ;
	}

#ifdef OCC153
	if( _BIMAGE )
        {
          if (_BIMAGE != pimage)
          {
            Xw_close_image(_BIMAGE);
            _BIMAGE = pimage;
          }
        }
        else
        {
          _BIMAGE = pimage;
        }
#else
	if( _BIMAGE ) Xw_close_image(_BIMAGE);
	_BIMAGE = pimage;
#endif
	_BMETHOD = method;

	status = Xw_update_background_image(pwindow);

#ifdef  TRACE_PUT_BACKGROUND_IMAGE
if( Xw_get_trace() )
    printf (" Xw_put_background_image(%lx,%lx)\n",(long ) pwindow,(long ) pimage) ;
#endif

	return status;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_clear_background_image (void* awindow)
#else
XW_STATUS Xw_clear_background_image (awindow)
void *awindow;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;

	if( _BIMAGE ) {
	    Xw_close_image( _BIMAGE );
	    _BIMAGE = NULL;
	}

	if( _BPIXMAP ) {
	    XFreePixmap(_DISPLAY,_BPIXMAP) ;
	    _BPIXMAP = 0 ;
	}

	XSetWindowBackgroundPixmap(_DISPLAY,_WINDOW,0) ;

	Xw_erase_window(pwindow) ;

#ifdef  TRACE_CLEAR_BACKGROUND_IMAGE
if( Xw_get_trace() ) {
    printf (" Xw_clear_background_image(%lx)\n",(long ) pwindow) ;
}
#endif

	return XW_SUCCESS;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_update_background_image(void* awindow) 
#else
XW_STATUS Xw_update_background_image(awindow)
void *awindow;
#endif
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_IMAGEDATA *pimage = _BIMAGE;
XW_STATUS status;
int ix,iy,iwidth,iheight,wx,wy;

	if( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXT_WINDOW Address*/
	    Xw_set_error(24,"Xw_update_background_image",pwindow) ;
	    return (XW_ERROR) ;
   	}

	if( !pimage ) return XW_ERROR;

	ix = iy = wx = wy = 0 ;
	iwidth = _IIMAGE->width;
	iheight = _IIMAGE->height;
	XImage *pximage = _IIMAGE ;
        if( _BMETHOD == Aspect_FM_NONE ) {
	  if( iwidth > _WIDTH ) {
	    iwidth = _WIDTH;
	  }
	  if( iheight > _HEIGHT ) {
	    iheight = _HEIGHT;
	  }
        } else if( _BMETHOD == Aspect_FM_CENTERED ) {
	  wx = (_WIDTH - iwidth)/2;
	  if( wx < 0 ) { ix = -wx; wx = 0; iwidth = _WIDTH; }
	  wy = (_HEIGHT - iheight)/2;
	  if( wy < 0 ) { iy = -wy; wy = 0; iheight = _HEIGHT; }
	} else if( _BMETHOD == Aspect_FM_STRETCH ) {
	  float zoom = 1.;
	  if( iwidth != _WIDTH ) {
	    zoom = (float)_WIDTH/iwidth;
	    iwidth *= (int)zoom; iheight *= (int)zoom;
	  }
	  if( iheight != _HEIGHT ) {
	    zoom *= (float)_HEIGHT/iheight;
	    iwidth *= (int)zoom; iheight *= (int)zoom;
	  }
	  if( _ZIMAGE && (zoom == pimage->zoom) ) {
	    pximage = _ZIMAGE;
	  } else {
	    status = Xw_zoom_image(pimage,zoom);
	    if( status ) {
	      iwidth = _ZIMAGE->width;
	      iheight = _ZIMAGE->height;
	      wx = (_WIDTH - iwidth)/2;
	      if( wx < 0 ) { ix = -wx; wx = 0; iwidth = _WIDTH; }
	      wy = (_HEIGHT - iheight)/2;
	      if( wy < 0 ) { iy = -wy; wy = 0; iheight = _HEIGHT; }
	      pximage = _ZIMAGE;
	    }
	  }
	} 

	if( _BPIXMAP ) Xw_close_background_pixmap(pwindow) ;

	status = Xw_open_background_pixmap(pwindow) ;
	if( !status ) return (XW_ERROR) ;

	if( _BMETHOD != Aspect_FM_TILED ) {
	  XPutImage(_DISPLAY,_BPIXMAP,pwindow->qgwind.gccopy,
	      		pximage,ix,iy,wx,wy,iwidth,iheight) ;
	} else {
	  for( wy=0 ; wy<_HEIGHT ; wy += iheight ) {
	    for( wx=0 ; wx<_WIDTH ; wx += iwidth ) {
	      XPutImage(_DISPLAY,_BPIXMAP,pwindow->qgwind.gccopy,
	      		pximage,ix,iy,wx,wy,iwidth,iheight) ;
	    }
	  }
	}
	XSetWindowBackgroundPixmap(_DISPLAY,_WINDOW,_BPIXMAP) ;

	Xw_erase_window(pwindow) ;

#ifdef  TRACE_PUT_BACKGROUND_IMAGE
  if( Xw_get_trace() )
    printf (" Xw_update_background_image(%lx)\n",(long ) pwindow) ;
#endif

	return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
 XW_STATUS Xw_set_background_pixmap( void * awindow, Aspect_Handle apixmap )
#else
 XW_STATUS Xw_set_background_pixmap( awindow, apixmap )
 void * awindow;
 Aspect_Handle apixmap;
#endif
{
  XW_EXT_WINDOW * pwindow = ( XW_EXT_WINDOW * ) awindow;

  if ( !Xw_isdefine_window( pwindow ) ) {
    Xw_set_error( 24, "Xw_set_background_pixmap", pwindow );
    return( XW_ERROR );
  }

 if ( !apixmap ) return XW_ERROR;

 if( _BIMAGE ) {
   Xw_close_image(_BIMAGE);
   _BIMAGE = NULL;
 }

 if( _BPIXMAP ) XFreePixmap( _DISPLAY, _BPIXMAP );

 _BPIXMAP = apixmap;

 XSetWindowBackgroundPixmap( _DISPLAY,_WINDOW,_BPIXMAP);

 Xw_erase_window(pwindow) ;

 return( XW_SUCCESS );

#ifdef  TRACE_PUT_BACKGROUND_IMAGE
  if( Xw_get_trace() )
    printf (" Xw_update_background_image(%lx)\n",(long ) pwindow) ;
#endif
}

#ifdef XW_PROTOTYPE
 XW_STATUS Xw_get_background_pixmap( void * awindow, Aspect_Handle& apixmap )
#else
 XW_STATUS Xw_get_background_pixmap( awindow, apixmap )
 void * awindow;
 Aspect_Handle& apixmap;
#endif
{
  XW_EXT_WINDOW * pwindow = ( XW_EXT_WINDOW * ) awindow;

 if ( !Xw_isdefine_window( pwindow ) ) {
   Xw_set_error( 24, "Xw_get_background_pixmap", pwindow );
   return( XW_ERROR );
 }

 if ( !_BPIXMAP ) return( XW_ERROR );

 apixmap = _BPIXMAP;
 
 return ( XW_SUCCESS );
}
