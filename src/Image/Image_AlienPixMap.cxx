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

#if !defined(HAVE_FREEIMAGE) && defined(_WIN32)
  #define HAVE_WINCODEC
#endif

#ifdef HAVE_FREEIMAGE
  #include <FreeImage.h>

  #ifdef _MSC_VER
    #pragma comment( lib, "FreeImage.lib" )
  #endif
#elif defined(HAVE_WINCODEC)
  //#include <initguid.h>
  #include <wincodec.h>
  #undef min
  #undef max
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

IMPLEMENT_STANDARD_RTTIEXT(Image_AlienPixMap,Image_PixMap)

namespace
{
#ifdef HAVE_FREEIMAGE
  static Image_Format convertFromFreeFormat (FREE_IMAGE_TYPE       theFormatFI,
                                             FREE_IMAGE_COLOR_TYPE theColorTypeFI,
                                             unsigned              theBitsPerPixel)
  {
    switch (theFormatFI)
    {
      case FIT_RGBF:   return Image_Format_RGBF;
      case FIT_RGBAF:  return Image_Format_RGBAF;
      case FIT_FLOAT:  return Image_Format_GrayF;
      case FIT_BITMAP:
      {
        switch (theColorTypeFI)
        {
          case FIC_MINISBLACK:
          {
            return Image_Format_Gray;
          }
          case FIC_RGB:
          {
            if (Image_PixMap::IsBigEndianHost())
            {
              return (theBitsPerPixel == 32) ? Image_Format_RGB32 : Image_Format_RGB;
            }
            else
            {
              return (theBitsPerPixel == 32) ? Image_Format_BGR32 : Image_Format_BGR;
            }
          }
          case FIC_RGBALPHA:
          {
            return Image_PixMap::IsBigEndianHost() ? Image_Format_RGBA : Image_Format_BGRA;
          }
          default:
            return Image_Format_UNKNOWN;
        }
      }
      default:
        return Image_Format_UNKNOWN;
    }
  }

  static FREE_IMAGE_TYPE convertToFreeFormat (Image_Format theFormat)
  {
    switch (theFormat)
    {
      case Image_Format_GrayF:
      case Image_Format_AlphaF:
        return FIT_FLOAT;
      case Image_Format_RGBAF:
        return FIT_RGBAF;
      case Image_Format_RGBF:
        return FIT_RGBF;
      case Image_Format_RGBA:
      case Image_Format_BGRA:
      case Image_Format_RGB32:
      case Image_Format_BGR32:
      case Image_Format_RGB:
      case Image_Format_BGR:
      case Image_Format_Gray:
      case Image_Format_Alpha:
        return FIT_BITMAP;
      default:
        return FIT_UNKNOWN;
    }
  }
#elif defined(HAVE_WINCODEC)

  //! Return a zero GUID
  static GUID getNullGuid()
  {
    GUID aGuid = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };
    return aGuid;
  }

  //! Sentry over IUnknown pointer.
  template<class T> class Image_ComPtr
  {
  public:
    //! Empty constructor.
    Image_ComPtr()
    : myPtr (NULL) {}

    //! Destructor.
    ~Image_ComPtr()
    {
      Nullify();
    }

    //! Return TRUE if pointer is NULL.
    bool IsNull() const { return myPtr == NULL; }

    //! Release the pointer.
    void Nullify()
    {
      if (myPtr != NULL)
      {
        myPtr->Release();
        myPtr = NULL;
      }
    }

    //! Return pointer for initialization.
    T*& ChangePtr()
    {
      Standard_ASSERT_RAISE (myPtr == NULL, "Pointer cannot be initialized twice!");
      return myPtr;
    }

    //! Return pointer.
    T* get() { return myPtr; }

    //! Return pointer.
    T* operator->() { return get(); }

    //! Cast handle to contained type
    T& operator*() { return *get(); }

  private:
    T* myPtr;
  };

  //! Convert WIC GUID to Image_Format.
  static Image_Format convertFromWicFormat (const WICPixelFormatGUID& theFormat)
  {
    if (theFormat == GUID_WICPixelFormat32bppBGRA)
    {
      return Image_Format_BGRA;
    }
    else if (theFormat == GUID_WICPixelFormat32bppBGR)
    {
      return Image_Format_BGR32;
    }
    else if (theFormat == GUID_WICPixelFormat24bppRGB)
    {
      return Image_Format_RGB;
    }
    else if (theFormat == GUID_WICPixelFormat24bppBGR)
    {
      return Image_Format_BGR;
    }
    else if (theFormat == GUID_WICPixelFormat8bppGray)
    {
      return Image_Format_Gray;
    }
    return Image_Format_UNKNOWN;
  }

  //! Convert Image_Format to WIC GUID.
  static WICPixelFormatGUID convertToWicFormat (Image_Format theFormat)
  {
    switch (theFormat)
    {
      case Image_Format_BGRA:   return GUID_WICPixelFormat32bppBGRA;
      case Image_Format_BGR32:  return GUID_WICPixelFormat32bppBGR;
      case Image_Format_RGB:    return GUID_WICPixelFormat24bppRGB;
      case Image_Format_BGR:    return GUID_WICPixelFormat24bppBGR;
      case Image_Format_Gray:   return GUID_WICPixelFormat8bppGray;
      case Image_Format_Alpha:  return GUID_WICPixelFormat8bppGray; // GUID_WICPixelFormat8bppAlpha
      case Image_Format_GrayF:  // GUID_WICPixelFormat32bppGrayFloat
      case Image_Format_AlphaF:
      case Image_Format_RGBAF:  // GUID_WICPixelFormat128bppRGBAFloat
      case Image_Format_RGBF:   // GUID_WICPixelFormat96bppRGBFloat
      case Image_Format_RGBA:   // GUID_WICPixelFormat32bppRGBA
      case Image_Format_RGB32:  // GUID_WICPixelFormat32bppRGB
      default:
        return getNullGuid();
    }
  }

#endif
}

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
bool Image_AlienPixMap::InitWrapper (Image_Format,
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
bool Image_AlienPixMap::InitTrash (Image_Format        thePixelFormat,
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
  Image_Format aFormat = convertFromFreeFormat (FreeImage_GetImageType(anImage),
                                                FreeImage_GetColorType(anImage),
                                                FreeImage_GetBPP      (anImage));
  if (thePixelFormat == Image_Format_BGR32
   || thePixelFormat == Image_Format_RGB32)
  {
    //FreeImage_SetTransparent (anImage, FALSE);
    aFormat = (aFormat == Image_Format_BGRA) ? Image_Format_BGR32 : Image_Format_RGB32;
  }

  Image_PixMap::InitWrapper (aFormat, FreeImage_GetBits (anImage),
                             FreeImage_GetWidth (anImage), FreeImage_GetHeight (anImage), FreeImage_GetPitch (anImage));
  SetTopDown (false);

  // assign image after wrapper initialization (virtual Clear() called inside)
  myLibImage = anImage;
  return true;
}
#elif defined(HAVE_WINCODEC)
bool Image_AlienPixMap::InitTrash (Image_Format        thePixelFormat,
                                   const Standard_Size theSizeX,
                                   const Standard_Size theSizeY,
                                   const Standard_Size theSizeRowBytes)
{
  Clear();
  Image_Format aFormat = thePixelFormat;
  switch (aFormat)
  {
    case Image_Format_RGB:
      aFormat = Image_Format_BGR;
      break;
    case Image_Format_RGB32:
      aFormat = Image_Format_BGR32;
      break;
    case Image_Format_RGBA:
      aFormat = Image_Format_BGRA;
      break;
    default:
      break;
  }

  if (!Image_PixMap::InitTrash (aFormat, theSizeX, theSizeY, theSizeRowBytes))
  {
    return false;
  }
  SetTopDown (true);
  return true;
}
#else
bool Image_AlienPixMap::InitTrash (Image_Format        thePixelFormat,
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
  const TCollection_ExtendedString aFileNameW (theImagePath);
  FREE_IMAGE_FORMAT aFIF = FreeImage_GetFileTypeU (aFileNameW.ToWideString(), 0);
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
  FIBITMAP* anImage = FreeImage_LoadU (aFIF, aFileNameW.ToWideString(), aLoadFlags);
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

  Image_Format aFormat = convertFromFreeFormat (FreeImage_GetImageType(anImage),
                                                FreeImage_GetColorType(anImage),
                                                FreeImage_GetBPP      (anImage));
  if (aFormat == Image_Format_UNKNOWN)
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
#elif defined(HAVE_WINCODEC)
bool Image_AlienPixMap::Load (const TCollection_AsciiString& theImagePath)
{
  Clear();

  IWICImagingFactory* aWicImgFactory = NULL;
  CoInitializeEx (NULL, COINIT_MULTITHREADED);
  if (CoCreateInstance (CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&aWicImgFactory)) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot initialize WIC Imaging Factory", Message_Fail);
    return false;
  }

  Image_ComPtr<IWICBitmapDecoder> aWicDecoder;
  const TCollection_ExtendedString aFileNameW (theImagePath);
  if (aWicImgFactory->CreateDecoderFromFilename (aFileNameW.ToWideString(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &aWicDecoder.ChangePtr()) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot create WIC Image Decoder", Message_Fail);
    return false;
  }

  UINT aFrameCount = 0, aFrameSizeX = 0, aFrameSizeY = 0;
  WICPixelFormatGUID aWicPixelFormat = getNullGuid();
  Image_ComPtr<IWICBitmapFrameDecode> aWicFrameDecode;
  if (aWicDecoder->GetFrameCount (&aFrameCount) != S_OK
   || aFrameCount < 1
   || aWicDecoder->GetFrame (0, &aWicFrameDecode.ChangePtr()) != S_OK
   || aWicFrameDecode->GetSize (&aFrameSizeX, &aFrameSizeY) != S_OK
   || aWicFrameDecode->GetPixelFormat (&aWicPixelFormat))
  {
    Message::DefaultMessenger()->Send ("Error: cannot get WIC Image Frame", Message_Fail);
    return false;
  }

  Image_ComPtr<IWICFormatConverter> aWicConvertedFrame;
  Image_Format aPixelFormat = convertFromWicFormat (aWicPixelFormat);
  if (aPixelFormat == Image_Format_UNKNOWN)
  {
    aPixelFormat = Image_Format_RGB;
    if (aWicImgFactory->CreateFormatConverter (&aWicConvertedFrame.ChangePtr()) != S_OK
     || aWicConvertedFrame->Initialize (aWicFrameDecode.get(), convertToWicFormat (aPixelFormat), WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom) != S_OK)
    {
      Message::DefaultMessenger()->Send ("Error: cannot convert WIC Image Frame to RGB format", Message_Fail);
      return false;
    }
    aWicFrameDecode.Nullify();
  }

  if (!Image_PixMap::InitTrash (aPixelFormat, aFrameSizeX, aFrameSizeY))
  {
    Message::DefaultMessenger()->Send ("Error: cannot initialize memory for image", Message_Fail);
    return false;
  }

  IWICBitmapSource* aWicSrc = aWicFrameDecode.get();
  if(!aWicConvertedFrame.IsNull())
  {
    aWicSrc = aWicConvertedFrame.get();
  }
  if (aWicSrc->CopyPixels (NULL, (UINT )SizeRowBytes(), (UINT )SizeBytes(), ChangeData()) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot copy pixels from WIC Image", Message_Fail);
    return false;
  }
  SetTopDown (true);
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
  Standard_Byte aByte;
  for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
  {
    for (Standard_Size aCol = 0; aCol < SizeX(); ++aCol)
    {
      // extremely SLOW but universal (implemented for all supported pixel formats)
      const Quantity_ColorRGBA aColor = PixelColor ((Standard_Integer )aCol, (Standard_Integer )aRow);
      aByte = Standard_Byte(aColor.GetRGB().Red()   * 255.0); fwrite (&aByte, 1, 1, aFile);
      aByte = Standard_Byte(aColor.GetRGB().Green() * 255.0); fwrite (&aByte, 1, 1, aFile);
      aByte = Standard_Byte(aColor.GetRGB().Blue()  * 255.0); fwrite (&aByte, 1, 1, aFile);
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
  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilenameU (aFileNameW.ToWideString());
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

  // FreeImage doesn't provide flexible format conversion API
  // so we should perform multiple conversions in some cases!
  FIBITMAP* anImageToDump = myLibImage;
  switch (anImageFormat)
  {
    case FIF_PNG:
    case FIF_BMP:
    {
      if (Format() == Image_Format_BGR32
       || Format() == Image_Format_RGB32)
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

      // need conversion to image with palette (requires 24bit bitmap)
      anImageToDump = FreeImage_ColorQuantize (aTmpBitmap, FIQ_NNQUANT);
      if (aTmpBitmap != myLibImage)
      {
        FreeImage_Unload (aTmpBitmap);
      }
      break;
    }
    case FIF_HDR:
    case FIF_EXR:
    {
      if (Format() == Image_Format_Gray
       || Format() == Image_Format_Alpha)
      {
        anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_FLOAT);
      }
      else if (Format() == Image_Format_RGBA
            || Format() == Image_Format_BGRA)
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
  bool isSaved = (FreeImage_SaveU (anImageFormat, anImageToDump, aFileNameW.ToWideString()) != FALSE);
#else
  bool isSaved = (FreeImage_Save  (anImageFormat, anImageToDump, theFileName.ToCString()) != FALSE);
#endif
  if (anImageToDump != myLibImage)
  {
    FreeImage_Unload (anImageToDump);
  }
  return isSaved;

#elif defined(HAVE_WINCODEC)

  TCollection_AsciiString aFileNameLower = theFileName;
  aFileNameLower.LowerCase();
  GUID aFileFormat = getNullGuid();
  if (aFileNameLower.EndsWith (".ppm"))
  {
    return savePPM (theFileName);
  }
  else if (aFileNameLower.EndsWith (".bmp"))
  {
    aFileFormat = GUID_ContainerFormatBmp;
  }
  else if (aFileNameLower.EndsWith (".png"))
  {
    aFileFormat = GUID_ContainerFormatPng;
  }
  else if (aFileNameLower.EndsWith (".jpg")
        || aFileNameLower.EndsWith (".jpeg"))
  {
    aFileFormat = GUID_ContainerFormatJpeg;
  }
  else if (aFileNameLower.EndsWith (".tiff"))
  {
    aFileFormat = GUID_ContainerFormatTiff;
  }
  else if (aFileNameLower.EndsWith (".gif"))
  {
    aFileFormat = GUID_ContainerFormatGif;
  }

  if (aFileFormat == getNullGuid())
  {
    Message::DefaultMessenger()->Send ("Error: unsupported image format", Message_Fail);
    return false;
  }

  IWICImagingFactory* aWicImgFactory = NULL;
  CoInitializeEx (NULL, COINIT_MULTITHREADED);
  if (CoCreateInstance (CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&aWicImgFactory)) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot initialize WIC Imaging Factory", Message_Fail);
    return false;
  }

  Image_ComPtr<IWICStream> aWicFileStream;
  Image_ComPtr<IWICBitmapEncoder> aWicEncoder;
  const TCollection_ExtendedString aFileNameW (theFileName);
  if (aWicImgFactory->CreateStream (&aWicFileStream.ChangePtr()) != S_OK
   || aWicFileStream->InitializeFromFilename (aFileNameW.ToWideString(), GENERIC_WRITE) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot create WIC File Stream", Message_Fail);
    return false;
  }
  if (aWicImgFactory->CreateEncoder (aFileFormat, NULL, &aWicEncoder.ChangePtr()) != S_OK
   || aWicEncoder->Initialize (aWicFileStream.get(), WICBitmapEncoderNoCache) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot create WIC Encoder", Message_Fail);
    return false;
  }

  const WICPixelFormatGUID aWicPixelFormat = convertToWicFormat (myImgFormat);
  if (aWicPixelFormat == getNullGuid())
  {
    Message::DefaultMessenger()->Send ("Error: unsupported pixel format", Message_Fail);
    return false;
  }

  WICPixelFormatGUID aWicPixelFormatRes = aWicPixelFormat;
  Image_ComPtr<IWICBitmapFrameEncode> aWicFrameEncode;
  if (aWicEncoder->CreateNewFrame (&aWicFrameEncode.ChangePtr(), NULL) != S_OK
   || aWicFrameEncode->Initialize (NULL) != S_OK
   || aWicFrameEncode->SetSize ((UINT )SizeX(), (UINT )SizeY()) != S_OK
   || aWicFrameEncode->SetPixelFormat (&aWicPixelFormatRes) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot create WIC Frame", Message_Fail);
    return false;
  }

  if (aWicPixelFormatRes != aWicPixelFormat)
  {
    Message::DefaultMessenger()->Send ("Error: pixel format is unsupported by image format", Message_Fail);
    return false;
  }

  if (IsTopDown())
  {
    if (aWicFrameEncode->WritePixels ((UINT )SizeY(), (UINT )SizeRowBytes(), (UINT )SizeBytes(), (BYTE* )Data()) != S_OK)
    {
      Message::DefaultMessenger()->Send ("Error: cannot write pixels to WIC Frame", Message_Fail);
      return false;
    }
  }
  else
  {
    for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
    {
      if (aWicFrameEncode->WritePixels (1, (UINT )SizeRowBytes(), (UINT )SizeRowBytes(), (BYTE* )Row (aRow)) != S_OK)
      {
        Message::DefaultMessenger()->Send ("Error: cannot write pixels to WIC Frame", Message_Fail);
        return false;
      }
    }
  }

  if (aWicFrameEncode->Commit() != S_OK
   || aWicEncoder->Commit() != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot commit data to WIC Frame", Message_Fail);
    return false;
  }
  if (aWicFileStream->Commit (STGC_DEFAULT) != S_OK)
  {
    //Message::DefaultMessenger()->Send ("Error: cannot commit data to WIC File Stream", Message_Fail);
    //return false;
  }
  return true;
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
