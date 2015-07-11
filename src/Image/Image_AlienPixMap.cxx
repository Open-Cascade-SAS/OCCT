// Created on: 2010-09-16
// Created by: KGV
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

#ifdef HAVE_FREEIMAGE
  #include <FreeImage.h>

  #ifdef _MSC_VER
    #pragma comment( lib, "FreeImage.lib" )
  #endif
#endif

#include <Image_AlienPixMap.hxx>
#include <gp.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_OpenFile.hxx>
#include <fstream>
#include <algorithm>

#ifdef HAVE_FREEIMAGE
namespace
{
  static Image_PixMap::ImgFormat convertFromFreeFormat (FREE_IMAGE_TYPE       theFormatFI,
                                                        FREE_IMAGE_COLOR_TYPE theColorTypeFI,
                                                        unsigned              theBitsPerPixel)
  {
    switch (theFormatFI)
    {
      case FIT_RGBF:   return Image_PixMap::ImgRGBF;
      case FIT_RGBAF:  return Image_PixMap::ImgRGBAF;
      case FIT_FLOAT:  return Image_PixMap::ImgGrayF;
      case FIT_BITMAP:
      {
        switch (theColorTypeFI)
        {
          case FIC_MINISBLACK:
          {
            return Image_PixMap::ImgGray;
          }
          case FIC_RGB:
          {
            if (Image_PixMap::IsBigEndianHost())
            {
              return (theBitsPerPixel == 32) ? Image_PixMap::ImgRGB32 : Image_PixMap::ImgRGB;
            }
            else
            {
              return (theBitsPerPixel == 32) ? Image_PixMap::ImgBGR32 : Image_PixMap::ImgBGR;
            }
          }
          case FIC_RGBALPHA:
          {
            return Image_PixMap::IsBigEndianHost() ? Image_PixMap::ImgRGBA : Image_PixMap::ImgBGRA;
          }
          default:
            return Image_PixMap::ImgUNKNOWN;
        }
      }
      default:
        return Image_PixMap::ImgUNKNOWN;
    }
  }

  static FREE_IMAGE_TYPE convertToFreeFormat (Image_PixMap::ImgFormat theFormat)
  {
    switch (theFormat)
    {
      case Image_PixMap::ImgGrayF:
      case Image_PixMap::ImgAlphaF:
        return FIT_FLOAT;
      case Image_PixMap::ImgRGBAF:
        return FIT_RGBAF;
      case Image_PixMap::ImgRGBF:
        return FIT_RGBF;
      case Image_PixMap::ImgRGBA:
      case Image_PixMap::ImgBGRA:
      case Image_PixMap::ImgRGB32:
      case Image_PixMap::ImgBGR32:
      case Image_PixMap::ImgRGB:
      case Image_PixMap::ImgBGR:
      case Image_PixMap::ImgGray:
      case Image_PixMap::ImgAlpha:
        return FIT_BITMAP;
      default:
        return FIT_UNKNOWN;
    }
  }
};
#endif


// =======================================================================
// function : Image_AlienPixMap
// purpose  :
// =======================================================================
Image_AlienPixMap::Image_AlienPixMap()
: myLibImage (NULL)
{
  SetTopDown (false);
}

// =======================================================================
// function : ~Image_AlienPixMap
// purpose  :
// =======================================================================
Image_AlienPixMap::~Image_AlienPixMap()
{
  Clear();
}

// =======================================================================
// function : InitWrapper
// purpose  :
// =======================================================================
bool Image_AlienPixMap::InitWrapper (ImgFormat,
                                     Standard_Byte*,
                                     const Standard_Size,
                                     const Standard_Size,
                                     const Standard_Size)
{
  Clear();
  return false;
}

// =======================================================================
// function : InitTrash
// purpose  :
// =======================================================================
#ifdef HAVE_FREEIMAGE
bool Image_AlienPixMap::InitTrash (ImgFormat           thePixelFormat,
                                   const Standard_Size theSizeX,
                                   const Standard_Size theSizeY,
                                   const Standard_Size /*theSizeRowBytes*/)
{
  Clear();
  FREE_IMAGE_TYPE aFormatFI = convertToFreeFormat (thePixelFormat);
  int aBitsPerPixel = (int )Image_PixMap::SizePixelBytes (thePixelFormat) * 8;
  if (aFormatFI == FIT_UNKNOWN)
  {
    aFormatFI     = FIT_BITMAP;
    aBitsPerPixel = 24;
  }

  FIBITMAP* anImage = FreeImage_AllocateT (aFormatFI, (int )theSizeX, (int )theSizeY, aBitsPerPixel);
  Image_PixMap::ImgFormat aFormat = convertFromFreeFormat (FreeImage_GetImageType (anImage),
                                                           FreeImage_GetColorType (anImage),
                                                           FreeImage_GetBPP (anImage));
  if (thePixelFormat == Image_PixMap::ImgBGR32
   || thePixelFormat == Image_PixMap::ImgRGB32)
  {
    //FreeImage_SetTransparent (anImage, FALSE);
    aFormat = (aFormat == Image_PixMap::ImgBGRA) ? Image_PixMap::ImgBGR32 : Image_PixMap::ImgRGB32;
  }

  Image_PixMap::InitWrapper (aFormat, FreeImage_GetBits (anImage),
                             FreeImage_GetWidth (anImage), FreeImage_GetHeight (anImage), FreeImage_GetPitch (anImage));
  SetTopDown (false);

  // assign image after wrapper initialization (virtual Clear() called inside)
  myLibImage = anImage;
  return true;
}
#else
bool Image_AlienPixMap::InitTrash (ImgFormat           thePixelFormat,
                                   const Standard_Size theSizeX,
                                   const Standard_Size theSizeY,
                                   const Standard_Size theSizeRowBytes)
{
  return Image_PixMap::InitTrash (thePixelFormat, theSizeX, theSizeY, theSizeRowBytes);
}
#endif

// =======================================================================
// function : InitCopy
// purpose  :
// =======================================================================
bool Image_AlienPixMap::InitCopy (const Image_PixMap& theCopy)
{
  if (&theCopy == this)
  {
    // self-copying disallowed
    return false;
  }
  if (!InitTrash (theCopy.Format(), theCopy.SizeX(), theCopy.SizeY(), theCopy.SizeRowBytes()))
  {
    return false;
  }

  if (myImgFormat == theCopy.Format())
  {
    if (SizeRowBytes() == theCopy.SizeRowBytes()
     && TopDownInc()   == theCopy.TopDownInc())
    {
      // copy with one call
      memcpy (ChangeData(), theCopy.Data(), std::min (SizeBytes(), theCopy.SizeBytes()));
      return true;
    }

    // copy row-by-row
    const Standard_Size aRowSizeBytes = std::min (SizeRowBytes(), theCopy.SizeRowBytes());
    for (Standard_Size aRow = 0; aRow < myData.SizeY; ++aRow)
    {
      memcpy (ChangeRow (aRow), theCopy.Row (aRow), aRowSizeBytes);
    }
    return true;
  }

  // pixel format conversion required
  Clear();
  return false;
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Image_AlienPixMap::Clear()
{
  Image_PixMap::Clear();
#ifdef HAVE_FREEIMAGE
  if (myLibImage != NULL)
  {
    FreeImage_Unload (myLibImage);
    myLibImage = NULL;
  }
#endif
}

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
#ifdef HAVE_FREEIMAGE
bool Image_AlienPixMap::Load (const TCollection_AsciiString& theImagePath)
{
  Clear();

#ifdef _WIN32
  const TCollection_ExtendedString aFileNameW (theImagePath.ToCString(), Standard_True);
  FREE_IMAGE_FORMAT aFIF = FreeImage_GetFileTypeU ((const wchar_t* )aFileNameW.ToExtString(), 0);
#else
  FREE_IMAGE_FORMAT aFIF = FreeImage_GetFileType (theImagePath.ToCString(), 0);
#endif
  if (aFIF == FIF_UNKNOWN)
  {
    // no signature? try to guess the file format from the file extension
    aFIF = FreeImage_GetFIFFromFilename (theImagePath.ToCString());
  }
  if ((aFIF == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading (aFIF))
  {
    TCollection_AsciiString aMessage = "Error: image file '";
    aMessage.AssignCat (theImagePath);
    aMessage.AssignCat ("' has unsupported file format.");
    ::Message::DefaultMessenger()->Send (aMessage, Message_Fail);
    return false;
  }

  int aLoadFlags = 0;
  if (aFIF == FIF_GIF)
  {
    // 'Play' the GIF to generate each frame (as 32bpp) instead of returning raw frame data when loading
    aLoadFlags = GIF_PLAYBACK;
  }
  else if (aFIF == FIF_ICO)
  {
    // convert to 32bpp and create an alpha channel from the AND-mask when loading
    aLoadFlags = ICO_MAKEALPHA;
  }

#ifdef _WIN32
  FIBITMAP* anImage = FreeImage_LoadU (aFIF, (const wchar_t* )aFileNameW.ToExtString(), aLoadFlags);
#else
  FIBITMAP* anImage = FreeImage_Load  (aFIF, theImagePath.ToCString(), aLoadFlags);
#endif
  if (anImage == NULL)
  {
    TCollection_AsciiString aMessage = "Error: image file '";
    aMessage.AssignCat (theImagePath);
    aMessage.AssignCat ("' is missing or invalid.");
    ::Message::DefaultMessenger()->Send (aMessage, Message_Fail);
    return false;
  }

  Image_PixMap::ImgFormat aFormat = convertFromFreeFormat (FreeImage_GetImageType (anImage),
                                                           FreeImage_GetColorType (anImage),
                                                           FreeImage_GetBPP (anImage));
  if (aFormat == Image_PixMap::ImgUNKNOWN)
  {
    //anImage = FreeImage_ConvertTo24Bits (anImage);
    TCollection_AsciiString aMessage = "Error: image file '";
    aMessage.AssignCat (theImagePath);
    aMessage.AssignCat ("' has unsupported pixel format.");
    ::Message::DefaultMessenger()->Send (aMessage, Message_Fail);
    return false;
  }

  Image_PixMap::InitWrapper (aFormat, FreeImage_GetBits (anImage),
                             FreeImage_GetWidth (anImage), FreeImage_GetHeight (anImage), FreeImage_GetPitch (anImage));
  SetTopDown (false);

  // assign image after wrapper initialization (virtual Clear() called inside)
  myLibImage = anImage;
  return true;
}
#else
bool Image_AlienPixMap::Load (const TCollection_AsciiString&)
{
  Clear();
  return false;
}
#endif

// =======================================================================
// function : savePPM
// purpose  :
// =======================================================================
bool Image_AlienPixMap::savePPM (const TCollection_AsciiString& theFileName) const
{
  if (IsEmpty())
  {
    return false;
  }

  // Open file
  FILE* aFile = OSD_OpenFile (theFileName.ToCString(), "wb");
  if (aFile == NULL)
  {
    return false;
  }

  // Write header
  fprintf (aFile, "P6\n%d %d\n255\n", (int )SizeX(), (int )SizeY());
  fprintf (aFile, "# Image stored by OpenCASCADE framework in linear RGB colorspace\n");

  // Write pixel data
  Quantity_Color aColor;
  Quantity_Parameter aDummy;
  Standard_Byte aByte;
  for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
  {
    for (Standard_Size aCol = 0; aCol < SizeX(); ++aCol)
    {
      // extremely SLOW but universal (implemented for all supported pixel formats)
      aColor = PixelColor ((Standard_Integer )aCol, (Standard_Integer )aRow, aDummy);
      aByte = Standard_Byte(aColor.Red() * 255.0);   fwrite (&aByte, 1, 1, aFile);
      aByte = Standard_Byte(aColor.Green() * 255.0); fwrite (&aByte, 1, 1, aFile);
      aByte = Standard_Byte(aColor.Blue() * 255.0);  fwrite (&aByte, 1, 1, aFile);
    }
  }

  // Close file
  fclose (aFile);
  return true;
}

// =======================================================================
// function : Save
// purpose  :
// =======================================================================
bool Image_AlienPixMap::Save (const TCollection_AsciiString& theFileName)
{
#ifdef HAVE_FREEIMAGE
  if (myLibImage == NULL)
  {
    return false;
  }

#ifdef _WIN32
  const TCollection_ExtendedString aFileNameW (theFileName.ToCString(), Standard_True);
  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilenameU ((const wchar_t* )aFileNameW.ToExtString());
#else
  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilename (theFileName.ToCString());
#endif
  if (anImageFormat == FIF_UNKNOWN)
  {
#ifdef OCCT_DEBUG
    std::cerr << "Image_PixMap, image format doesn't supported!\n";
#endif
    return false;
  }

  if (IsTopDown())
  {
    FreeImage_FlipVertical (myLibImage);
    SetTopDown (false);
  }

  // FreeImage doesn't provide flexible format convertion API
  // so we should perform multiple convertions in some cases!
  FIBITMAP* anImageToDump = myLibImage;
  switch (anImageFormat)
  {
    case FIF_PNG:
    case FIF_BMP:
    {
      if (Format() == Image_PixMap::ImgBGR32
       || Format() == Image_PixMap::ImgRGB32)
      {
        // stupid FreeImage treats reserved byte as alpha if some bytes not set to 0xFF
        for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < SizeX(); ++aCol)
          {
            myData.ChangeValue (aRow, aCol)[3] = 0xFF;
          }
        }
      }
      else if (FreeImage_GetImageType (myLibImage) != FIT_BITMAP)
      {
        anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_BITMAP);
      }
      break;
    }
    case FIF_GIF:
    {
      FIBITMAP* aTmpBitmap = myLibImage;
      if (FreeImage_GetImageType (myLibImage) != FIT_BITMAP)
      {
        aTmpBitmap = FreeImage_ConvertToType (myLibImage, FIT_BITMAP);
        if (aTmpBitmap == NULL)
        {
          return false;
        }
      }

      if (FreeImage_GetBPP (aTmpBitmap) != 24)
      {
        FIBITMAP* aTmpBitmap24 = FreeImage_ConvertTo24Bits (aTmpBitmap);
        if (aTmpBitmap != myLibImage)
        {
          FreeImage_Unload (aTmpBitmap);
        }
        if (aTmpBitmap24 == NULL)
        {
          return false;
        }
        aTmpBitmap = aTmpBitmap24;
      }

      // need convertion to image with pallete (requires 24bit bitmap)
      anImageToDump = FreeImage_ColorQuantize (aTmpBitmap, FIQ_NNQUANT);
      if (aTmpBitmap != myLibImage)
      {
        FreeImage_Unload (aTmpBitmap);
      }
      break;
    }
    case FIF_EXR:
    {
      if (Format() == Image_PixMap::ImgGray
       || Format() == Image_PixMap::ImgAlpha)
      {
        anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_FLOAT);
      }
      else if (Format() == Image_PixMap::ImgRGBA
            || Format() == Image_PixMap::ImgBGRA)
      {
        anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_RGBAF);
      }
      else
      {
        FREE_IMAGE_TYPE aImgTypeFI = FreeImage_GetImageType (myLibImage);
        if (aImgTypeFI != FIT_RGBF
         && aImgTypeFI != FIT_RGBAF
         && aImgTypeFI != FIT_FLOAT)
        {
          anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_RGBF);
        }
      }
      break;
    }
    default:
    {
      if (FreeImage_GetImageType (myLibImage) != FIT_BITMAP)
      {
        anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_BITMAP);
        if (anImageToDump == NULL)
        {
          return false;
        }
      }

      if (FreeImage_GetBPP (anImageToDump) != 24)
      {
        FIBITMAP* aTmpBitmap24 = FreeImage_ConvertTo24Bits (anImageToDump);
        if (anImageToDump != myLibImage)
        {
          FreeImage_Unload (anImageToDump);
        }
        if (aTmpBitmap24 == NULL)
        {
          return false;
        }
        anImageToDump = aTmpBitmap24;
      }
      break;
    }
  }

  if (anImageToDump == NULL)
  {
    return false;
  }

#ifdef _WIN32
  bool isSaved = (FreeImage_SaveU (anImageFormat, anImageToDump, (const wchar_t* )aFileNameW.ToExtString()) != FALSE);
#else
  bool isSaved = (FreeImage_Save  (anImageFormat, anImageToDump, theFileName.ToCString()) != FALSE);
#endif
  if (anImageToDump != myLibImage)
  {
    FreeImage_Unload (anImageToDump);
  }
  return isSaved;
#else
  const Standard_Integer aLen = theFileName.Length();
  if ((aLen >= 4) && (theFileName.Value (aLen - 3) == '.')
      && strcasecmp( theFileName.ToCString() + aLen - 3, "ppm") == 0 )
  {
    return savePPM (theFileName);
  }
#ifdef OCCT_DEBUG
  std::cerr << "Image_PixMap, no image library available! Image saved in PPM format.\n";
#endif
  return savePPM (theFileName);
#endif
}

// =======================================================================
// function : AdjustGamma
// purpose  :
// =======================================================================
bool Image_AlienPixMap::AdjustGamma (const Standard_Real theGammaCorr)
{
#ifdef HAVE_FREEIMAGE
  return FreeImage_AdjustGamma (myLibImage, theGammaCorr) != FALSE;
#else
  (void )theGammaCorr;
  return false;
#endif
}
