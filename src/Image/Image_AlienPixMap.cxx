// Created on: 2010-09-16
// Created by: KGV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
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

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#ifdef HAVE_FREEIMAGE
  #include <FreeImage.h>

  #ifdef _MSC_VER
    #pragma comment( lib, "FreeImage.lib" )
  #endif
#endif

#include <Image_AlienPixMap.hxx>
#include <gp.hxx>
#include <TCollection_AsciiString.hxx>
#include <fstream>

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
        return FIT_BITMAP;
      default:
        return FIT_UNKNOWN;
    }
  }
};
#endif

IMPLEMENT_STANDARD_HANDLE (Image_AlienPixMap, Image_PixMap)
IMPLEMENT_STANDARD_RTTIEXT(Image_AlienPixMap, Image_PixMap)

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
bool Image_AlienPixMap::InitWrapper (ImgFormat            thePixelFormat,
                                     Standard_Byte*       theDataPtr,
                                     const Standard_Size  theSizeX,
                                     const Standard_Size  theSizeY,
                                     const Standard_Size  theSizeRowBytes)
{
  Clear();
  return false;
}

// =======================================================================
// function : InitTrash
// purpose  :
// =======================================================================
bool Image_AlienPixMap::InitTrash (ImgFormat           thePixelFormat,
                                   const Standard_Size theSizeX,
                                   const Standard_Size theSizeY,
                                   const Standard_Size theSizeRowBytes)
{
  Clear();
#ifdef HAVE_FREEIMAGE
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
#else
  return Image_PixMap::InitTrash (thePixelFormat, theSizeX, theSizeY, theSizeRowBytes);
#endif
}

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
    if (myData.mySizeRowBytes == theCopy.SizeRowBytes()
     && myData.myTopToDown    == theCopy.TopDownInc())
    {
      // copy with one call
      memcpy (myData.myDataPtr, theCopy.Data(), theCopy.SizeBytes());
      return true;
    }

    // copy row-by-row
    const Standard_Size aRowSizeBytes = (myData.mySizeRowBytes > theCopy.SizeRowBytes())
                                      ? theCopy.SizeRowBytes() : myData.mySizeRowBytes;
    for (Standard_Size aRow = 0; aRow < myData.mySizeY; ++aRow)
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
void Image_AlienPixMap::Clear (ImgFormat thePixelFormat)
{
  Image_PixMap::Clear (thePixelFormat);
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
bool Image_AlienPixMap::Load (const TCollection_AsciiString& theImagePath)
{
  Clear();
#ifdef HAVE_FREEIMAGE

  FREE_IMAGE_FORMAT aFIF = FreeImage_GetFileType (theImagePath.ToCString(), 0);
  if (aFIF == FIF_UNKNOWN)
  {
    // no signature? try to guess the file format from the file extension
    aFIF = FreeImage_GetFIFFromFilename (theImagePath.ToCString());
  }
  if ((aFIF == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading (aFIF))
  {
    // unsupported image format
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

  FIBITMAP* anImage = FreeImage_Load (aFIF, theImagePath.ToCString(), aLoadFlags);
  if (anImage == NULL)
  {
    return false;
  }

  Image_PixMap::ImgFormat aFormat = convertFromFreeFormat (FreeImage_GetImageType (anImage),
                                                           FreeImage_GetColorType (anImage),
                                                           FreeImage_GetBPP (anImage));
  if (aFormat == Image_PixMap::ImgUNKNOWN)
  {
    //anImage = FreeImage_ConvertTo24Bits (anImage);
    return false;
  }

  Image_PixMap::InitWrapper (aFormat, FreeImage_GetBits (anImage),
                             FreeImage_GetWidth (anImage), FreeImage_GetHeight (anImage), FreeImage_GetPitch (anImage));
  SetTopDown (false);

  // assign image after wrapper initialization (virtual Clear() called inside)
  myLibImage = anImage;
  return true;
#else
  return false;
#endif
}

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
  FILE* aFile = fopen (theFileName.ToCString(), "wb");
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
    for (Standard_Size aCol = 0; aCol < SizeY(); ++aCol)
    {
      // extremely SLOW but universal (implemented for all supported pixel formats)
      aColor = PixelColor (aCol, aRow, aDummy);
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

  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilename (theFileName.ToCString());
  if (anImageFormat == FIF_UNKNOWN)
  {
    std::cerr << "Image_PixMap, image format doesn't supported!\n";
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
        Image_PixMapData<Image_ColorRGB32>& aData = Image_PixMap::EditData<Image_ColorRGB32>();
        for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < SizeX(); ++aCol)
          {
            aData.ChangeValue (aRow, aCol).a_() = 0xFF;
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
      if (Format() == Image_PixMap::ImgGray)
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

  bool isSaved = (FreeImage_Save (anImageFormat, anImageToDump, theFileName.ToCString()) != FALSE);
  if (anImageToDump != myLibImage)
  {
    FreeImage_Unload (anImageToDump);
  }
  return isSaved;
#else
  const Standard_Integer aLen = theFileName.Length();
  if ((aLen >= 4) && (theFileName.Value (aLen - 3) == '.')
   && TCollection_AsciiString::ISSIMILAR (theFileName.SubString (aLen - 2, aLen), "ppm"))
  {
    return savePPM (theFileName);
  }
  std::cerr << "Image_PixMap, no image library available! Image saved in PPM format.\n";
  return savePPM (theFileName);
#endif
}

// =======================================================================
// function : AdjustGamma
// purpose  :
// =======================================================================
Standard_EXPORT bool Image_AlienPixMap::AdjustGamma (const Standard_Real theGammaCorr)
{
#ifdef HAVE_FREEIMAGE
  return FreeImage_AdjustGamma (myLibImage, theGammaCorr) != FALSE;
#else
  return false;
#endif
}
