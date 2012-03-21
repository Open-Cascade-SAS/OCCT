// Created on: 1999-12-20
// Created by: VKH
// Copyright (c) 1999 Matra Datavision
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

//    SZV/GG IMP100701 Add the "depth" field and method
//              to the pixmap object.


//-Version

// include windows.h first to have all definitions available
#include <windows.h>

#include <WNT_PixMap.ixx>

#include <WNT_Window.hxx>

extern int DumpBitmapToFile( Handle(WNT_GraphicDevice)&, HDC, HBITMAP, char* );

#include <WNT_GraphicDevice.hxx>

Standard_Integer WNT_PixMap::PreferedDepth(
  const Handle(Aspect_Window)& aWindow,
  const Standard_Integer aCDepth) const
{
  Standard_Integer theDepth = 32;
  if (aCDepth <= 1) theDepth = 1;
  else if (aCDepth <=  4) theDepth =  4;
  else if (aCDepth <=  8) theDepth =  8;
  else if (aCDepth <= 16) theDepth = 16;
  else if (aCDepth <= 24) theDepth = 24;

  return theDepth;
}

///////////////////////////////////////////////////////////////////////////////////////
WNT_PixMap::WNT_PixMap ( const Handle(Aspect_Window)& aWindow,
                         const Standard_Integer aWidth,
                         const Standard_Integer anHeight,
                         const Standard_Integer aCDepth ) :
Aspect_PixMap(aWidth, anHeight, PreferedDepth(aWindow, aCDepth))
{
  myWND = aWindow;

  const Handle(WNT_Window)& hWindow = Handle(WNT_Window)::DownCast(aWindow);
  HDC hdc = GetDC ( (HWND)(hWindow->HWindow()) );
  HDC hdcMem = CreateCompatibleDC ( hdc );
  ReleaseDC ( (HWND)(hWindow->HWindow()), hdc );
  myDC = hdcMem;

  Standard_Integer theNbColors = 0, theFormat = PFD_TYPE_RGBA;

#ifdef BUG  // Our OpenGl driver supports only RGB mode.
  //WIL001: Color table can not be initialized - do not use
  if (myDepth <= 8)
  {
    theNbColors = (1 << myDepth);
    theFormat = PFD_TYPE_COLORINDEX;
  }
#endif

  Standard_Integer sizeBmi = Standard_Integer(sizeof(BITMAPINFO)+sizeof(RGBQUAD)*theNbColors);
  PBITMAPINFO pBmi = (PBITMAPINFO)(new char[sizeBmi]);
  ZeroMemory (  pBmi, sizeBmi  );

  pBmi->bmiHeader.biSize          = sizeof (BITMAPINFOHEADER); //sizeBmi
  pBmi->bmiHeader.biWidth         = aWidth;
  pBmi->bmiHeader.biHeight        = anHeight;
  pBmi->bmiHeader.biPlanes        = 1;
  pBmi->bmiHeader.biBitCount      = myDepth; //WIL001: was 24
  pBmi->bmiHeader.biCompression   = BI_RGB;

  LPVOID ppvBits;
  HBITMAP hBmp = CreateDIBSection ( hdcMem, pBmi, DIB_RGB_COLORS, &ppvBits, NULL, 0 );
  if ( !hBmp )
    Aspect_PixmapDefinitionError::Raise ( "CreateDIBSection" );
  SelectBitmap ( hdcMem, hBmp );
  myBitmap = hBmp;

  delete[] pBmi;

  if (myDepth > 1) {

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory (  &pfd, sizeof (PIXELFORMATDESCRIPTOR)  );
    pfd.nSize           =  sizeof (PIXELFORMATDESCRIPTOR);
    pfd.nVersion        =  1;
    pfd.dwFlags         =  PFD_SUPPORT_OPENGL | PFD_DRAW_TO_BITMAP;
    pfd.iPixelType      =  theFormat; //WIL001: was PFD_TYPE_RGBA
    pfd.cColorBits      =  myDepth; //WIL001: was 24
    pfd.cDepthBits      =  24;//
    pfd.iLayerType      =  PFD_MAIN_PLANE;

    Standard_Integer iPf = ChoosePixelFormat(hdcMem, &pfd);
    if ( !iPf )
      Aspect_PixmapDefinitionError::Raise ( "ChoosePixelFormat" );

    if ( !DescribePixelFormat ( hdcMem, iPf, sizeof(PIXELFORMATDESCRIPTOR), &pfd ) )
      Aspect_PixmapDefinitionError::Raise ( "DescribePixelFormat" );

    if ( !SetPixelFormat(hdcMem, iPf, &pfd) )
      Aspect_PixmapDefinitionError::Raise ( "SetPixelFormat" );
  }
}

///////////////////////////////
void WNT_PixMap::Destroy ()
{
  if ( myDC     ) DeleteDC ( (HDC)myDC );
  if ( myBitmap ) DeleteObject ( (HBITMAP)myBitmap );
}

////////////////////////////////////////////////////////////
Standard_Boolean WNT_PixMap::Dump ( const Standard_CString aFilename,
                                   const Standard_Real aGammaValue ) const
{
  // *** gamma correction must be implemented also on WNT system ...
  const Handle(WNT_Window) hWindow = Handle(WNT_Window)::DownCast(myWND);

  Handle(WNT_GraphicDevice) dev =
    Handle ( WNT_GraphicDevice )::DownCast ( hWindow->MyGraphicDevice );
  if ( dev.IsNull() ) return Standard_False;
  //Aspect_PixmapError::Raise ( "WNT_GraphicDevice is NULL" );

  return DumpBitmapToFile ( dev, (HDC)myDC, (HBITMAP)myBitmap, (Standard_PCharacter)aFilename );
}

////////////////////////////////////////////////////////////
Standard_Address WNT_PixMap::PixmapID() const
{
  return myDC;
}

Quantity_Color WNT_PixMap::PixelColor (const Standard_Integer ,
                                       const Standard_Integer ) const
{
  Aspect_PixmapError::Raise ("PixelColor() method not implemented!");
  return Quantity_Color (0.0, 0.0, 0.0, Quantity_TOC_RGB);
}
