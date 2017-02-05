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
#include <Standard_ProgramError.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(Image_PixMap,Standard_Transient)

// =======================================================================
// function : Image_PixMap
// purpose  :
// =======================================================================
Image_PixMap::Image_PixMap()
: myImgFormat (Image_Format_Gray)
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

Standard_Size Image_PixMap::SizePixelBytes (const Image_Format thePixelFormat)
{
  switch (thePixelFormat)
  {
    case Image_Format_GrayF:
    case Image_Format_AlphaF:
      return sizeof(float);
    case Image_Format_RGBAF:
    case Image_Format_BGRAF:
      return sizeof(float) * 4;
    case Image_Format_RGBF:
    case Image_Format_BGRF:
      return sizeof(float) * 3;
    case Image_Format_RGBA:
    case Image_Format_BGRA:
      return 4;
    case Image_Format_RGB32:
    case Image_Format_BGR32:
      return 4;
    case Image_Format_RGB:
    case Image_Format_BGR:
      return 3;
    case Image_Format_Gray:
    case Image_Format_Alpha:
      return 1;
    case Image_Format_UNKNOWN:
      return 1;
  }
  return 1;
}

// =======================================================================
// function : SetFormat
// purpose  :
// =======================================================================
void Image_PixMap::SetFormat (Image_Format thePixelFormat)
{
  if (myImgFormat == thePixelFormat)
  {
    return;
  }

  if (!IsEmpty()
    && SizePixelBytes (myImgFormat) != SizePixelBytes (thePixelFormat))
  {
    throw Standard_ProgramError("Image_PixMap::SetFormat() - incompatible pixel format");
    return;
  }

  myImgFormat = thePixelFormat;
}

// =======================================================================
// function : InitWrapper
// purpose  :
// =======================================================================
bool Image_PixMap::InitWrapper (Image_Format        thePixelFormat,
                                Standard_Byte*      theDataPtr,
                                const Standard_Size theSizeX,
                                const Standard_Size theSizeY,
                                const Standard_Size theSizeRowBytes)
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
bool Image_PixMap::InitTrash (Image_Format        thePixelFormat,
                              const Standard_Size theSizeX,
                              const Standard_Size theSizeY,
                              const Standard_Size theSizeRowBytes)
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
bool Image_PixMap::InitZero (Image_Format        thePixelFormat,
                             const Standard_Size theSizeX,
                             const Standard_Size theSizeY,
                             const Standard_Size theSizeRowBytes,
                             const Standard_Byte theValue)
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
    case Image_Format_GrayF:
    {
      const Standard_ShortReal& aPixel = Value<Standard_ShortReal> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel)),
                             Quantity_Parameter (Standard_Real (aPixel)),
                             Quantity_Parameter (Standard_Real (aPixel)),
                             Quantity_TOC_RGB);
    }
    case Image_Format_AlphaF:
    {
      const Standard_ShortReal& aPixel = Value<Standard_ShortReal> (theY, theX);
      theAlpha = aPixel;
      return Quantity_Color (1.0, 1.0, 1.0, Quantity_TOC_RGB);
    }
    case Image_Format_RGBAF:
    {
      const Image_ColorRGBAF& aPixel = Value<Image_ColorRGBAF> (theY, theX);
      theAlpha = aPixel.a();
      return Quantity_Color (Quantity_Parameter (aPixel.r()),
                             Quantity_Parameter (aPixel.g()),
                             Quantity_Parameter (aPixel.b()),
                             Quantity_TOC_RGB);
    }
    case Image_Format_BGRAF:
    {    
      const Image_ColorBGRAF& aPixel = Value<Image_ColorBGRAF> (theY, theX);
      theAlpha = aPixel.a();
      return Quantity_Color (Quantity_Parameter (aPixel.r()),
                             Quantity_Parameter (aPixel.g()),
                             Quantity_Parameter (aPixel.b()),
                             Quantity_TOC_RGB);
    }
    case Image_Format_RGBF:
    {
      const Image_ColorRGBF& aPixel = Value<Image_ColorRGBF> (theY, theX);
      theAlpha =  1.0; // opaque
      return Quantity_Color (Quantity_Parameter (aPixel.r()),
                             Quantity_Parameter (aPixel.g()),
                             Quantity_Parameter (aPixel.b()),
                             Quantity_TOC_RGB);
    }
    case Image_Format_BGRF:
    {
      const Image_ColorBGRF& aPixel = Value<Image_ColorBGRF> (theY, theX);
      theAlpha =  1.0; // opaque
      return Quantity_Color (Quantity_Parameter (aPixel.r()),
                             Quantity_Parameter (aPixel.g()),
                             Quantity_Parameter (aPixel.b()),
                             Quantity_TOC_RGB);
    }
    case Image_Format_RGBA:
    {
      const Image_ColorRGBA& aPixel = Value<Image_ColorRGBA> (theY, theX);
      theAlpha = Standard_Real (aPixel.a()) / 255.0;
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case Image_Format_BGRA:
    {
      const Image_ColorBGRA& aPixel = Value<Image_ColorBGRA> (theY, theX);
      theAlpha = Standard_Real (aPixel.a()) / 255.0;
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case Image_Format_RGB32:
    {
      const Image_ColorRGB32& aPixel = Value<Image_ColorRGB32> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case Image_Format_BGR32:
    {
      const Image_ColorBGR32& aPixel = Value<Image_ColorBGR32> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case Image_Format_RGB:
    {
      const Image_ColorRGB& aPixel = Value<Image_ColorRGB> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case Image_Format_BGR:
    {
      const Image_ColorBGR& aPixel = Value<Image_ColorBGR> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel.r()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.g()) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel.b()) / 255.0),
                             Quantity_TOC_RGB);
    }
    case Image_Format_Gray:
    {
      const Standard_Byte& aPixel = Value<Standard_Byte> (theY, theX);
      theAlpha = 1.0; // opaque
      return Quantity_Color (Quantity_Parameter (Standard_Real (aPixel) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel) / 255.0),
                             Quantity_Parameter (Standard_Real (aPixel) / 255.0),
                             Quantity_TOC_RGB);
    }
    case Image_Format_Alpha:
    {
      const Standard_Byte& aPixel = Value<Standard_Byte> (theY, theX);
      theAlpha = Standard_Real (aPixel) / 255.0;
      return Quantity_Color (1.0, 1.0, 1.0, Quantity_TOC_RGB);
    }
    case Image_Format_UNKNOWN:
    {
      break;
    }
  }

  // unsupported image type
  theAlpha = 0.0; // transparent
  return Quantity_Color (0.0, 0.0, 0.0, Quantity_TOC_RGB);
}

// =======================================================================
// function : SetPixelColor
// purpose  :
// =======================================================================
void Image_PixMap::SetPixelColor (const Standard_Integer theX,
                                  const Standard_Integer theY,
                                  const NCollection_Vec4<float>& theColor)
{
  if (IsEmpty()
   || theX < 0 || Standard_Size(theX) >= SizeX()
   || theY < 0 || Standard_Size(theY) >= SizeY())
  {
    return;
  }

  switch (myImgFormat)
  {
    case Image_Format_GrayF:
    {
      ChangeValue<Standard_ShortReal> (theY, theX) = theColor.r();
      return;
    }
    case Image_Format_AlphaF:
    {
      ChangeValue<Standard_ShortReal> (theY, theX) = theColor.a();
      return;
    }
    case Image_Format_RGBAF:
    {
      Image_ColorRGBAF& aPixel = ChangeValue<Image_ColorRGBAF> (theY, theX);
      aPixel.r() = theColor.r();
      aPixel.g() = theColor.g();
      aPixel.b() = theColor.b();
      aPixel.a() = theColor.a();
      return;
    }
    case Image_Format_BGRAF:
    {
      Image_ColorBGRAF& aPixel = ChangeValue<Image_ColorBGRAF> (theY, theX);
      aPixel.r() = theColor.r();
      aPixel.g() = theColor.g();
      aPixel.b() = theColor.b();
      aPixel.a() = theColor.a();
      return;
    }
    case Image_Format_RGBF:
    {
      Image_ColorRGBF& aPixel = ChangeValue<Image_ColorRGBF> (theY, theX);
      aPixel.r() = theColor.r();
      aPixel.g() = theColor.g();
      aPixel.b() = theColor.b();
      return;
    }
    case Image_Format_BGRF:
    {
      Image_ColorBGRF& aPixel = ChangeValue<Image_ColorBGRF> (theY, theX);
      aPixel.r() = theColor.r();
      aPixel.g() = theColor.g();
      aPixel.b() = theColor.b();
      return;
    }
    case Image_Format_RGBA:
    {
      Image_ColorRGBA& aPixel = ChangeValue<Image_ColorRGBA> (theY, theX);
      aPixel.r() = Standard_Byte(theColor.r() * 255.0f);
      aPixel.g() = Standard_Byte(theColor.g() * 255.0f);
      aPixel.b() = Standard_Byte(theColor.b() * 255.0f);
      aPixel.a() = Standard_Byte(theColor.a() * 255.0f);
      return;
    }
    case Image_Format_BGRA:
    {
      Image_ColorBGRA& aPixel = ChangeValue<Image_ColorBGRA> (theY, theX);
      aPixel.r() = Standard_Byte(theColor.r() * 255.0f);
      aPixel.g() = Standard_Byte(theColor.g() * 255.0f);
      aPixel.b() = Standard_Byte(theColor.b() * 255.0f);
      aPixel.a() = Standard_Byte(theColor.a() * 255.0f);
      return;
    }
    case Image_Format_RGB32:
    {
      Image_ColorRGB32& aPixel = ChangeValue<Image_ColorRGB32> (theY, theX);
      aPixel.r()  = Standard_Byte(theColor.r() * 255.0f);
      aPixel.g()  = Standard_Byte(theColor.g() * 255.0f);
      aPixel.b()  = Standard_Byte(theColor.b() * 255.0f);
      aPixel.a_() = 255;
      return;
    }
    case Image_Format_BGR32:
    {
      Image_ColorBGR32& aPixel = ChangeValue<Image_ColorBGR32> (theY, theX);
      aPixel.r()  = Standard_Byte(theColor.r() * 255.0f);
      aPixel.g()  = Standard_Byte(theColor.g() * 255.0f);
      aPixel.b()  = Standard_Byte(theColor.b() * 255.0f);
      aPixel.a_() = 255;
      return;
    }
    case Image_Format_RGB:
    {
      Image_ColorRGB& aPixel = ChangeValue<Image_ColorRGB> (theY, theX);
      aPixel.r() = Standard_Byte(theColor.r() * 255.0f);
      aPixel.g() = Standard_Byte(theColor.g() * 255.0f);
      aPixel.b() = Standard_Byte(theColor.b() * 255.0f);
      return;
    }
    case Image_Format_BGR:
    {
      Image_ColorBGR& aPixel = ChangeValue<Image_ColorBGR> (theY, theX);
      aPixel.r() = Standard_Byte(theColor.r() * 255.0f);
      aPixel.g() = Standard_Byte(theColor.g() * 255.0f);
      aPixel.b() = Standard_Byte(theColor.b() * 255.0f);
      return;
    }
    case Image_Format_Gray:
    {
      ChangeValue<Standard_Byte> (theY, theX) = Standard_Byte(theColor.r() * 255.0f);
      return;
    }
    case Image_Format_Alpha:
    {
      ChangeValue<Standard_Byte> (theY, theX) = Standard_Byte(theColor.a() * 255.0f);
      return;
    }
    case Image_Format_UNKNOWN:
    {
      return;
    }
  }
}

// =======================================================================
// function : SwapRgbaBgra
// purpose  :
// =======================================================================
bool Image_PixMap::SwapRgbaBgra (Image_PixMap& theImage)
{
  switch (theImage.Format())
  {
    case Image_Format_BGR32:
    case Image_Format_RGB32:
    case Image_Format_BGRA:
    case Image_Format_RGBA:
    {
      const bool toResetAlpha = theImage.Format() == Image_Format_BGR32
                             || theImage.Format() == Image_Format_RGB32;
      for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
      {
        for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
        {
          Image_ColorRGBA& aPixel     = theImage.ChangeValue<Image_ColorRGBA> (aRow, aCol);
          Image_ColorBGRA  aPixelCopy = theImage.Value      <Image_ColorBGRA> (aRow, aCol);
          aPixel.r() = aPixelCopy.r();
          aPixel.g() = aPixelCopy.g();
          aPixel.b() = aPixelCopy.b();
          if (toResetAlpha)
          {
            aPixel.a() = 255;
          }
        }
      }
      return true;
    }
    case Image_Format_BGR:
    case Image_Format_RGB:
    {
      for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
      {
        for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
        {
          Image_ColorRGB& aPixel     = theImage.ChangeValue<Image_ColorRGB> (aRow, aCol);
          Image_ColorBGR  aPixelCopy = theImage.Value      <Image_ColorBGR> (aRow, aCol);
          aPixel.r() = aPixelCopy.r();
          aPixel.g() = aPixelCopy.g();
          aPixel.b() = aPixelCopy.b();
        }
      }
      return true;
    }
    case Image_Format_BGRF:
    case Image_Format_RGBF:
    case Image_Format_BGRAF:
    case Image_Format_RGBAF:
    {
      for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
      {
        for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
        {
          Image_ColorRGBF& aPixel     = theImage.ChangeValue<Image_ColorRGBF> (aRow, aCol);
          Image_ColorBGRF  aPixelCopy = theImage.Value      <Image_ColorBGRF> (aRow, aCol);
          aPixel.r() = aPixelCopy.r();
          aPixel.g() = aPixelCopy.g();
          aPixel.b() = aPixelCopy.b();
        }
      }
      return true;
    }
    default: return false;
  }
}
