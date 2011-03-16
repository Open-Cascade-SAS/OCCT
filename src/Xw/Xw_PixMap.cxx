// File		Xw_PixMap.cxx
// Created	14 October 1999
// Author	VKH
// Updated      GG 06/03/00 Xw_PixMap MUST creates a Pixmap and no more an Image !!!
//              GG IMP100701 Add the "depth" field and method
//              to the pixmap object.


//-Copyright	MatraDatavision 1999

//-Version

#define xTRACE 1

//
#include <errno.h>
#include <stdio.h>

#include <Xw_PixMap.ixx>

#include <Xw_Window.hxx>
#include <Xw_Extension.h>

#include <TCollection_AsciiString.hxx>
#include <Image_PixMap.hxx>

XW_STATUS Xw_save_xwd_image ( void*, void*, char* );
XW_STATUS Xw_save_bmp_image ( void*, void*, char* );
XW_STATUS Xw_save_gif_image ( void*, void*, char* );

Standard_Integer Xw_PixMap::PreferedDepth(
			const Handle(Aspect_Window)& aWindow,
		     	const Standard_Integer aDepth) const {
  Handle(Xw_Window) hwindow = Handle(Xw_Window)::DownCast(aWindow);
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) hwindow->ExtendedWindow();
  if( aDepth <= 0 ) return _DEPTH;
// Gets the nearest depth from possible screen depths
  Standard_Integer i,j;
  Screen *scr = DefaultScreenOfDisplay(_DISPLAY);
  for( i=j=0 ; i<scr->ndepths ; i++ ) {
    if( Abs(aDepth - scr->depths[i].depth) <
		Abs(aDepth - scr->depths[j].depth) ) {
      j = i;
    }
  }
  return scr->depths[j].depth;
}


//////////////////////////////////////////////////////////////////////////////////////////
Xw_PixMap::Xw_PixMap ( const Handle(Aspect_Window)& aWindow,
                       const Standard_Integer aWidth,
                       const Standard_Integer anHeight,
		       const Standard_Integer aDepth ) :
	Aspect_PixMap(aWidth, anHeight, PreferedDepth(aWindow, aDepth))
{

  myWindow = Handle(Xw_Window)::DownCast(aWindow);

  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) myWindow->ExtendedWindow();

  Xw_print_error();
  if( !Xw_get_trace() ) Xw_set_synchronize(_DISPLAY,True);
  myPixmap = XCreatePixmap( _DISPLAY, _WINDOW, myWidth, myHeight, myDepth);
  if( !Xw_get_trace() ) Xw_set_synchronize(_DISPLAY,False);

  if ( !myPixmap || Xw_print_error() ) {
    char errstring[256];
    sprintf(errstring," *** Xw_PixMap : Cann't allocates pixmap of size %d x %d, request failed with errno : '%s'",myWidth, myHeight,strerror(errno));
    Aspect_PixmapDefinitionError::Raise ( errstring );
  }
}

///////////////////////////////
void
Xw_PixMap::Destroy ()
{
  if( myPixmap ) {
#ifdef TRACE
    printf(" $$$ Xw_PixMap::Destroy() %x\n",myPixmap);
#endif
    XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) myWindow->ExtendedWindow();
    XFreePixmap(_DISPLAY,myPixmap);
  }
}


////////////////////////////////////////////////////////////
Standard_Boolean Xw_PixMap::Dump (const Standard_CString theFilename,
                                  const Standard_Real theGammaValue) const
{
  // the attributes
  XWindowAttributes winAttr;
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) myWindow->ExtendedWindow();
  XGetWindowAttributes (_DISPLAY, _WINDOW, &winAttr);

  // find the image
  XImage* pximage = XGetImage (_DISPLAY, myPixmap,
                               0, 0, myWidth, myHeight,
                               AllPlanes, ZPixmap);
  if (pximage == NULL)
  {
    return Standard_False;
  }

  if (winAttr.visual->c_class == TrueColor)
  {
    Standard_Byte* aDataPtr = (Standard_Byte* )pximage->data;
    Handle(Image_PixMap) anImagePixMap = new Image_PixMap (aDataPtr,
                                                           pximage->width, pximage->height,
                                                           pximage->bytes_per_line,
                                                           pximage->bits_per_pixel,
                                                           Standard_True);
    // destroy the image
    XDestroyImage (pximage);

    // save the image
    return anImagePixMap->Dump (theFilename, theGammaValue);
  }
  else
  {
    std::cerr << "Visual Type not supported!";
    // destroy the image
    XDestroyImage (pximage);
    return Standard_False;
  }
}

////////////////////////////////////////////////////////////
Aspect_Handle Xw_PixMap::PixmapID() const
{
  return myPixmap;
}

Quantity_Color Xw_PixMap::PixelColor (const Standard_Integer theX,
                                      const Standard_Integer theY) const
{
  // the attributes
  XWindowAttributes winAttr;
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) myWindow->ExtendedWindow();
  XGetWindowAttributes (_DISPLAY, _WINDOW, &winAttr);

  // find the image
  XImage* pximage = XGetImage (_DISPLAY, myPixmap,
                               0, 0, myWidth, myHeight,
                               AllPlanes, ZPixmap);
  if (pximage == NULL)
  {
    return Quantity_Color (0.0, 0.0, 0.0, Quantity_TOC_RGB);
  }

  XColor aColor;
  aColor.pixel = XGetPixel (pximage, theX, theY);
  XDestroyImage (pximage);

  XQueryColor (_DISPLAY, myWindow->XColorMap(), &aColor);
  return Quantity_Color (Quantity_Parameter (aColor.red)   / 65535.0,
                         Quantity_Parameter (aColor.green) / 65535.0,
                         Quantity_Parameter (aColor.blue)  / 65535.0,
                         Quantity_TOC_RGB);
}
