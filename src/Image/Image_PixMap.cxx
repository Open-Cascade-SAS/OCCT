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
Quantity_ColorRGBA Image_PixMap::PixelColor (const Standard_Integer theX,
                                             const Standard_Integer theY) const
{
  if (IsEmpty()
   || theX < 0 || (Standard_Size )theX >= SizeX()
   || theY < 0 || (Standard_Size )theY >= SizeY())
  {
    return Quantity_ColorRGBA (0.0f, 0.0f, 0.0f, 0.0f); // transparent
  }

  switch (myImgFormat)
  {
    case Image_Format_GrayF:
    {
      const Standard_ShortReal& aPixel = Value<Standard_ShortReal> (theY, theX);
      return Quantity_ColorRGBA (NCollection_Vec4<float> (aPixel, aPixel, aPixel, 1.0f)); // opaque
    }
    case Image_Format_AlphaF:
    {
      const Standard_ShortReal& aPixel = Value<Standard_ShortReal> (theY, theX);
      return Quantity_ColorRGBA (NCollection_Vec4<float> (1.0f, 1.0f, 1.0f, aPixel));
    }
    case Image_Format_RGBAF:
    {
      const Image_ColorRGBAF& aPixel = Value<Image_ColorRGBAF> (theY, theX);
      return Quantity_ColorRGBA (NCollection_Vec4<float> (aPixel.r(), aPixel.g(), aPixel.b(), aPixel.a()));
    }
    case Image_Format_BGRAF:
    {    
      const Image_ColorBGRAF& aPixel = Value<Image_ColorBGRAF> (theY, theX);
      return Quantity_ColorRGBA (NCollection_Vec4<float> (aPixel.r(), aPixel.g(), aPixel.b(), aPixel.a()));
    }
    case Image_Format_RGBF:
    {
      const Image_ColorRGBF& aPixel = Value<Image_ColorRGBF> (theY, theX);
      return Quantity_ColorRGBA (NCollection_Vec4<float> (aPixel.r(), aPixel.g(), aPixel.b(), 1.0f)); // opaque
    }
    case Image_Format_BGRF:
    {
      const Image_ColorBGRF& aPixel = Value<Image_ColorBGRF> (theY, theX);
      return Quantity_ColorRGBA (NCollection_Vec4<float> (aPixel.r(), aPixel.g(), aPixel.b(), 1.0f)); // opaque
    }
    case Image_Format_RGBA:
    {
      const Image_ColorRGBA& aPixel = Value<Image_ColorRGBA> (theY, theX);
      return Quantity_ColorRGBA (float(aPixel.r()) / 255.0f, float(aPixel.g()) / 255.0f, float(aPixel.b()) / 255.0f, float(aPixel.a()) / 255.0f);
    }
    case Image_Format_BGRA:
    {
      const Image_ColorBGRA& aPixel = Value<Image_ColorBGRA> (theY, theX);
      return Quantity_ColorRGBA (float(aPixel.r()) / 255.0f, float(aPixel.g()) / 255.0f, float(aPixel.b()) / 255.0f, float(aPixel.a()) / 255.0f);
    }
    case Image_Format_RGB32:
    {
      const Image_ColorRGB32& aPixel = Value<Image_ColorRGB32> (theY, theX);
      return Quantity_ColorRGBA (float(aPixel.r()) / 255.0f, float(aPixel.g()) / 255.0f, float(aPixel.b()) / 255.0f, 1.0f); // opaque
    }
    case Image_Format_BGR32:
    {
      const Image_ColorBGR32& aPixel = Value<Image_ColorBGR32> (theY, theX);
      return Quantity_ColorRGBA (float(aPixel.r()) / 255.0f, float(aPixel.g()) / 255.0f, float(aPixel.b()) / 255.0f, 1.0f); // opaque
    }
    case Image_Format_RGB:
    {
      const Image_ColorRGB& aPixel = Value<Image_ColorRGB> (theY, theX);
      return Quantity_ColorRGBA (float(aPixel.r()) / 255.0f, float(aPixel.g()) / 255.0f, float(aPixel.b()) / 255.0f, 1.0f); // opaque
    }
    case Image_Format_BGR:
    {
      const Image_ColorBGR& aPixel = Value<Image_ColorBGR> (theY, theX);
      return Quantity_ColorRGBA (float(aPixel.r()) / 255.0f, float(aPixel.g()) / 255.0f, float(aPixel.b()) / 255.0f, 1.0f); // opaque
    }
    case Image_Format_Gray:
    {
      const Standard_Byte& aPixel = Value<Standard_Byte> (theY, theX);
      return Quantity_ColorRGBA (float(aPixel) / 255.0f, float(aPixel) / 255.0f, float(aPixel) / 255.0f, 1.0f); // opaque
    }
    case Image_Format_Alpha:
    {
      const Standard_Byte& aPixel = Value<Standard_Byte> (theY, theX);
      return Quantity_ColorRGBA (1.0f, 1.0f, 1.0f, float(aPixel) / 255.0f);
    }
    case Image_Format_UNKNOWN:
    {
      break;
    }
  }

  // unsupported image type
  return Quantity_ColorRGBA (0.0f, 0.0f, 0.0f, 0.0f); // transparent
}

// =======================================================================
// function : SetPixelColor
// purpose  :
// =======================================================================
void Image_PixMap::SetPixelColor (const Standard_Integer theX,
                                  const Standard_Integer theY,
                                  const Quantity_ColorRGBA& theColor)
{
  if (IsEmpty()
   || theX < 0 || Standard_Size(theX) >= SizeX()
   || theY < 0 || Standard_Size(theY) >= SizeY())
  {
    return;
  }

  const NCollection_Vec4<float>& aColor = theColor;
  switch (myImgFormat)
  {
    case Image_Format_GrayF:
    {
      ChangeValue<Standard_ShortReal> (theY, theX) = aColor.r();
      return;
    }
    case Image_Format_AlphaF:
    {
      ChangeValue<Standard_ShortReal> (theY, theX) = aColor.a();
      return;
    }
    case Image_Format_RGBAF:
    {
      Image_ColorRGBAF& aPixel = ChangeValue<Image_ColorRGBAF> (theY, theX);
      aPixel.r() = aColor.r();
      aPixel.g() = aColor.g();
      aPixel.b() = aColor.b();
      aPixel.a() = aColor.a();
      return;
    }
    case Image_Format_BGRAF:
    {
      Image_ColorBGRAF& aPixel = ChangeValue<Image_ColorBGRAF> (theY, theX);
      aPixel.r() = aColor.r();
      aPixel.g() = aColor.g();
      aPixel.b() = aColor.b();
      aPixel.a() = aColor.a();
      return;
    }
    case Image_Format_RGBF:
    {
      Image_ColorRGBF& aPixel = ChangeValue<Image_ColorRGBF> (theY, theX);
      aPixel.r() = aColor.r();
      aPixel.g() = aColor.g();
      aPixel.b() = aColor.b();
      return;
    }
    case Image_Format_BGRF:
    {
      Image_ColorBGRF& aPixel = ChangeValue<Image_ColorBGRF> (theY, theX);
      aPixel.r() = aColor.r();
      aPixel.g() = aColor.g();
      aPixel.b() = aColor.b();
      return;
    }
    case Image_Format_RGBA:
    {
      Image_ColorRGBA& aPixel = ChangeValue<Image_ColorRGBA> (theY, theX);
      aPixel.r() = Standard_Byte(aColor.r() * 255.0f);
      aPixel.g() = Standard_Byte(aColor.g() * 255.0f);
      aPixel.b() = Standard_Byte(aColor.b() * 255.0f);
      aPixel.a() = Standard_Byte(aColor.a() * 255.0f);
      return;
    }
    case Image_Format_BGRA:
    {
      Image_ColorBGRA& aPixel = ChangeValue<Image_ColorBGRA> (theY, theX);
      aPixel.r() = Standard_Byte(aColor.r() * 255.0f);
      aPixel.g() = Standard_Byte(aColor.g() * 255.0f);
      aPixel.b() = Standard_Byte(aColor.b() * 255.0f);
      aPixel.a() = Standard_Byte(aColor.a() * 255.0f);
      return;
    }
    case Image_Format_RGB32:
    {
      Image_ColorRGB32& aPixel = ChangeValue<Image_ColorRGB32> (theY, theX);
      aPixel.r()  = Standard_Byte(aColor.r() * 255.0f);
      aPixel.g()  = Standard_Byte(aColor.g() * 255.0f);
      aPixel.b()  = Standard_Byte(aColor.b() * 255.0f);
      aPixel.a_() = 255;
      return;
    }
    case Image_Format_BGR32:
    {
      Image_ColorBGR32& aPixel = ChangeValue<Image_ColorBGR32> (theY, theX);
      aPixel.r()  = Standard_Byte(aColor.r() * 255.0f);
      aPixel.g()  = Standard_Byte(aColor.g() * 255.0f);
      aPixel.b()  = Standard_Byte(aColor.b() * 255.0f);
      aPixel.a_() = 255;
      return;
    }
    case Image_Format_RGB:
    {
      Image_ColorRGB& aPixel = ChangeValue<Image_ColorRGB> (theY, theX);
      aPixel.r() = Standard_Byte(aColor.r() * 255.0f);
      aPixel.g() = Standard_Byte(aColor.g() * 255.0f);
      aPixel.b() = Standard_Byte(aColor.b() * 255.0f);
      return;
    }
    case Image_Format_BGR:
    {
      Image_ColorBGR& aPixel = ChangeValue<Image_ColorBGR> (theY, theX);
      aPixel.r() = Standard_Byte(aColor.r() * 255.0f);
      aPixel.g() = Standard_Byte(aColor.g() * 255.0f);
      aPixel.b() = Standard_Byte(aColor.b() * 255.0f);
      return;
    }
    case Image_Format_Gray:
    {
      ChangeValue<Standard_Byte> (theY, theX) = Standard_Byte(aColor.r() * 255.0f);
      return;
    }
    case Image_Format_Alpha:
    {
      ChangeValue<Standard_Byte> (theY, theX) = Standard_Byte(aColor.a() * 255.0f);
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

// =======================================================================
// function : ToBlackWhite
// purpose  :
// =======================================================================
void Image_PixMap::ToBlackWhite (Image_PixMap& theImage)
{
  switch (theImage.Format())
  {
    case Image_Format_Gray:
    case Image_Format_Alpha:
    {
      for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
      {
        for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
        {
          unsigned char& aPixel = theImage.ChangeValue<unsigned char> (aRow, aCol);
          if (aPixel != 0)
          {
            aPixel = 255;
          }
        }
      }
      break;
    }
    case Image_Format_RGB:
    case Image_Format_BGR:
    case Image_Format_RGB32:
    case Image_Format_BGR32:
    case Image_Format_RGBA:
    case Image_Format_BGRA:
    {
      const NCollection_Vec3<unsigned char> aWhite24 (255, 255, 255);
      for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
      {
        for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
        {
          NCollection_Vec3<unsigned char>& aPixel = theImage.ChangeValue< NCollection_Vec3<unsigned char> > (aRow, aCol);
          if (aPixel[0] != 0
           || aPixel[1] != 0
           || aPixel[2] != 0)
          {
            aPixel = aWhite24;
          }
        }
      }
      break;
    }
    default:
    {
      const Quantity_ColorRGBA aWhiteRgba (1.0f, 1.0f, 1.0f, 1.0f);
      for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
      {
        for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
        {
          const Quantity_ColorRGBA       aPixelRgba = theImage.PixelColor (Standard_Integer(aCol), Standard_Integer(aRow));
          const NCollection_Vec4<float>& aPixel     = aPixelRgba;
          if (aPixel[0] != 0.0f
           || aPixel[1] != 0.0f
           || aPixel[2] != 0.0f)
          {
            theImage.SetPixelColor (int(aCol), int(aRow), aWhiteRgba);
          }
        }
      }
      break;
    }
  }
}
