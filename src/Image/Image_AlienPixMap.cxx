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
    #pragma comment(lib, "FreeImage.lib")
  #endif
#elif defined(HAVE_WINCODEC)
  #include <wincodec.h>
  // prevent warnings on MSVC10
  #include <Standard_WarningsDisable.hxx>
  #include <Standard_TypeDef.hxx>
  #include <Standard_WarningsRestore.hxx>
  #undef min
  #undef max

  #ifdef _MSC_VER
    #pragma comment(lib, "Ole32.lib")
  #endif
#elif defined(__EMSCRIPTEN__)
  #include <emscripten/emscripten.h>
#endif

#include <Image_AlienPixMap.hxx>
#include <gp.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_ArrayStreamBuffer.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_OpenFile.hxx>

#include <fstream>
#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(Image_AlienPixMap, Image_PixMap)

namespace
{
#ifdef HAVE_FREEIMAGE
static Image_Format convertFromFreeFormat(FREE_IMAGE_TYPE       theFormatFI,
                                          FREE_IMAGE_COLOR_TYPE theColorTypeFI,
                                          unsigned              theBitsPerPixel)
{
  switch (theFormatFI)
  {
    case FIT_RGBF:
      return Image_Format_RGBF;
    case FIT_RGBAF:
      return Image_Format_RGBAF;
    case FIT_FLOAT:
      return Image_Format_GrayF;
    case FIT_INT16:
    case FIT_UINT16:
      return Image_Format_Gray16;
    case FIT_BITMAP: {
      switch (theColorTypeFI)
      {
        case FIC_MINISBLACK: {
          return Image_Format_Gray;
        }
        case FIC_RGB: {
          if (Image_PixMap::IsBigEndianHost())
          {
            return (theBitsPerPixel == 32) ? Image_Format_RGB32 : Image_Format_RGB;
          }
          else
          {
            return (theBitsPerPixel == 32) ? Image_Format_BGR32 : Image_Format_BGR;
          }
        }
        case FIC_RGBALPHA: {
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

static FREE_IMAGE_TYPE convertToFreeFormat(Image_Format theFormat)
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
    case Image_Format_Gray16:
      return FIT_UINT16;
    default:
      return FIT_UNKNOWN;
  }
}

//! Wrapper for accessing C++ stream from FreeImage.
class Image_FreeImageStream
{
public:
  //! Construct wrapper over input stream.
  Image_FreeImageStream(std::istream& theStream)
      : myIStream(&theStream),
        myOStream(NULL),
        myInitPos(theStream.tellg())
  {
  }

  //! Construct wrapper over output stream.
  Image_FreeImageStream(std::ostream& theStream)
      : myIStream(NULL),
        myOStream(&theStream),
        myInitPos(theStream.tellp())
  {
  }

  //! Get io object.
  FreeImageIO GetFiIO() const
  {
    FreeImageIO anIo;
    memset(&anIo, 0, sizeof(anIo));
    if (myIStream != NULL)
    {
      anIo.read_proc = readProc;
      anIo.seek_proc = seekProcIn;
      anIo.tell_proc = tellProcIn;
    }
    if (myOStream != NULL)
    {
      anIo.write_proc = writeProc;
      // seek and tell are also used for saving in some formats (.tif for example)
      anIo.seek_proc = seekProcOut;
      anIo.tell_proc = tellProcOut;
    }
    return anIo;
  }

public:
  //! Simulate fread().
  static unsigned int DLL_CALLCONV readProc(void*        theBuffer,
                                            unsigned int theSize,
                                            unsigned int theCount,
                                            fi_handle    theHandle)
  {
    Image_FreeImageStream* aThis = (Image_FreeImageStream*)theHandle;
    if (aThis->myIStream == NULL)
    {
      return 0;
    }

    if (!aThis->myIStream->read((char*)theBuffer,
                                std::streamsize(theSize) * std::streamsize(theCount)))
    {
      // aThis->myIStream->clear();
    }
    const std::streamsize aNbRead = aThis->myIStream->gcount();
    return (unsigned int)(aNbRead / theSize);
  }

  //! Simulate fwrite().
  static unsigned int DLL_CALLCONV writeProc(void*        theBuffer,
                                             unsigned int theSize,
                                             unsigned int theCount,
                                             fi_handle    theHandle)
  {
    Image_FreeImageStream* aThis = (Image_FreeImageStream*)theHandle;
    if (aThis->myOStream != NULL
        && aThis->myOStream->write((const char*)theBuffer,
                                   std::streamsize(theSize) * std::streamsize(theCount)))
    {
      return theCount;
    }
    return 0;
  }

  //! Simulate fseek().
  static int DLL_CALLCONV seekProcIn(fi_handle theHandle, long theOffset, int theOrigin)
  {
    Image_FreeImageStream* aThis = (Image_FreeImageStream*)theHandle;
    if (aThis->myIStream == NULL)
    {
      return -1;
    }

    bool isSeekDone = false;
    switch (theOrigin)
    {
      case SEEK_SET:
        if (aThis->myIStream->seekg((std::streamoff)aThis->myInitPos + theOffset, std::ios::beg))
        {
          isSeekDone = true;
        }
        break;
      case SEEK_CUR:
        if (aThis->myIStream->seekg(theOffset, std::ios::cur))
        {
          isSeekDone = true;
        }
        break;
      case SEEK_END:
        if (aThis->myIStream->seekg(theOffset, std::ios::end))
        {
          isSeekDone = true;
        }
        break;
    }
    return isSeekDone ? 0 : -1;
  }

  static int DLL_CALLCONV seekProcOut(fi_handle theHandle, long theOffset, int theOrigin)
  {
    Image_FreeImageStream* aThis = (Image_FreeImageStream*)theHandle;
    if (aThis->myOStream == NULL)
    {
      return -1;
    }

    bool isSeekDone = false;
    switch (theOrigin)
    {
      case SEEK_SET:
        if (aThis->myOStream->seekp((std::streamoff)aThis->myInitPos + theOffset, std::ios::beg))
        {
          isSeekDone = true;
        }
        break;
      case SEEK_CUR:
        if (aThis->myOStream->seekp(theOffset, std::ios::cur))
        {
          isSeekDone = true;
        }
        break;
      case SEEK_END:
        if (aThis->myOStream->seekp(theOffset, std::ios::end))
        {
          isSeekDone = true;
        }
        break;
    }
    return isSeekDone ? 0 : -1;
  }

  //! Simulate ftell().
  static long DLL_CALLCONV tellProcIn(fi_handle theHandle)
  {
    Image_FreeImageStream* aThis = (Image_FreeImageStream*)theHandle;
    const long             aPos =
      aThis->myIStream != NULL ? (long)(aThis->myIStream->tellg() - aThis->myInitPos) : 0;
    return aPos;
  }

  static long DLL_CALLCONV tellProcOut(fi_handle theHandle)
  {
    Image_FreeImageStream* aThis = (Image_FreeImageStream*)theHandle;
    const long             aPos =
      aThis->myOStream != NULL ? (long)(aThis->myOStream->tellp() - aThis->myInitPos) : 0;
    return aPos;
  }

private:
  std::istream*  myIStream;
  std::ostream*  myOStream;
  std::streampos myInitPos;
};

#elif defined(HAVE_WINCODEC)

//! Return a zero GUID
static GUID getNullGuid()
{
  GUID aGuid = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};
  return aGuid;
}

//! Returns GUID of image format from file name
static GUID getFileFormatFromName(const TCollection_AsciiString& theFileName)
{
  TCollection_AsciiString aFileNameLower = theFileName;
  aFileNameLower.LowerCase();
  GUID aFileFormat = getNullGuid();
  if (aFileNameLower.EndsWith(".bmp"))
  {
    aFileFormat = GUID_ContainerFormatBmp;
  }
  else if (aFileNameLower.EndsWith(".png"))
  {
    aFileFormat = GUID_ContainerFormatPng;
  }
  else if (aFileNameLower.EndsWith(".jpg") || aFileNameLower.EndsWith(".jpeg"))
  {
    aFileFormat = GUID_ContainerFormatJpeg;
  }
  else if (aFileNameLower.EndsWith(".tiff") || aFileNameLower.EndsWith(".tif"))
  {
    aFileFormat = GUID_ContainerFormatTiff;
  }
  else if (aFileNameLower.EndsWith(".gif"))
  {
    aFileFormat = GUID_ContainerFormatGif;
  }
  return aFileFormat;
}

//! Sentry over IUnknown pointer.
template <class T>
class Image_ComPtr
{
public:
  //! Empty constructor.
  Image_ComPtr()
      : myPtr(NULL)
  {
  }

  //! Destructor.
  ~Image_ComPtr() { Nullify(); }

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
    Standard_ASSERT_RAISE(myPtr == NULL, "Pointer cannot be initialized twice!");
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
static Image_Format convertFromWicFormat(const WICPixelFormatGUID& theFormat)
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
  else if (theFormat == GUID_WICPixelFormat16bppGray)
  {
    return Image_Format_Gray16;
  }
  return Image_Format_UNKNOWN;
}

//! Convert Image_Format to WIC GUID.
static WICPixelFormatGUID convertToWicFormat(Image_Format theFormat)
{
  switch (theFormat)
  {
    case Image_Format_BGRA:
      return GUID_WICPixelFormat32bppBGRA;
    case Image_Format_BGR32:
      return GUID_WICPixelFormat32bppBGR;
    case Image_Format_RGB:
      return GUID_WICPixelFormat24bppRGB;
    case Image_Format_BGR:
      return GUID_WICPixelFormat24bppBGR;
    case Image_Format_Gray:
      return GUID_WICPixelFormat8bppGray;
      // clang-format off
      case Image_Format_Alpha:  return GUID_WICPixelFormat8bppGray; // GUID_WICPixelFormat8bppAlpha
      // clang-format on
    case Image_Format_Gray16:
      return GUID_WICPixelFormat16bppGray;
    case Image_Format_GrayF: // GUID_WICPixelFormat32bppGrayFloat
    case Image_Format_AlphaF:
    case Image_Format_RGBAF: // GUID_WICPixelFormat128bppRGBAFloat
    case Image_Format_RGBF:  // GUID_WICPixelFormat96bppRGBFloat
    case Image_Format_RGBA:  // GUID_WICPixelFormat32bppRGBA
    case Image_Format_RGB32: // GUID_WICPixelFormat32bppRGB
    default:
      return getNullGuid();
  }
}

#endif
} // namespace

// =======================================================================
// function : Image_AlienPixMap
// purpose  :
// =======================================================================
Image_AlienPixMap::Image_AlienPixMap()
#ifdef HAVE_WINCODEC
    : myPalette(NULL)
#else
    : myLibImage(NULL)
#endif
{
  SetTopDown(false);
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
bool Image_AlienPixMap::InitWrapper(Image_Format,
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
bool Image_AlienPixMap::InitTrash(Image_Format        thePixelFormat,
                                  const Standard_Size theSizeX,
                                  const Standard_Size theSizeY,
                                  const Standard_Size /*theSizeRowBytes*/)
{
  Clear();
  FREE_IMAGE_TYPE aFormatFI     = convertToFreeFormat(thePixelFormat);
  int             aBitsPerPixel = (int)Image_PixMap::SizePixelBytes(thePixelFormat) * 8;
  if (aFormatFI == FIT_UNKNOWN)
  {
    aFormatFI     = FIT_BITMAP;
    aBitsPerPixel = 24;
  }

  FIBITMAP* anImage = FreeImage_AllocateT(aFormatFI, (int)theSizeX, (int)theSizeY, aBitsPerPixel);
  Image_Format aFormat = convertFromFreeFormat(FreeImage_GetImageType(anImage),
                                               FreeImage_GetColorType(anImage),
                                               FreeImage_GetBPP(anImage));
  if (thePixelFormat == Image_Format_BGR32 || thePixelFormat == Image_Format_RGB32)
  {
    // FreeImage_SetTransparent (anImage, FALSE);
    aFormat = (aFormat == Image_Format_BGRA) ? Image_Format_BGR32 : Image_Format_RGB32;
  }

  Image_PixMap::InitWrapper(aFormat,
                            FreeImage_GetBits(anImage),
                            FreeImage_GetWidth(anImage),
                            FreeImage_GetHeight(anImage),
                            FreeImage_GetPitch(anImage));
  SetTopDown(false);

  // assign image after wrapper initialization (virtual Clear() called inside)
  myLibImage = anImage;
  return true;
}
#elif defined(HAVE_WINCODEC)
bool Image_AlienPixMap::InitTrash(Image_Format        thePixelFormat,
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

  if (!Image_PixMap::InitTrash(aFormat, theSizeX, theSizeY, theSizeRowBytes))
  {
    return false;
  }
  SetTopDown(true);
  return true;
}
#else
bool Image_AlienPixMap::InitTrash(Image_Format        thePixelFormat,
                                  const Standard_Size theSizeX,
                                  const Standard_Size theSizeY,
                                  const Standard_Size theSizeRowBytes)
{
  return Image_PixMap::InitTrash(thePixelFormat, theSizeX, theSizeY, theSizeRowBytes);
}
#endif

// =======================================================================
// function : InitCopy
// purpose  :
// =======================================================================
bool Image_AlienPixMap::InitCopy(const Image_PixMap& theCopy)
{
  if (&theCopy == this)
  {
    // self-copying disallowed
    return false;
  }
  if (!InitTrash(theCopy.Format(), theCopy.SizeX(), theCopy.SizeY(), theCopy.SizeRowBytes()))
  {
    return false;
  }

  if (myImgFormat == theCopy.Format())
  {
    if (SizeRowBytes() == theCopy.SizeRowBytes() && TopDownInc() == theCopy.TopDownInc())
    {
      // copy with one call
      memcpy(ChangeData(), theCopy.Data(), std::min(SizeBytes(), theCopy.SizeBytes()));
      return true;
    }

    // copy row-by-row
    const Standard_Size aRowSizeBytes = std::min(SizeRowBytes(), theCopy.SizeRowBytes());
    for (Standard_Size aRow = 0; aRow < myData.SizeY; ++aRow)
    {
      memcpy(ChangeRow(aRow), theCopy.Row(aRow), aRowSizeBytes);
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
    FreeImage_Unload(myLibImage);
    myLibImage = NULL;
  }
#elif defined(HAVE_WINCODEC)
  if (myPalette != NULL)
  {
    myPalette->Release();
    myPalette = NULL;
  }
#elif defined(__EMSCRIPTEN__)
  if (myLibImage != NULL)
  {
    free((void*)myLibImage);
    myLibImage = NULL;
  }
#endif
}

// =======================================================================
// function : IsTopDownDefault
// purpose  :
// =======================================================================
bool Image_AlienPixMap::IsTopDownDefault()
{
#ifdef HAVE_FREEIMAGE
  return false;
#elif defined(HAVE_WINCODEC)
  return true;
#else
  return false;
#endif
}

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
#ifdef HAVE_FREEIMAGE
bool Image_AlienPixMap::Load(const Standard_Byte*           theData,
                             const Standard_Size            theLength,
                             const TCollection_AsciiString& theImagePath)
{
  Clear();

  #ifdef _WIN32
  const TCollection_ExtendedString aFileNameW(theImagePath);
  #endif
  FREE_IMAGE_FORMAT aFIF   = FIF_UNKNOWN;
  FIMEMORY*         aFiMem = NULL;
  if (theData != NULL)
  {
    aFiMem = FreeImage_OpenMemory((BYTE*)theData, (DWORD)theLength);
    aFIF   = FreeImage_GetFileTypeFromMemory(aFiMem, 0);
  }
  else
  {
  #ifdef _WIN32
    aFIF = FreeImage_GetFileTypeU(aFileNameW.ToWideString(), 0);
  #else
    aFIF = FreeImage_GetFileType(theImagePath.ToCString(), 0);
  #endif
  }
  if (aFIF == FIF_UNKNOWN)
  {
    // no signature? try to guess the file format from the file extension
    aFIF = FreeImage_GetFIFFromFilename(theImagePath.ToCString());
  }
  if ((aFIF == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading(aFIF))
  {
    ::Message::SendFail(TCollection_AsciiString("Error: image '") + theImagePath
                        + "' has unsupported file format");
    if (aFiMem != NULL)
    {
      FreeImage_CloseMemory(aFiMem);
    }
    return false;
  }

  int aLoadFlags = 0;
  if (aFIF == FIF_GIF)
  {
    // 'Play' the GIF to generate each frame (as 32bpp) instead of returning raw frame data when
    // loading
    aLoadFlags = GIF_PLAYBACK;
  }
  else if (aFIF == FIF_ICO)
  {
    // convert to 32bpp and create an alpha channel from the AND-mask when loading
    aLoadFlags = ICO_MAKEALPHA;
  }

  FIBITMAP* anImage = NULL;
  if (theData != NULL)
  {
    anImage = FreeImage_LoadFromMemory(aFIF, aFiMem, aLoadFlags);
    FreeImage_CloseMemory(aFiMem);
    aFiMem = NULL;
  }
  else
  {
  #ifdef _WIN32
    anImage = FreeImage_LoadU(aFIF, aFileNameW.ToWideString(), aLoadFlags);
  #else
    anImage = FreeImage_Load(aFIF, theImagePath.ToCString(), aLoadFlags);
  #endif
  }
  if (anImage == NULL)
  {
    ::Message::SendFail(TCollection_AsciiString("Error: image file '") + theImagePath
                        + "' is missing or invalid");
    return false;
  }

  Image_Format aFormat = Image_Format_UNKNOWN;
  if (FreeImage_GetBPP(anImage) == 1)
  {
    FIBITMAP* aTmpImage = FreeImage_ConvertTo8Bits(anImage);
    FreeImage_Unload(anImage);
    anImage = aTmpImage;
  }
  if (anImage != NULL)
  {
    aFormat = convertFromFreeFormat(FreeImage_GetImageType(anImage),
                                    FreeImage_GetColorType(anImage),
                                    FreeImage_GetBPP(anImage));
    if (aFormat == Image_Format_UNKNOWN)
    {
      FIBITMAP* aTmpImage = FreeImage_ConvertTo24Bits(anImage);
      FreeImage_Unload(anImage);
      anImage = aTmpImage;
      if (anImage != NULL)
      {
        aFormat = convertFromFreeFormat(FreeImage_GetImageType(anImage),
                                        FreeImage_GetColorType(anImage),
                                        FreeImage_GetBPP(anImage));
      }
    }
  }
  if (aFormat == Image_Format_UNKNOWN)
  {
    ::Message::SendFail(TCollection_AsciiString("Error: image '") + theImagePath
                        + "' has unsupported pixel format");
    return false;
  }

  Image_PixMap::InitWrapper(aFormat,
                            FreeImage_GetBits(anImage),
                            FreeImage_GetWidth(anImage),
                            FreeImage_GetHeight(anImage),
                            FreeImage_GetPitch(anImage));
  SetTopDown(false);

  // assign image after wrapper initialization (virtual Clear() called inside)
  myLibImage = anImage;
  return true;
}

bool Image_AlienPixMap::Load(std::istream& theStream, const TCollection_AsciiString& theFileName)
{
  Clear();

  Image_FreeImageStream aStream(theStream);
  FreeImageIO           aFiIO = aStream.GetFiIO();

  FREE_IMAGE_FORMAT aFIF = FreeImage_GetFileTypeFromHandle(&aFiIO, &aStream, 0);
  if (aFIF == FIF_UNKNOWN)
  {
    // no signature? try to guess the file format from the file extension
    aFIF = FreeImage_GetFIFFromFilename(theFileName.ToCString());
  }
  if ((aFIF == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading(aFIF))
  {
    ::Message::SendFail(TCollection_AsciiString("Error: image stream '") + theFileName
                        + "' has unsupported file format");
    return false;
  }

  int aLoadFlags = 0;
  if (aFIF == FIF_GIF)
  {
    // 'Play' the GIF to generate each frame (as 32bpp) instead of returning raw frame data when
    // loading
    aLoadFlags = GIF_PLAYBACK;
  }
  else if (aFIF == FIF_ICO)
  {
    // convert to 32bpp and create an alpha channel from the AND-mask when loading
    aLoadFlags = ICO_MAKEALPHA;
  }

  FIBITMAP* anImage = FreeImage_LoadFromHandle(aFIF, &aFiIO, &aStream, aLoadFlags);
  if (anImage == NULL)
  {
    ::Message::SendFail(TCollection_AsciiString("Error: image stream '") + theFileName
                        + "' is missing or invalid");
    return false;
  }

  Image_Format aFormat = convertFromFreeFormat(FreeImage_GetImageType(anImage),
                                               FreeImage_GetColorType(anImage),
                                               FreeImage_GetBPP(anImage));
  if (aFormat == Image_Format_UNKNOWN)
  {
    ::Message::SendFail(TCollection_AsciiString("Error: image stream '") + theFileName
                        + "' has unsupported pixel format");
    return false;
  }

  Image_PixMap::InitWrapper(aFormat,
                            FreeImage_GetBits(anImage),
                            FreeImage_GetWidth(anImage),
                            FreeImage_GetHeight(anImage),
                            FreeImage_GetPitch(anImage));
  SetTopDown(false);

  // assign image after wrapper initialization (virtual Clear() called inside)
  myLibImage = anImage;
  return true;
}

#elif defined(HAVE_WINCODEC)
bool Image_AlienPixMap::Load(const Standard_Byte*           theData,
                             const Standard_Size            theLength,
                             const TCollection_AsciiString& theFileName)
{
  Clear();

  Image_ComPtr<IWICImagingFactory> aWicImgFactory;
  CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (CoCreateInstance(CLSID_WICImagingFactory,
                       NULL,
                       CLSCTX_INPROC_SERVER,
                       IID_PPV_ARGS(&aWicImgFactory.ChangePtr()))
      != S_OK)
  {
    Message::SendFail("Error: cannot initialize WIC Imaging Factory");
    return false;
  }

  Image_ComPtr<IWICBitmapDecoder> aWicDecoder;
  Image_ComPtr<IWICStream>        aWicStream;
  if (theData != NULL)
  {
    if (aWicImgFactory->CreateStream(&aWicStream.ChangePtr()) != S_OK
        || aWicStream->InitializeFromMemory((BYTE*)theData, (DWORD)theLength) != S_OK)
    {
      Message::SendFail("Error: cannot initialize WIC Stream");
      return false;
    }
    if (aWicImgFactory->CreateDecoderFromStream(aWicStream.get(),
                                                NULL,
                                                WICDecodeMetadataCacheOnDemand,
                                                &aWicDecoder.ChangePtr())
        != S_OK)
    {
      Message::SendFail("Error: cannot create WIC Image Decoder");
      return false;
    }
  }
  else
  {
    const TCollection_ExtendedString aFileNameW(theFileName);
    if (aWicImgFactory->CreateDecoderFromFilename(aFileNameW.ToWideString(),
                                                  NULL,
                                                  GENERIC_READ,
                                                  WICDecodeMetadataCacheOnDemand,
                                                  &aWicDecoder.ChangePtr())
        != S_OK)
    {
      Message::SendFail("Error: cannot create WIC Image Decoder");
      return false;
    }
  }

  UINT                                aFrameCount = 0, aFrameSizeX = 0, aFrameSizeY = 0;
  WICPixelFormatGUID                  aWicPixelFormat = getNullGuid();
  Image_ComPtr<IWICBitmapFrameDecode> aWicFrameDecode;
  if (aWicDecoder->GetFrameCount(&aFrameCount) != S_OK || aFrameCount < 1
      || aWicDecoder->GetFrame(0, &aWicFrameDecode.ChangePtr()) != S_OK
      || aWicFrameDecode->GetSize(&aFrameSizeX, &aFrameSizeY) != S_OK
      || aWicFrameDecode->GetPixelFormat(&aWicPixelFormat) != S_OK)
  {
    Message::SendFail("Error: cannot get WIC Image Frame");
    return false;
  }

  Image_ComPtr<IWICFormatConverter> aWicConvertedFrame;
  Image_Format                      aPixelFormat = convertFromWicFormat(aWicPixelFormat);
  if (aPixelFormat == Image_Format_UNKNOWN)
  {
    aPixelFormat = Image_Format_RGB;
    if (aWicImgFactory->CreateFormatConverter(&aWicConvertedFrame.ChangePtr()) != S_OK
        || aWicConvertedFrame->Initialize(aWicFrameDecode.get(),
                                          convertToWicFormat(aPixelFormat),
                                          WICBitmapDitherTypeNone,
                                          NULL,
                                          0.0f,
                                          WICBitmapPaletteTypeCustom)
             != S_OK)
    {
      Message::SendFail("Error: cannot convert WIC Image Frame to RGB format");
      return false;
    }
  }

  if (!Image_PixMap::InitTrash(aPixelFormat, aFrameSizeX, aFrameSizeY))
  {
    Message::SendFail("Error: cannot initialize memory for image");
    return false;
  }

  TCollection_AsciiString aFileNameLower = theFileName;
  aFileNameLower.LowerCase();
  if (aFileNameLower.EndsWith(".gif")
      && (aWicImgFactory->CreatePalette(&myPalette) != S_OK
          || aWicFrameDecode->CopyPalette(myPalette) != S_OK))
  {
    Message::SendFail("Error: cannot get palette for GIF image");
    return false;
  }

  IWICBitmapSource* aWicSrc = aWicFrameDecode.get();
  if (!aWicConvertedFrame.IsNull())
  {
    aWicSrc = aWicConvertedFrame.get();
  }

  IWICBitmapFlipRotator* aRotator;
  bool                   isTopDown = true;
  if (aWicImgFactory->CreateBitmapFlipRotator(&aRotator) == S_OK
      && aRotator->Initialize(aWicSrc, WICBitmapTransformFlipVertical) == S_OK)
  {
    isTopDown = false;
    aWicSrc   = aRotator;
  }

  if (aWicSrc->CopyPixels(NULL, (UINT)SizeRowBytes(), (UINT)SizeBytes(), ChangeData()) != S_OK)
  {
    Message::SendFail("Error: cannot copy pixels from WIC Image");
    return false;
  }
  SetTopDown(isTopDown);
  return true;
}

bool Image_AlienPixMap::Load(std::istream& theStream, const TCollection_AsciiString& theFilePath)
{
  Clear();

  // fallback copying stream data into transient buffer
  const std::streamoff aStart = theStream.tellg();
  theStream.seekg(0, std::ios::end);
  const Standard_Integer aLen = Standard_Integer(theStream.tellg() - aStart);
  theStream.seekg(aStart);
  if (aLen <= 0)
  {
    Message::SendFail("Error: empty stream");
    return false;
  }

  NCollection_Array1<Standard_Byte> aBuff(1, aLen);
  if (!theStream.read((char*)&aBuff.ChangeFirst(), aBuff.Size()))
  {
    Message::SendFail("Error: unable to read stream");
    return false;
  }

  return Load(&aBuff.ChangeFirst(), aBuff.Size(), theFilePath);
}
#elif defined(__EMSCRIPTEN__)
bool Image_AlienPixMap::Load(std::istream&, const TCollection_AsciiString&)
{
  Clear();
  Message::SendFail("Error: no image library available for decoding stream");
  return false;
}

bool Image_AlienPixMap::Load(const Standard_Byte*           theData,
                             const Standard_Size            theLength,
                             const TCollection_AsciiString& theImagePath)
{
  Clear();
  if (theData != NULL)
  {
    (void)theLength;
    Message::SendFail("Error: no image library available for decoding in-memory buffer");
    return false;
  }

  int   aSizeX = 0, aSizeY = 0;
  char* anImgData = emscripten_get_preloaded_image_data(theImagePath.ToCString(), &aSizeX, &aSizeY);
  if (anImgData == NULL)
  {
    Message::SendFail() << "Error: image '" << theImagePath << "' is not preloaded";
    return false;
  }

  Image_PixMap::InitWrapper(Image_Format_RGBA, (Standard_Byte*)anImgData, aSizeX, aSizeY);
  SetTopDown(true);
  myLibImage = (FIBITMAP*)anImgData;
  return true;
}
#else
bool Image_AlienPixMap::Load(std::istream&, const TCollection_AsciiString&)
{
  Clear();
  Message::SendFail("Error: no image library available");
  return false;
}

bool Image_AlienPixMap::Load(const Standard_Byte*,
                             const Standard_Size,
                             const TCollection_AsciiString&)
{
  Clear();
  Message::SendFail("Error: no image library available");
  return false;
}
#endif

// =======================================================================
// function : savePPM
// purpose  :
// =======================================================================
bool Image_AlienPixMap::savePPM(const TCollection_AsciiString& theFileName) const
{
  if (IsEmpty())
  {
    return false;
  }

  // Open file
  FILE* aFile = OSD_OpenFile(theFileName.ToCString(), "wb");
  if (aFile == NULL)
  {
    return false;
  }

  // Write header
  fprintf(aFile, "P6\n%d %d\n255\n", (int)SizeX(), (int)SizeY());
  fprintf(aFile, "# Image stored by OpenCASCADE framework in linear RGB colorspace\n");

  // Write pixel data
  Standard_Byte aByte;
  for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
  {
    for (Standard_Size aCol = 0; aCol < SizeX(); ++aCol)
    {
      // extremely SLOW but universal (implemented for all supported pixel formats)
      const Quantity_ColorRGBA aColor = PixelColor((Standard_Integer)aCol, (Standard_Integer)aRow);
      aByte                           = Standard_Byte(aColor.GetRGB().Red() * 255.0);
      fwrite(&aByte, 1, 1, aFile);
      aByte = Standard_Byte(aColor.GetRGB().Green() * 255.0);
      fwrite(&aByte, 1, 1, aFile);
      aByte = Standard_Byte(aColor.GetRGB().Blue() * 255.0);
      fwrite(&aByte, 1, 1, aFile);
    }
  }

  // Close file
  fclose(aFile);
  return true;
}

// =======================================================================
// function : convertData
// purpose  :
// =======================================================================
#ifdef HAVE_WINCODEC
static bool convertData(const Image_AlienPixMap&  theSrcPixMapData,
                        const WICPixelFormatGUID& theFormat,
                        IWICImagingFactory&       theWicImgFactory,
                        Image_PixMapData&         theDstPixMapData)
{
  const UINT aSizeRowBytes = (UINT)theSrcPixMapData.SizeRowBytes();
  const UINT aSizeBytes    = (UINT)theSrcPixMapData.SizeBytes();

  Image_ComPtr<IWICBitmap>          anSrcImg;
  Image_ComPtr<IWICFormatConverter> aWicFormatConverter;
  HRESULT                           anHResult =
    theWicImgFactory.CreateBitmapFromMemory((UINT)theSrcPixMapData.SizeX(),
                                            (UINT)theSrcPixMapData.SizeY(),
                                            convertToWicFormat(theSrcPixMapData.Format()),
                                            aSizeRowBytes,
                                            aSizeBytes,
                                            (BYTE*)theSrcPixMapData.Data(),
                                            &anSrcImg.ChangePtr());
  if (anHResult != S_OK
      || theWicImgFactory.CreateFormatConverter(&aWicFormatConverter.ChangePtr()) != S_OK
      || aWicFormatConverter->Initialize(anSrcImg.get(),
                                         theFormat,
                                         WICBitmapDitherTypeNone,
                                         theSrcPixMapData.GetPalette(),
                                         0.0f,
                                         WICBitmapPaletteTypeCustom)
           != S_OK)
  {
    Message::SendFail("Error: cannot convert WIC Image Frame to required format");
    return false;
  }

  theDstPixMapData.Init(Image_PixMap::DefaultAllocator(),
                        1,
                        theSrcPixMapData.SizeXYZ(),
                        aSizeRowBytes,
                        NULL);

  if (aWicFormatConverter->CopyPixels(NULL,
                                      aSizeRowBytes,
                                      aSizeBytes,
                                      theDstPixMapData.ChangeData())
      != S_OK)
  {
    Message::SendFail("Error: cannot copy pixels from WIC Image");
    return false;
  }

  return true;
}
#endif

// =======================================================================
// function : Save
// purpose  :
// =======================================================================
bool Image_AlienPixMap::Save(Standard_Byte*                 theBuffer,
                             const Standard_Size            theLength,
                             const TCollection_AsciiString& theFileName)
{
#ifdef HAVE_FREEIMAGE
  if (myLibImage == NULL)
  {
    return false;
  }

  #ifdef _WIN32
  const TCollection_ExtendedString aFileNameW(theFileName.ToCString(), Standard_True);
  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilenameU(aFileNameW.ToWideString());
  #else
  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilename(theFileName.ToCString());
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
    FreeImage_FlipVertical(myLibImage);
    SetTopDown(false);
  }

  FIBITMAP* anImageToDump = getImageToDump(anImageFormat);

  if (anImageToDump == NULL)
  {
    return false;
  }

  bool isSaved = false;
  if (theBuffer != NULL)
  {
    // a memory buffer wrapped by FreeImage is read only (images can be loaded but not be saved)
    // so we call FreeImage_OpenMemory() with default arguments and just memcpy in requested buffer.
    FIMEMORY* aFiMem = FreeImage_OpenMemory();
    isSaved          = (FreeImage_SaveToMemory(anImageFormat, anImageToDump, aFiMem) != FALSE);
    BYTE* aData      = NULL;
    DWORD aSize;
    FreeImage_AcquireMemory(aFiMem, &aData, &aSize);
    if (aSize > theLength)
    {
      Message::SendFail("Error: memory buffer too small for storing image");
      return false;
    }
    memcpy(theBuffer, aData, aSize);
    FreeImage_CloseMemory(aFiMem);
  }
  else
  {
  #ifdef _WIN32
    isSaved = (FreeImage_SaveU(anImageFormat, anImageToDump, aFileNameW.ToWideString()) != FALSE);
  #else
    isSaved = (FreeImage_Save(anImageFormat, anImageToDump, theFileName.ToCString()) != FALSE);
  #endif
  }
  if (anImageToDump != myLibImage)
  {
    FreeImage_Unload(anImageToDump);
  }
  return isSaved;

#elif defined(HAVE_WINCODEC)

  TCollection_AsciiString aFileNameLower = theFileName;
  aFileNameLower.LowerCase();
  if (aFileNameLower.EndsWith(".ppm"))
  {
    return savePPM(theFileName);
  }

  GUID aFileFormat = getFileFormatFromName(theFileName);
  if (aFileFormat == getNullGuid())
  {
    Message::SendFail("Error: unsupported image format");
    return false;
  }

  Image_ComPtr<IWICImagingFactory> aWicImgFactory;
  CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (CoCreateInstance(CLSID_WICImagingFactory,
                       NULL,
                       CLSCTX_INPROC_SERVER,
                       IID_PPV_ARGS(&aWicImgFactory.ChangePtr()))
      != S_OK)
  {
    Message::SendFail("Error: cannot initialize WIC Imaging Factory");
    return false;
  }

  WICPixelFormatGUID aWicPixelFormat = convertToWicFormat(myImgFormat);
  if (aWicPixelFormat == getNullGuid())
  {
    Message::SendFail("Error: unsupported pixel format");
    return false;
  }

  Image_PixMapData* aPixMapData = &myData;
  Image_PixMapData  aConvertedData;
  if (aFileFormat == GUID_ContainerFormatGif)
  {
    aWicPixelFormat = GUID_WICPixelFormat8bppIndexed;
    convertData(*this, aWicPixelFormat, *aWicImgFactory, aConvertedData);
    aPixMapData = &aConvertedData;
  }

  Image_ComPtr<IWICStream>         aWicStream;
  Image_ComPtr<IWICBitmapEncoder>  aWicEncoder;
  const TCollection_ExtendedString aFileNameW(theFileName);
  if (theBuffer != NULL)
  {
    if (aWicImgFactory->CreateStream(&aWicStream.ChangePtr()) != S_OK
        || aWicStream->InitializeFromMemory(theBuffer, (DWORD)theLength) != S_OK)
    {
      Message::SendFail("Error: cannot create WIC Memory Stream");
      return false;
    }
  }
  else
  {
    if (aWicImgFactory->CreateStream(&aWicStream.ChangePtr()) != S_OK
        || aWicStream->InitializeFromFilename(aFileNameW.ToWideString(), GENERIC_WRITE) != S_OK)
    {
      Message::SendFail("Error: cannot create WIC File Stream");
      return false;
    }
  }
  if (aWicImgFactory->CreateEncoder(aFileFormat, NULL, &aWicEncoder.ChangePtr()) != S_OK
      || aWicEncoder->Initialize(aWicStream.get(), WICBitmapEncoderNoCache) != S_OK)
  {
    Message::SendFail("Error: cannot create WIC Encoder");
    return false;
  }

  WICPixelFormatGUID                  aWicPixelFormatRes = aWicPixelFormat;
  Image_ComPtr<IWICBitmapFrameEncode> aWicFrameEncode;
  if (aWicEncoder->CreateNewFrame(&aWicFrameEncode.ChangePtr(), NULL) != S_OK
      || aWicFrameEncode->Initialize(NULL) != S_OK
      || aWicFrameEncode->SetSize((UINT)SizeX(), (UINT)SizeY()) != S_OK
      || aWicFrameEncode->SetPixelFormat(&aWicPixelFormatRes) != S_OK)
  {
    Message::SendFail("Error: cannot create WIC Frame");
    return false;
  }

  if (aFileFormat == GUID_ContainerFormatGif
      && (myPalette == NULL || aWicFrameEncode->SetPalette(myPalette) != S_OK))
  {
    Message::SendFail("Error: cannot set palette");
    return false;
  }

  if (aWicPixelFormatRes != aWicPixelFormat)
  {
    Message::SendFail("Error: pixel format is unsupported by image format");
    return false;
  }

  if (IsTopDown())
  {
    if (aWicFrameEncode->WritePixels((UINT)SizeY(),
                                     (UINT)SizeRowBytes(),
                                     (UINT)SizeBytes(),
                                     (BYTE*)aPixMapData->Data())
        != S_OK)
    {
      Message::SendFail("Error: cannot write pixels to WIC Frame");
      return false;
    }
  }
  else
  {
    for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
    {
      if (aWicFrameEncode->WritePixels(1,
                                       (UINT)SizeRowBytes(),
                                       (UINT)SizeRowBytes(),
                                       (BYTE*)aPixMapData->Row(aRow))
          != S_OK)
      {
        Message::SendFail("Error: cannot write pixels to WIC Frame");
        return false;
      }
    }
  }
  if (aWicFrameEncode->Commit() != S_OK || aWicEncoder->Commit() != S_OK)
  {
    Message::SendFail("Error: cannot commit data to WIC Frame");
    return false;
  }
  if (aWicStream->Commit(STGC_DEFAULT) != S_OK)
  {
    // Message::Send ("Error: cannot commit data to WIC File Stream", Message_Fail);
    // return false;
  }
  return true;
#else
  (void)theLength;
  if (theBuffer != NULL)
  {
    Message::SendFail("Error: no image library available");
    return false;
  }
  const Standard_Integer aLen = theFileName.Length();
  if ((aLen >= 4) && (theFileName.Value(aLen - 3) == '.')
      && strcasecmp(theFileName.ToCString() + aLen - 3, "ppm") == 0)
  {
    return savePPM(theFileName);
  }
  Message::SendTrace("Image_PixMap, no image library available! Image saved in PPM format");
  return savePPM(theFileName);
#endif
}

bool Image_AlienPixMap::Save(std::ostream& theStream, const TCollection_AsciiString& theExtension)
{
#ifdef HAVE_FREEIMAGE
  if (myLibImage == NULL)
  {
    return false;
  }

  #ifdef _WIN32
  const TCollection_ExtendedString anExtW(theExtension.ToCString(), Standard_True);
  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilenameU(anExtW.ToWideString());
  #else
  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilename(theExtension.ToCString());
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
    FreeImage_FlipVertical(myLibImage);
    SetTopDown(false);
  }

  FIBITMAP* anImageToDump = getImageToDump(anImageFormat);

  if (anImageToDump == NULL)
  {
    return false;
  }

  bool                  isSaved = false;
  Image_FreeImageStream aStream(theStream);
  FreeImageIO           anIO = aStream.GetFiIO();

  isSaved = (FreeImage_SaveToHandle(anImageFormat, anImageToDump, &anIO, &aStream) != FALSE);

  if (anImageToDump != myLibImage)
  {
    FreeImage_Unload(anImageToDump);
  }
  return isSaved;
#elif defined(HAVE_WINCODEC)
  GUID aFileFormat = getFileFormatFromName(theExtension);
  if (aFileFormat == getNullGuid())
  {
    Message::SendFail("Error: unsupported image format");
    return false;
  }

  Image_ComPtr<IWICImagingFactory> aWicImgFactory;
  CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (CoCreateInstance(CLSID_WICImagingFactory,
                       NULL,
                       CLSCTX_INPROC_SERVER,
                       IID_PPV_ARGS(&aWicImgFactory.ChangePtr()))
      != S_OK)
  {
    Message::SendFail("Error: cannot initialize WIC Imaging Factory");
    return false;
  }

  WICPixelFormatGUID aWicPixelFormat = convertToWicFormat(myImgFormat);
  if (aWicPixelFormat == getNullGuid())
  {
    Message::SendFail("Error: unsupported pixel format");
    return false;
  }

  Image_PixMapData* aPixMapData = &myData;
  Image_PixMapData  aConvertedData;
  if (aFileFormat == GUID_ContainerFormatGif)
  {
    aWicPixelFormat = GUID_WICPixelFormat8bppIndexed;
    convertData(*this, aWicPixelFormat, *aWicImgFactory, aConvertedData);
    aPixMapData = &aConvertedData;
  }

  Image_ComPtr<IStream>           aStream;
  Image_ComPtr<IWICBitmapEncoder> aWicEncoder;

  if (CreateStreamOnHGlobal(NULL, Standard_True, &aStream.ChangePtr()) != S_OK)
  {
    Message::SendFail("Error: cannot create Stream on global");
    return false;
  }

  if (aWicImgFactory->CreateEncoder(aFileFormat, NULL, &aWicEncoder.ChangePtr()) != S_OK
      || aWicEncoder->Initialize(aStream.get(), WICBitmapEncoderNoCache) != S_OK)
  {
    Message::SendFail("Error: cannot create WIC Encoder");
    return false;
  }

  WICPixelFormatGUID                  aWicPixelFormatRes = aWicPixelFormat;
  Image_ComPtr<IWICBitmapFrameEncode> aWicFrameEncode;
  if (aWicEncoder->CreateNewFrame(&aWicFrameEncode.ChangePtr(), NULL) != S_OK
      || aWicFrameEncode->Initialize(NULL) != S_OK
      || aWicFrameEncode->SetSize((UINT)SizeX(), (UINT)SizeY()) != S_OK
      || aWicFrameEncode->SetPixelFormat(&aWicPixelFormatRes) != S_OK)
  {
    Message::SendFail("Error: cannot create WIC Frame");
    return false;
  }

  if (aFileFormat == GUID_ContainerFormatGif
      && (myPalette == NULL || aWicFrameEncode->SetPalette(myPalette) != S_OK))
  {
    Message::SendFail("Error: cannot set palette");
    return false;
  }

  if (aWicPixelFormatRes != aWicPixelFormat)
  {
    Message::SendFail("Error: pixel format is unsupported by image format");
    return false;
  }

  if (IsTopDown())
  {
    if (aWicFrameEncode->WritePixels((UINT)SizeY(),
                                     (UINT)SizeRowBytes(),
                                     (UINT)SizeBytes(),
                                     (BYTE*)aPixMapData->Data())
        != S_OK)
    {
      Message::SendFail("Error: cannot write pixels to WIC Frame");
      return false;
    }
  }
  else
  {
    for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
    {
      if (aWicFrameEncode->WritePixels(1,
                                       (UINT)SizeRowBytes(),
                                       (UINT)SizeRowBytes(),
                                       (BYTE*)aPixMapData->Row(aRow))
          != S_OK)
      {
        Message::SendFail("Error: cannot write pixels to WIC Frame");
        return false;
      }
    }
  }
  if (aWicFrameEncode->Commit() != S_OK || aWicEncoder->Commit() != S_OK)
  {
    Message::SendFail("Error: cannot commit data to WIC Frame");
    return false;
  }
  if (aStream->Commit(STGC_DEFAULT) != S_OK)
  {
    // Message::Send ("Error: cannot commit data to Stream", Message_Fail);
    // return false;
  }

  // WIC doesn't have the way to encode image directly in std::ostream
  // so we use a workaround to transfer data from IStream to std::ostream
  STATSTG aStat;
  if (aStream->Stat(&aStat, STATFLAG_NONAME) != S_OK)
  {
    Message::SendFail("Error: cannot get stat from stream");
    return false;
  }
  HGLOBAL aMem;
  if (GetHGlobalFromStream(aStream.get(), &aMem) != S_OK)
  {
    Message::SendFail("Error: cannot get global from stream");
    return false;
  }

  LPVOID aData = GlobalLock(aMem);
  if (aData == NULL)
  {
    Message::SendFail("Error: cannot lock global");
    return false;
  }
  if (!theStream.write((const char*)aData, aStat.cbSize.QuadPart))
  {
    Message::SendFail("Error: cannot write data to ostream");
    return false;
  }
  if (GlobalUnlock(aMem) == 0 && GetLastError() != NO_ERROR)
  {
    Message::SendFail("Error: cannot unlock global");
    return false;
  }
  return true;
#else
  (void)theExtension;
  (void)theStream;
  Message::SendFail("Error: no image library available");
  return false;
#endif
}

// =======================================================================
// function : AdjustGamma
// purpose  :
// =======================================================================
bool Image_AlienPixMap::AdjustGamma(const Standard_Real theGammaCorr)
{
#ifdef HAVE_FREEIMAGE
  return FreeImage_AdjustGamma(myLibImage, theGammaCorr) != FALSE;
#else
  (void)theGammaCorr;
  return false;
#endif
}

#ifdef HAVE_FREEIMAGE
// =======================================================================
// function : GetImageToDump
// purpose  :
// =======================================================================
FIBITMAP* Image_AlienPixMap::getImageToDump(const Standard_Integer theFormat)
{
  FIBITMAP* anImageToDump = myLibImage;
  // FreeImage doesn't provide flexible format conversion API
  // so we should perform multiple conversions in some cases!
  switch (theFormat)
  {
    case FIF_PNG:
    case FIF_BMP: {
      if (Format() == Image_Format_BGR32 || Format() == Image_Format_RGB32)
      {
        // stupid FreeImage treats reserved byte as alpha if some bytes not set to 0xFF
        for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < SizeX(); ++aCol)
          {
            myData.ChangeValue(aRow, aCol)[3] = 0xFF;
          }
        }
      }
      else if (FreeImage_GetImageType(myLibImage) != FIT_BITMAP)
      {
        anImageToDump = FreeImage_ConvertToType(myLibImage, FIT_BITMAP);
      }
      break;
    }
    case FIF_GIF: {
      FIBITMAP* aTmpBitmap = myLibImage;
      if (FreeImage_GetImageType(myLibImage) != FIT_BITMAP)
      {
        aTmpBitmap = FreeImage_ConvertToType(myLibImage, FIT_BITMAP);
        if (aTmpBitmap == NULL)
        {
          return NULL;
        }
      }

      if (FreeImage_GetBPP(aTmpBitmap) != 24)
      {
        FIBITMAP* aTmpBitmap24 = FreeImage_ConvertTo24Bits(aTmpBitmap);
        if (aTmpBitmap != myLibImage)
        {
          FreeImage_Unload(aTmpBitmap);
        }
        if (aTmpBitmap24 == NULL)
        {
          return NULL;
        }
        aTmpBitmap = aTmpBitmap24;
      }

      // need conversion to image with palette (requires 24bit bitmap)
      anImageToDump = FreeImage_ColorQuantize(aTmpBitmap, FIQ_NNQUANT);
      if (aTmpBitmap != myLibImage)
      {
        FreeImage_Unload(aTmpBitmap);
      }
      break;
    }
    case FIF_HDR:
    case FIF_EXR: {
      if (Format() == Image_Format_Gray || Format() == Image_Format_Alpha)
      {
        anImageToDump = FreeImage_ConvertToType(myLibImage, FIT_FLOAT);
      }
      else if (Format() == Image_Format_RGBA || Format() == Image_Format_BGRA)
      {
        anImageToDump = FreeImage_ConvertToType(myLibImage, FIT_RGBAF);
      }
      else
      {
        FREE_IMAGE_TYPE aImgTypeFI = FreeImage_GetImageType(myLibImage);
        if (aImgTypeFI != FIT_RGBF && aImgTypeFI != FIT_RGBAF && aImgTypeFI != FIT_FLOAT)
        {
          anImageToDump = FreeImage_ConvertToType(myLibImage, FIT_RGBF);
        }
      }
      break;
    }
    default: {
      if (FreeImage_GetImageType(myLibImage) != FIT_BITMAP)
      {
        anImageToDump = FreeImage_ConvertToType(myLibImage, FIT_BITMAP);
        if (anImageToDump == NULL)
        {
          return NULL;
        }
      }

      if (FreeImage_GetBPP(anImageToDump) != 24)
      {
        FIBITMAP* aTmpBitmap24 = FreeImage_ConvertTo24Bits(anImageToDump);
        if (anImageToDump != myLibImage)
        {
          FreeImage_Unload(anImageToDump);
        }
        if (aTmpBitmap24 == NULL)
        {
          return NULL;
        }
        anImageToDump = aTmpBitmap24;
      }
      break;
    }
  }
  return anImageToDump;
}
#endif
