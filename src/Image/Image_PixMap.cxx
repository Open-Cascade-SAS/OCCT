// Created on: 2010-07-18
// Created by: Kirill GAVRILOV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Image_PixMap.hxx>
#include <NCollection_AlignedAllocator.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_HANDLE (Image_PixMap, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Image_PixMap, Standard_Transient)

IMPLEMENT_STANDARD_HANDLE (Image_PixMapData, NCollection_Buffer)
IMPLEMENT_STANDARD_RTTIEXT(Image_PixMapData, NCollection_Buffer)

// =======================================================================
// function : Image_PixMap
// purpose  :
// =======================================================================
Image_PixMap::Image_PixMap()
: myImgFormat (Image_PixMap::ImgGray)
{
  //
}

// =======================================================================
// function : ~Image_PixMap
// purpose  :
// =======================================================================
Image_PixMap::~Image_PixMap()
{
  Clear();
}

Standard_Size Image_PixMap::SizePixelBytes (const Image_PixMap::ImgFormat thePixelFormat)
{
  switch (thePixelFormat)
  {
    case ImgGrayF:
      return sizeof(float);
    case ImgRGBAF:
    case ImgBGRAF:
      return sizeof(float) * 4;
    case ImgRGBF:
    case ImgBGRF:
      return sizeof(float) * 3;
    case ImgRGBA:
    case ImgBGRA:
      return 4;
    case ImgRGB32:
    case ImgBGR32:
      return 4;
    case ImgRGB:
    case ImgBGR:
      return 3;
    case ImgGray:
    default:
      return 1;
  }
}

// =======================================================================
// function : setFormat
// purpose  :
// =======================================================================
void Image_PixMap::setFormat (Image_PixMap::ImgFormat thePixelFormat)
{
  myImgFormat = thePixelFormat;
}

// =======================================================================
// function : InitWrapper
// purpose  :
// =======================================================================
bool Image_PixMap::InitWrapper (Image_PixMap::ImgFormat thePixelFormat,
                                Standard_Byte*          theDataPtr,
                                const Standard_Size     theSizeX,
                                const Standard_Size     theSizeY,
                                const Standard_Size     theSizeRowBytes)
{
  Clear();
  myImgFormat = thePixelFormat;
  if ((theSizeX == 0) || (theSizeY == 0) || (theDataPtr == NULL))
  {
    return false;
  }

  Handle(NCollection_BaseAllocator) anEmptyAlloc;
  myData.Init (anEmptyAlloc, Image_PixMap::SizePixelBytes (thePixelFormat),
               theSizeX, theSizeY, theSizeRowBytes, theDataPtr);
  return true;
}

// =======================================================================
// function : InitTrash
// purpose  :
// =======================================================================
bool Image_PixMap::InitTrash (Image_PixMap::ImgFormat thePixelFormat,
                              const Standard_Size     theSizeX,
                              const Standard_Size     theSizeY,
                              const Standard_Size     theSizeRowBytes)
{
  Clear();
  myImgFormat = thePixelFormat;
  if ((theSizeX == 0) || (theSizeY == 0))
  {
    return false;
  }

  // use argument only if it greater
  const Standard_Size aSizeRowBytes = std::max (theSizeRowBytes, theSizeX * SizePixelBytes (thePixelFormat));
  Handle(NCollection_BaseAllocator) anAlloc = new NCollection_AlignedAllocator (16);
  myData.Init (anAlloc, Image_PixMap::SizePixelBytes (thePixelFormat),
               theSizeX, theSizeY, aSizeRowBytes, NULL);
  return !myData.IsEmpty();
}

// =======================================================================
// function : InitZero
// purpose  :
// =======================================================================
bool Image_PixMap::InitZero (Image_PixMap::ImgFormat thePixelFormat,
                             const Standard_Size     theSizeX,
                             const Standard_Size     theSizeY,
                             const Standard_Size     theSizeRowBytes,
                             const Standard_Byte     theValue)
{
  if (!InitTrash (thePixelFormat, theSizeX, theSizeY, theSizeRowBytes))
  {
    return false;
  }
  memset (myData.ChangeData(), (int )theValue, SizeBytes());
  return true;
}

// =======================================================================
// function : InitCopy
// purpose  :
// =======================================================================
bool Image_PixMap::InitCopy (const Image_PixMap& theCopy)
{
  if (&theCopy == this)
  {
    // self-copying disallowed
    return false;
  }
  if (InitTrash (theCopy.myImgFormat, theCopy.SizeX(), theCopy.SizeY(), theCopy.SizeRowBytes()))
  {
    memcpy (myData.ChangeData(), theCopy.myData.Data(), theCopy.SizeBytes());
    return true;
  }
  return false;
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Image_PixMap::Clear()
{
  Handle(NCollection_BaseAllocator) anEmptyAlloc;
  myData.Init (anEmptyAlloc, Image_PixMap::SizePixelBytes (myImgFormat),
               0, 0, 0, NULL);
}

// =======================================================================
// function : PixelColor
// purpose  :
// =======================================================================
Quantity_Color Image_PixMap::PixelColor (const Standard_Integer theX,
                                         const Standard_Integer theY,
                                         Quantity_Parameter&    theAlpha) const
{
  if (IsEmpty()
   || theX < 0 || (Standard_Size )theX >= SizeX()
   || theY < 0 || (Standard_Size )theY >= SizeY())
  {
    theAlpha = 0.0; // transparent
    return Quantity_Color (0.0, 0.0, 0.0, Quantity_TOC_RGB);
  }

  switch (myImgFormat)
  {
    case ImgGrayF:
    {
      const Standard_ShortReal& aPixel = Value<Standard_ShortReal> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel)),
                             Quantity_Parameter (Standard_Real (aPixel)),
                             Quantity_Parameter (Standard_Real (aPixel)),
                             Quantity_TOC_RGB);
    }
    case ImgRGBAF:
    {
      const Image_ColorRGBAF& aPixel = Value<Image_ColorRGBAF> (theY, theX);
      theAlpha = aPixel.a();
      return Quantity_Color (Quantity_Parameter (aPixel.r()),
                             Quantity_Parameter (aPixel.g()),
                             Quantity_Parameter (aPixel.b()),
                             Quantity_TOC_RGB);
    }
    case ImgBGRAF:
    {    
      const Image_ColorBGRAF& aPixel = Value<Image_ColorBGRAF> (theY, theX);
      theAlpha = aPixel.a();
      return Quantity_Color (Quantity_Parameter (aPixel.r()),
                             Quantity_Parameter (aPixel.g()),
                             Quantity_Parameter (aPixel.b()),
                             Quantity_TOC_RGB);
    }
    case ImgRGBF:
    {
      const Image_ColorRGBF& aPixel = Value<Image_ColorRGBF> (theY, theX);
      theAlpha =  1.0; // opaque
      return Quantity_Color (Quantity_Parameter (aPixel.r()),
                             Quantity_Parameter (aPixel.g()),
                             Quantity_Parameter (aPixel.b()),
                             Quantity_TOC_RGB);
    }
    case ImgBGRF:
    {
      const Image_ColorBGRF& aPixel = Value<Image_ColorBGRF> (theY, theX);
      theAlpha =  1.0; // opaque
      return Quantity_Color (Quantity_Parameter (aPixel.r()),
                             Quantity_Parameter (aPixel.g()),
                             Quantity_Parameter (aPixel.b()),
                             Quantity_TOC_RGB);
    }
    case ImgRGBA:
    {
      const Image_ColorRGBA& aPixel = Value<Image_ColorRGBA> (theY, theX);
      theAlpha = Standard_Real (aPixel.a()) / 255.0;
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case ImgBGRA:
    {
      const Image_ColorBGRA& aPixel = Value<Image_ColorBGRA> (theY, theX);
      theAlpha = Standard_Real (aPixel.a()) / 255.0;
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case ImgRGB32:
    {
      const Image_ColorRGB32& aPixel = Value<Image_ColorRGB32> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case ImgBGR32:
    {
      const Image_ColorBGR32& aPixel = Value<Image_ColorBGR32> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case ImgRGB:
    {
      const Image_ColorRGB& aPixel = Value<Image_ColorRGB> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case ImgBGR:
    {
      const Image_ColorBGR& aPixel = Value<Image_ColorBGR> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case ImgGray:
    {
      const Standard_Byte& aPixel = Value<Standard_Byte> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel) / 255.0),
                             Quantity_TOC_RGB);
    }
    default:
    {
      // not supported image type
      theAlpha = 0.0; // transparent
      return Quantity_Color (0.0, 0.0, 0.0, Quantity_TOC_RGB);
    }
  }
}
