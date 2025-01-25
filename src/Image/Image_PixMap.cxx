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

namespace
{
//! Structure defining image pixel format description.
struct Image_FormatInfo
{
  const char*  Name;         //!< string representation
  int          Format;       //!< enumeration name
  unsigned int NbComponents; //!< number of components
  unsigned int PixelSize;    //!< bytes per pixel

  Image_FormatInfo(Image_Format  theFormat,
                   const char*   theName,
                   unsigned int  theNbComponents,
                   Standard_Size thePixelSize)
      : Name(theName),
        Format(theFormat),
        NbComponents(theNbComponents),
        PixelSize((unsigned int)thePixelSize)
  {
  }

  Image_FormatInfo(Image_CompressedFormat theFormat,
                   const char*            theName,
                   unsigned int           theNbComponents,
                   Standard_Size          thePixelSize)
      : Name(theName),
        Format(theFormat),
        NbComponents(theNbComponents),
        PixelSize((unsigned int)thePixelSize)
  {
  }
};

#define ImageFormatInfo(theName, theNbComponents, thePixelSize)                                    \
  Image_FormatInfo(Image_Format_##theName, #theName, theNbComponents, thePixelSize)

#define CompressedImageFormatInfo(theName, theNbComponents, thePixelSize)                          \
  Image_FormatInfo(Image_CompressedFormat_##theName, #theName, theNbComponents, thePixelSize)

//! Table of image pixel formats.
static const Image_FormatInfo Image_Table_ImageFormats[Image_CompressedFormat_NB] = {
  ImageFormatInfo(UNKNOWN, 0, 1),
  ImageFormatInfo(Gray, 1, 1),
  ImageFormatInfo(Alpha, 1, 1),
  ImageFormatInfo(RGB, 3, 3),
  ImageFormatInfo(BGR, 3, 3),
  ImageFormatInfo(RGB32, 3, 4),
  ImageFormatInfo(BGR32, 3, 4),
  ImageFormatInfo(RGBA, 4, 4),
  ImageFormatInfo(BGRA, 4, 4),
  ImageFormatInfo(GrayF, 1, sizeof(float)),
  ImageFormatInfo(AlphaF, 1, sizeof(float)),
  ImageFormatInfo(RGF, 2, sizeof(float) * 2),
  ImageFormatInfo(RGBF, 3, sizeof(float) * 3),
  ImageFormatInfo(BGRF, 3, sizeof(float) * 3),
  ImageFormatInfo(RGBAF, 4, sizeof(float) * 4),
  ImageFormatInfo(BGRAF, 4, sizeof(float) * 4),
  ImageFormatInfo(GrayF_half, 1, sizeof(uint16_t) * 1),
  ImageFormatInfo(RGF_half, 2, sizeof(uint16_t) * 2),
  ImageFormatInfo(RGBAF_half, 4, sizeof(uint16_t) * 4),
  ImageFormatInfo(Gray16, 1, 2),
  CompressedImageFormatInfo(RGB_S3TC_DXT1,
                            3,
                            1), // DXT1 uses circa half a byte per pixel (64 bits per 4x4 block)
  CompressedImageFormatInfo(RGBA_S3TC_DXT1, 4, 1),
  CompressedImageFormatInfo(RGBA_S3TC_DXT3,
                            4,
                            1), // DXT3/5 uses circa 1 byte per pixel (128 bits per 4x4 block)
  CompressedImageFormatInfo(RGBA_S3TC_DXT5, 4, 1)};
} // namespace

IMPLEMENT_STANDARD_RTTIEXT(Image_PixMapData, NCollection_Buffer)
IMPLEMENT_STANDARD_RTTIEXT(Image_PixMap, Standard_Transient)

// =======================================================================
// function : DefaultAllocator
// purpose  :
// =======================================================================
const Handle(NCollection_BaseAllocator)& Image_PixMap::DefaultAllocator()
{
  static const Handle(NCollection_BaseAllocator) THE_ALLOC = new NCollection_AlignedAllocator(16);
  return THE_ALLOC;
}

// =======================================================================
// function : ImageFormatToString
// purpose  :
// =======================================================================
Standard_CString Image_PixMap::ImageFormatToString(Image_Format theFormat)
{
  return Image_Table_ImageFormats[theFormat].Name;
}

// =======================================================================
// function : ImageFormatToString
// purpose  :
// =======================================================================
Standard_CString Image_PixMap::ImageFormatToString(Image_CompressedFormat theFormat)
{
  return Image_Table_ImageFormats[theFormat].Name;
}

// =======================================================================
// function : Image_PixMap
// purpose  :
// =======================================================================
Image_PixMap::Image_PixMap()
    : myImgFormat(Image_Format_Gray)
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

// =======================================================================
// function : SizePixelBytes
// purpose  :
// =======================================================================
Standard_Size Image_PixMap::SizePixelBytes(const Image_Format thePixelFormat)
{
  return Image_Table_ImageFormats[thePixelFormat].PixelSize;
}

// =======================================================================
// function : SetFormat
// purpose  :
// =======================================================================
void Image_PixMap::SetFormat(Image_Format thePixelFormat)
{
  if (myImgFormat == thePixelFormat)
  {
    return;
  }

  if (!IsEmpty() && SizePixelBytes(myImgFormat) != SizePixelBytes(thePixelFormat))
  {
    throw Standard_ProgramError("Image_PixMap::SetFormat() - incompatible pixel format");
    return;
  }

  myImgFormat = thePixelFormat;
}

// =======================================================================
// function : InitWrapper3D
// purpose  :
// =======================================================================
bool Image_PixMap::InitWrapper3D(Image_Format                           thePixelFormat,
                                 Standard_Byte*                         theDataPtr,
                                 const NCollection_Vec3<Standard_Size>& theSizeXYZ,
                                 const Standard_Size                    theSizeRowBytes)
{
  Clear();
  myImgFormat = thePixelFormat;
  if (theSizeXYZ.x() == 0 || theSizeXYZ.y() == 0 || theSizeXYZ.z() == 0 || theDataPtr == nullptr)
  {
    return false;
  }

  Handle(NCollection_BaseAllocator) anEmptyAlloc;
  myData.Init(anEmptyAlloc,
              Image_PixMap::SizePixelBytes(thePixelFormat),
              theSizeXYZ,
              theSizeRowBytes,
              theDataPtr);
  return true;
}

// =======================================================================
// function : InitWrapper
// purpose  :
// =======================================================================
bool Image_PixMap::InitWrapper(Image_Format        thePixelFormat,
                               Standard_Byte*      theDataPtr,
                               const Standard_Size theSizeX,
                               const Standard_Size theSizeY,
                               const Standard_Size theSizeRowBytes)
{
  return InitWrapper3D(thePixelFormat,
                       theDataPtr,
                       NCollection_Vec3<Standard_Size>(theSizeX, theSizeY, 1),
                       theSizeRowBytes);
}

// =======================================================================
// function : InitTrash3D
// purpose  :
// =======================================================================
bool Image_PixMap::InitTrash3D(Image_Format                           thePixelFormat,
                               const NCollection_Vec3<Standard_Size>& theSizeXYZ,
                               const Standard_Size                    theSizeRowBytes)
{
  Clear();
  myImgFormat = thePixelFormat;
  if (theSizeXYZ.x() == 0 || theSizeXYZ.y() == 0 || theSizeXYZ.z() == 0)
  {
    return false;
  }

  // use argument only if it greater
  const Standard_Size aSizeRowBytes =
    std::max(theSizeRowBytes, theSizeXYZ.x() * SizePixelBytes(thePixelFormat));
  myData.Init(DefaultAllocator(),
              Image_PixMap::SizePixelBytes(thePixelFormat),
              theSizeXYZ,
              aSizeRowBytes,
              NULL);
  return !myData.IsEmpty();
}

// =======================================================================
// function : InitTrash
// purpose  :
// =======================================================================
bool Image_PixMap::InitTrash(Image_Format        thePixelFormat,
                             const Standard_Size theSizeX,
                             const Standard_Size theSizeY,
                             const Standard_Size theSizeRowBytes)
{
  return InitTrash3D(thePixelFormat,
                     NCollection_Vec3<Standard_Size>(theSizeX, theSizeY, 1),
                     theSizeRowBytes);
}

// =======================================================================
// function : InitZero3D
// purpose  :
// =======================================================================
bool Image_PixMap::InitZero3D(Image_Format                           thePixelFormat,
                              const NCollection_Vec3<Standard_Size>& theSizeXYZ,
                              const Standard_Size                    theSizeRowBytes,
                              const Standard_Byte                    theValue)
{
  if (theSizeXYZ.z() > 1)
  {
    if (!InitTrash3D(thePixelFormat, theSizeXYZ, theSizeRowBytes))
    {
      return false;
    }
  }
  else
  {
    if (!InitTrash(thePixelFormat, theSizeXYZ.x(), theSizeXYZ.y(), theSizeRowBytes))
    {
      return false;
    }
  }
  memset(myData.ChangeData(), (int)theValue, SizeBytes());
  return true;
}

// =======================================================================
// function : InitCopy
// purpose  :
// =======================================================================
bool Image_PixMap::InitCopy(const Image_PixMap& theCopy)
{
  if (&theCopy == this)
  {
    // self-copying disallowed
    return false;
  }

  if (theCopy.SizeZ() > 1)
  {
    if (!InitTrash3D(theCopy.myImgFormat, theCopy.SizeXYZ(), theCopy.SizeRowBytes()))
    {
      return false;
    }
  }
  else
  {
    if (!InitTrash(theCopy.myImgFormat, theCopy.SizeX(), theCopy.SizeY(), theCopy.SizeRowBytes()))
    {
      return false;
    }
  }

  memcpy(myData.ChangeData(), theCopy.myData.Data(), theCopy.SizeBytes());
  return true;
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Image_PixMap::Clear()
{
  Handle(NCollection_BaseAllocator) anEmptyAlloc;
  myData.Init(anEmptyAlloc,
              Image_PixMap::SizePixelBytes(myImgFormat),
              NCollection_Vec3<Standard_Size>(0),
              0,
              nullptr);
}

// =======================================================================
// function : ColorFromRawPixel
// purpose  :
// =======================================================================
Quantity_ColorRGBA Image_PixMap::ColorFromRawPixel(const Standard_Byte*   theRawValue,
                                                   const Image_Format     theFormat,
                                                   const Standard_Boolean theToLinearize)
{
  switch (theFormat)
  {
    case Image_Format_GrayF: {
      const Standard_ShortReal& aPixel = *reinterpret_cast<const Standard_ShortReal*>(theRawValue);
      // clang-format off
      return Quantity_ColorRGBA (NCollection_Vec4<float> (aPixel, aPixel, aPixel, 1.0f)); // opaque
      // clang-format on
    }
    case Image_Format_AlphaF: {
      const Standard_ShortReal& aPixel = *reinterpret_cast<const Standard_ShortReal*>(theRawValue);
      return Quantity_ColorRGBA(NCollection_Vec4<float>(1.0f, 1.0f, 1.0f, aPixel));
    }
    case Image_Format_RGF: {
      const Image_ColorRGF& aPixel = *reinterpret_cast<const Image_ColorRGF*>(theRawValue);
      return Quantity_ColorRGBA(NCollection_Vec4<float>(aPixel.r(), aPixel.g(), 0.0f, 1.0f));
    }
    case Image_Format_RGBAF: {
      const Image_ColorRGBAF& aPixel = *reinterpret_cast<const Image_ColorRGBAF*>(theRawValue);
      return Quantity_ColorRGBA(
        NCollection_Vec4<float>(aPixel.r(), aPixel.g(), aPixel.b(), aPixel.a()));
    }
    case Image_Format_BGRAF: {
      const Image_ColorBGRAF& aPixel = *reinterpret_cast<const Image_ColorBGRAF*>(theRawValue);
      return Quantity_ColorRGBA(
        NCollection_Vec4<float>(aPixel.r(), aPixel.g(), aPixel.b(), aPixel.a()));
    }
    case Image_Format_RGBF: {
      const Image_ColorRGBF& aPixel = *reinterpret_cast<const Image_ColorRGBF*>(theRawValue);
      // clang-format off
      return Quantity_ColorRGBA (NCollection_Vec4<float> (aPixel.r(), aPixel.g(), aPixel.b(), 1.0f)); // opaque
    }
    case Image_Format_BGRF:
    {
      const Image_ColorBGRF& aPixel = *reinterpret_cast<const Image_ColorBGRF*> (theRawValue);
      return Quantity_ColorRGBA (NCollection_Vec4<float> (aPixel.r(), aPixel.g(), aPixel.b(), 1.0f)); // opaque
      // clang-format on
    }
    case Image_Format_GrayF_half: {
      const uint16_t& aPixel = *reinterpret_cast<const uint16_t*>(theRawValue);
      return Quantity_ColorRGBA(
        NCollection_Vec4<float>(ConvertFromHalfFloat(aPixel), 0.0f, 0.0f, 1.0f));
    }
    case Image_Format_RGF_half: {
      const NCollection_Vec2<uint16_t>& aPixel =
        *reinterpret_cast<const NCollection_Vec2<uint16_t>*>(theRawValue);
      return Quantity_ColorRGBA(NCollection_Vec4<float>(ConvertFromHalfFloat(aPixel.x()),
                                                        ConvertFromHalfFloat(aPixel.y()),
                                                        0.0f,
                                                        1.0f));
    }
    case Image_Format_RGBAF_half: {
      const NCollection_Vec4<uint16_t>& aPixel =
        *reinterpret_cast<const NCollection_Vec4<uint16_t>*>(theRawValue);
      return Quantity_ColorRGBA(NCollection_Vec4<float>(ConvertFromHalfFloat(aPixel.r()),
                                                        ConvertFromHalfFloat(aPixel.g()),
                                                        ConvertFromHalfFloat(aPixel.b()),
                                                        ConvertFromHalfFloat(aPixel.a())));
    }
    case Image_Format_RGBA: {
      const Image_ColorRGBA& aPixel = *reinterpret_cast<const Image_ColorRGBA*>(theRawValue);
      return theToLinearize
               ? Quantity_ColorRGBA(
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.r()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.g()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.b()) / 255.0f),
                   float(aPixel.a()) / 255.0f)
               : Quantity_ColorRGBA(float(aPixel.r()) / 255.0f,
                                    float(aPixel.g()) / 255.0f,
                                    float(aPixel.b()) / 255.0f,
                                    float(aPixel.a()) / 255.0f);
    }
    case Image_Format_BGRA: {
      const Image_ColorBGRA& aPixel = *reinterpret_cast<const Image_ColorBGRA*>(theRawValue);
      return theToLinearize
               ? Quantity_ColorRGBA(
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.r()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.g()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.b()) / 255.0f),
                   float(aPixel.a()) / 255.0f)
               : Quantity_ColorRGBA(float(aPixel.r()) / 255.0f,
                                    float(aPixel.g()) / 255.0f,
                                    float(aPixel.b()) / 255.0f,
                                    float(aPixel.a()) / 255.0f);
    }
    case Image_Format_RGB32: {
      const Image_ColorRGB32& aPixel = *reinterpret_cast<const Image_ColorRGB32*>(theRawValue);
      return theToLinearize
               ? Quantity_ColorRGBA(
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.r()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.g()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.b()) / 255.0f),
                   1.0f)
               : Quantity_ColorRGBA(float(aPixel.r()) / 255.0f,
                                    float(aPixel.g()) / 255.0f,
                                    float(aPixel.b()) / 255.0f,
                                    1.0f);
    }
    case Image_Format_BGR32: {
      const Image_ColorBGR32& aPixel = *reinterpret_cast<const Image_ColorBGR32*>(theRawValue);
      return theToLinearize
               ? Quantity_ColorRGBA(
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.r()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.g()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.b()) / 255.0f),
                   1.0f)
               : Quantity_ColorRGBA(float(aPixel.r()) / 255.0f,
                                    float(aPixel.g()) / 255.0f,
                                    float(aPixel.b()) / 255.0f,
                                    1.0f);
    }
    case Image_Format_RGB: {
      const Image_ColorRGB& aPixel = *reinterpret_cast<const Image_ColorRGB*>(theRawValue);
      return theToLinearize
               ? Quantity_ColorRGBA(
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.r()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.g()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.b()) / 255.0f),
                   1.0f)
               : Quantity_ColorRGBA(float(aPixel.r()) / 255.0f,
                                    float(aPixel.g()) / 255.0f,
                                    float(aPixel.b()) / 255.0f,
                                    1.0f);
    }
    case Image_Format_BGR: {
      const Image_ColorBGR& aPixel = *reinterpret_cast<const Image_ColorBGR*>(theRawValue);
      return theToLinearize
               ? Quantity_ColorRGBA(
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.r()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.g()) / 255.0f),
                   Quantity_Color::Convert_sRGB_To_LinearRGB(float(aPixel.b()) / 255.0f),
                   1.0f)
               : Quantity_ColorRGBA(float(aPixel.r()) / 255.0f,
                                    float(aPixel.g()) / 255.0f,
                                    float(aPixel.b()) / 255.0f,
                                    1.0f);
    }
    case Image_Format_Gray: {
      const Standard_Byte& aPixel      = *reinterpret_cast<const Standard_Byte*>(theRawValue);
      const float          anIntensity = float(aPixel) / 255.0f;
      return Quantity_ColorRGBA(anIntensity, anIntensity, anIntensity, 1.0f); // opaque
    }
    case Image_Format_Alpha: {
      const Standard_Byte& aPixel = *reinterpret_cast<const Standard_Byte*>(theRawValue);
      return Quantity_ColorRGBA(1.0f, 1.0f, 1.0f, float(aPixel) / 255.0f);
    }
    case Image_Format_Gray16: {
      const uint16_t& aPixel      = *reinterpret_cast<const uint16_t*>(theRawValue);
      const float     anIntensity = float(aPixel) / 65535.0f;
      return Quantity_ColorRGBA(anIntensity, anIntensity, anIntensity, 1.0f); // opaque
    }
    case Image_Format_UNKNOWN: {
      break;
    }
  }

  // unsupported image type
  return Quantity_ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f); // transparent
}

// =======================================================================
// function : ColorToRawPixel
// purpose  :
// =======================================================================
void Image_PixMap::ColorToRawPixel(Standard_Byte*            theRawValue,
                                   const Image_Format        theFormat,
                                   const Quantity_ColorRGBA& theColor,
                                   const Standard_Boolean    theToDeLinearize)
{
  const NCollection_Vec4<float>& aColor = theColor;
  switch (theFormat)
  {
    case Image_Format_GrayF: {
      *reinterpret_cast<Standard_ShortReal*>(theRawValue) = aColor.r();
      return;
    }
    case Image_Format_AlphaF: {
      *reinterpret_cast<Standard_ShortReal*>(theRawValue) = aColor.a();
      return;
    }
    case Image_Format_RGF: {
      Image_ColorRGF& aPixel = *reinterpret_cast<Image_ColorRGF*>(theRawValue);
      aPixel.r()             = aColor.r();
      aPixel.g()             = aColor.g();
      return;
    }
    case Image_Format_RGBAF: {
      Image_ColorRGBAF& aPixel = *reinterpret_cast<Image_ColorRGBAF*>(theRawValue);
      aPixel.r()               = aColor.r();
      aPixel.g()               = aColor.g();
      aPixel.b()               = aColor.b();
      aPixel.a()               = aColor.a();
      return;
    }
    case Image_Format_BGRAF: {
      Image_ColorBGRAF& aPixel = *reinterpret_cast<Image_ColorBGRAF*>(theRawValue);
      aPixel.r()               = aColor.r();
      aPixel.g()               = aColor.g();
      aPixel.b()               = aColor.b();
      aPixel.a()               = aColor.a();
      return;
    }
    case Image_Format_RGBF: {
      Image_ColorRGBF& aPixel = *reinterpret_cast<Image_ColorRGBF*>(theRawValue);
      aPixel.r()              = aColor.r();
      aPixel.g()              = aColor.g();
      aPixel.b()              = aColor.b();
      return;
    }
    case Image_Format_BGRF: {
      Image_ColorBGRF& aPixel = *reinterpret_cast<Image_ColorBGRF*>(theRawValue);
      aPixel.r()              = aColor.r();
      aPixel.g()              = aColor.g();
      aPixel.b()              = aColor.b();
      return;
    }
    case Image_Format_GrayF_half: {
      uint16_t& aPixel = *reinterpret_cast<uint16_t*>(theRawValue);
      aPixel           = ConvertToHalfFloat(aColor.r());
      return;
    }
    case Image_Format_RGF_half: {
      NCollection_Vec2<uint16_t>& aPixel =
        *reinterpret_cast<NCollection_Vec2<uint16_t>*>(theRawValue);
      aPixel.x() = ConvertToHalfFloat(aColor.r());
      aPixel.y() = ConvertToHalfFloat(aColor.g());
      return;
    }
    case Image_Format_RGBAF_half: {
      NCollection_Vec4<uint16_t>& aPixel =
        *reinterpret_cast<NCollection_Vec4<uint16_t>*>(theRawValue);
      aPixel.r() = ConvertToHalfFloat(aColor.r());
      aPixel.g() = ConvertToHalfFloat(aColor.g());
      aPixel.b() = ConvertToHalfFloat(aColor.b());
      aPixel.a() = ConvertToHalfFloat(aColor.a());
      return;
    }
    case Image_Format_RGBA: {
      Image_ColorRGBA& aPixel = *reinterpret_cast<Image_ColorRGBA*>(theRawValue);
      if (theToDeLinearize)
      {
        aPixel.r() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.r()) * 255.0f);
        aPixel.g() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.g()) * 255.0f);
        aPixel.b() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.b()) * 255.0f);
      }
      else
      {
        aPixel.r() = Standard_Byte(aColor.r() * 255.0f);
        aPixel.g() = Standard_Byte(aColor.g() * 255.0f);
        aPixel.b() = Standard_Byte(aColor.b() * 255.0f);
      }
      aPixel.a() = Standard_Byte(aColor.a() * 255.0f);
      return;
    }
    case Image_Format_BGRA: {
      Image_ColorBGRA& aPixel = *reinterpret_cast<Image_ColorBGRA*>(theRawValue);
      if (theToDeLinearize)
      {
        aPixel.r() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.r()) * 255.0f);
        aPixel.g() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.g()) * 255.0f);
        aPixel.b() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.b()) * 255.0f);
      }
      else
      {
        aPixel.r() = Standard_Byte(aColor.r() * 255.0f);
        aPixel.g() = Standard_Byte(aColor.g() * 255.0f);
        aPixel.b() = Standard_Byte(aColor.b() * 255.0f);
      }
      aPixel.a() = Standard_Byte(aColor.a() * 255.0f);
      return;
    }
    case Image_Format_RGB32: {
      Image_ColorRGB32& aPixel = *reinterpret_cast<Image_ColorRGB32*>(theRawValue);
      if (theToDeLinearize)
      {
        aPixel.r() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.r()) * 255.0f);
        aPixel.g() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.g()) * 255.0f);
        aPixel.b() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.b()) * 255.0f);
      }
      else
      {
        aPixel.r() = Standard_Byte(aColor.r() * 255.0f);
        aPixel.g() = Standard_Byte(aColor.g() * 255.0f);
        aPixel.b() = Standard_Byte(aColor.b() * 255.0f);
      }
      aPixel.a_() = 255;
      return;
    }
    case Image_Format_BGR32: {
      Image_ColorBGR32& aPixel = *reinterpret_cast<Image_ColorBGR32*>(theRawValue);
      if (theToDeLinearize)
      {
        aPixel.r() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.r()) * 255.0f);
        aPixel.g() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.g()) * 255.0f);
        aPixel.b() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.b()) * 255.0f);
      }
      else
      {
        aPixel.r() = Standard_Byte(aColor.r() * 255.0f);
        aPixel.g() = Standard_Byte(aColor.g() * 255.0f);
        aPixel.b() = Standard_Byte(aColor.b() * 255.0f);
      }
      aPixel.a_() = 255;
      return;
    }
    case Image_Format_RGB: {
      Image_ColorRGB& aPixel = *reinterpret_cast<Image_ColorRGB*>(theRawValue);
      if (theToDeLinearize)
      {
        aPixel.r() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.r()) * 255.0f);
        aPixel.g() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.g()) * 255.0f);
        aPixel.b() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.b()) * 255.0f);
      }
      else
      {
        aPixel.r() = Standard_Byte(aColor.r() * 255.0f);
        aPixel.g() = Standard_Byte(aColor.g() * 255.0f);
        aPixel.b() = Standard_Byte(aColor.b() * 255.0f);
      }
      return;
    }
    case Image_Format_BGR: {
      Image_ColorBGR& aPixel = *reinterpret_cast<Image_ColorBGR*>(theRawValue);
      if (theToDeLinearize)
      {
        aPixel.r() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.r()) * 255.0f);
        aPixel.g() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.g()) * 255.0f);
        aPixel.b() = Standard_Byte(Quantity_Color::Convert_LinearRGB_To_sRGB(aColor.b()) * 255.0f);
      }
      else
      {
        aPixel.r() = Standard_Byte(aColor.r() * 255.0f);
        aPixel.g() = Standard_Byte(aColor.g() * 255.0f);
        aPixel.b() = Standard_Byte(aColor.b() * 255.0f);
      }
      return;
    }
    case Image_Format_Gray: {
      *reinterpret_cast<Standard_Byte*>(theRawValue) = Standard_Byte(aColor.r() * 255.0f);
      return;
    }
    case Image_Format_Alpha: {
      *reinterpret_cast<Standard_Byte*>(theRawValue) = Standard_Byte(aColor.a() * 255.0f);
      return;
    }
    case Image_Format_Gray16: {
      *reinterpret_cast<uint16_t*>(theRawValue) = uint16_t(aColor.r() * 65535.0f);
      return;
    }
    case Image_Format_UNKNOWN: {
      return;
    }
  }
}

// =======================================================================
// function : SwapRgbaBgra
// purpose  :
// =======================================================================
bool Image_PixMap::SwapRgbaBgra(Image_PixMap& theImage)
{
  switch (theImage.Format())
  {
    case Image_Format_BGR32:
    case Image_Format_RGB32:
    case Image_Format_BGRA:
    case Image_Format_RGBA: {
      const bool toResetAlpha =
        theImage.Format() == Image_Format_BGR32 || theImage.Format() == Image_Format_RGB32;
      for (Standard_Size aSlice = 0; aSlice < theImage.SizeZ(); ++aSlice)
      {
        for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
          {
            Image_ColorRGBA& aPixel = theImage.ChangeValueXYZ<Image_ColorRGBA>(aCol, aRow, aSlice);
            Image_ColorBGRA  aPixelCopy = theImage.ValueXYZ<Image_ColorBGRA>(aCol, aRow, aSlice);
            aPixel.r()                  = aPixelCopy.r();
            aPixel.g()                  = aPixelCopy.g();
            aPixel.b()                  = aPixelCopy.b();
            if (toResetAlpha)
            {
              aPixel.a() = 255;
            }
          }
        }
      }
      return true;
    }
    case Image_Format_BGR:
    case Image_Format_RGB: {
      for (Standard_Size aSlice = 0; aSlice < theImage.SizeZ(); ++aSlice)
      {
        for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
          {
            Image_ColorRGB& aPixel = theImage.ChangeValueXYZ<Image_ColorRGB>(aCol, aRow, aSlice);
            Image_ColorBGR  aPixelCopy = theImage.ValueXYZ<Image_ColorBGR>(aCol, aRow, aSlice);
            aPixel.r()                 = aPixelCopy.r();
            aPixel.g()                 = aPixelCopy.g();
            aPixel.b()                 = aPixelCopy.b();
          }
        }
      }
      return true;
    }
    case Image_Format_BGRF:
    case Image_Format_RGBF:
    case Image_Format_BGRAF:
    case Image_Format_RGBAF: {
      for (Standard_Size aSlice = 0; aSlice < theImage.SizeZ(); ++aSlice)
      {
        for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
          {
            Image_ColorRGBF& aPixel = theImage.ChangeValueXYZ<Image_ColorRGBF>(aCol, aRow, aSlice);
            Image_ColorBGRF  aPixelCopy = theImage.ValueXYZ<Image_ColorBGRF>(aCol, aRow, aSlice);
            aPixel.r()                  = aPixelCopy.r();
            aPixel.g()                  = aPixelCopy.g();
            aPixel.b()                  = aPixelCopy.b();
          }
        }
      }
      return true;
    }
    default:
      return false;
  }
}

// =======================================================================
// function : ToBlackWhite
// purpose  :
// =======================================================================
void Image_PixMap::ToBlackWhite(Image_PixMap& theImage)
{
  switch (theImage.Format())
  {
    case Image_Format_Gray:
    case Image_Format_Alpha: {
      for (Standard_Size aSlice = 0; aSlice < theImage.SizeZ(); ++aSlice)
      {
        for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
          {
            uint8_t& aPixel = theImage.ChangeValueXYZ<uint8_t>(aCol, aRow, aSlice);
            if (aPixel != 0)
            {
              aPixel = 255;
            }
          }
        }
      }
      break;
    }
    case Image_Format_Gray16: {
      for (Standard_Size aSlice = 0; aSlice < theImage.SizeZ(); ++aSlice)
      {
        for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
          {
            uint16_t& aPixel = theImage.ChangeValueXYZ<uint16_t>(aCol, aRow, aSlice);
            if (aPixel != 0)
            {
              aPixel = 65535;
            }
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
    case Image_Format_BGRA: {
      const NCollection_Vec3<uint8_t> aWhite24(255, 255, 255);
      for (Standard_Size aSlice = 0; aSlice < theImage.SizeZ(); ++aSlice)
      {
        for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
          {
            NCollection_Vec3<uint8_t>& aPixel =
              theImage.ChangeValueXYZ<NCollection_Vec3<uint8_t>>(aCol, aRow, aSlice);
            if (aPixel[0] != 0 || aPixel[1] != 0 || aPixel[2] != 0)
            {
              aPixel = aWhite24;
            }
          }
        }
      }
      break;
    }
    default: {
      const Quantity_ColorRGBA aWhiteRgba(1.0f, 1.0f, 1.0f, 1.0f);
      for (Standard_Size aSlice = 0; aSlice < theImage.SizeZ(); ++aSlice)
      {
        for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
          {
            Standard_Byte*           aRawPixel    = theImage.ChangeRawValueXYZ(aCol, aRow, aSlice);
            const Quantity_ColorRGBA aPixelRgba   = ColorFromRawPixel(aRawPixel, theImage.Format());
            const NCollection_Vec4<float>& aPixel = aPixelRgba;
            if (aPixel[0] != 0.0f || aPixel[1] != 0.0f || aPixel[2] != 0.0f)
            {
              ColorToRawPixel(aRawPixel, theImage.Format(), aWhiteRgba);
            }
          }
        }
      }
      break;
    }
  }
}

// =======================================================================
// function : FlipY
// purpose  :
// =======================================================================
bool Image_PixMap::FlipY(Image_PixMap& theImage)
{
  if (theImage.IsEmpty() || theImage.SizeX() == 0 || theImage.SizeY() == 0)
  {
    return false;
  }

  NCollection_Buffer aTmp(NCollection_BaseAllocator::CommonBaseAllocator());
  const size_t       aRowSize = theImage.SizeRowBytes();
  if (!aTmp.Allocate(aRowSize))
  {
    return false;
  }

  // for odd height middle row should be left as is
  Standard_Size aNbRowsHalf = theImage.SizeY() / 2;
  for (Standard_Size aSlice = 0; aSlice < theImage.SizeZ(); ++aSlice)
  {
    for (Standard_Size aRowT = 0, aRowB = theImage.SizeY() - 1; aRowT < aNbRowsHalf;
         ++aRowT, --aRowB)
    {
      Standard_Byte* aTop = theImage.ChangeSliceRow(aSlice, aRowT);
      Standard_Byte* aBot = theImage.ChangeSliceRow(aSlice, aRowB);
      memcpy(aTmp.ChangeData(), aTop, aRowSize);
      memcpy(aTop, aBot, aRowSize);
      memcpy(aBot, aTmp.Data(), aRowSize);
    }
  }
  return true;
}
