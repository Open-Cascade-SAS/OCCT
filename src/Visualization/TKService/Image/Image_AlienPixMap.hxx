// Created on: 2012-07-18
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef Image_AlienPixMap_HeaderFile
#define Image_AlienPixMap_HeaderFile

#include <Image_PixMap.hxx>

class TCollection_AsciiString;
struct IWICPalette;
struct FIBITMAP;

//! Image class that support file reading/writing operations using auxiliary image library.
//! Supported image formats:
//! - *.bmp - bitmap image, lossless format without compression.
//! - *.ppm - PPM (Portable Pixmap Format), lossless format without compression.
//! - *.png - PNG (Portable Network Graphics) lossless format with compression.
//! - *.jpg, *.jpe, *.jpeg - JPEG/JIFF (Joint Photographic Experts Group) lossy format (compressed
//! with quality losses). YUV color space used (automatically converted from/to RGB).
//! - *.tif, *.tiff - TIFF (Tagged Image File Format).
//! - *.tga - TGA (Truevision Targa Graphic), lossless format.
//! - *.gif - GIF (Graphical Interchange Format), lossy format. Color stored using palette (up to
//! 256 distinct colors).
//! - *.exr - OpenEXR high dynamic-range format (supports float pixel formats).
class Image_AlienPixMap : public Image_PixMap
{
  DEFINE_STANDARD_RTTIEXT(Image_AlienPixMap, Image_PixMap)
public:
  //! Return default rows order used by underlying image library.
  Standard_EXPORT static bool IsTopDownDefault();

public:
  //! Empty constructor.
  Standard_EXPORT Image_AlienPixMap();

  //! Destructor
  Standard_EXPORT virtual ~Image_AlienPixMap();

  //! Read image data from file.
  bool Load(const TCollection_AsciiString& theFileName) { return Load(NULL, 0, theFileName); }

  //! Read image data from stream.
  Standard_EXPORT bool Load(std::istream& theStream, const TCollection_AsciiString& theFileName);

  //! Read image data from memory buffer.
  //! @param[in] theData     memory pointer to read from;
  //!                        when NULL, function will attempt to open theFileName file
  //! @param[in] theLength   memory buffer length
  //! @param[in] theFileName optional file name
  Standard_EXPORT bool Load(const Standard_Byte*           theData,
                            const Standard_Size            theLength,
                            const TCollection_AsciiString& theFileName);

  //! Write image data to file.
  //! @param[in] theFileName file name to save
  bool Save(const TCollection_AsciiString& theFileName) { return Save(NULL, 0, theFileName); }

  //! Write image data to stream.
  //! @param[out] theStream   stream where to write
  //! @param[in] theExtension image format
  Standard_EXPORT bool Save(std::ostream& theStream, const TCollection_AsciiString& theExtension);

  //! Write image data to file or memory buffer using file extension to determine format.
  //! @param[out] theBuffer  buffer pointer where to write
  //!                        when NULL, function write image data to theFileName file
  //! @param[in] theLength   memory buffer length
  //! @param[in] theFileName file name to save;
  //!                        when theBuffer isn't NULL used only to determine format
  Standard_EXPORT bool Save(Standard_Byte*                 theBuffer,
                            const Standard_Size            theLength,
                            const TCollection_AsciiString& theFileName);

  //! Initialize image plane with required dimensions.
  //! @param[in] thePixelFormat  if specified pixel format doesn't supported by image library
  //!                            than nearest supported will be used instead!
  //! @param[in] theSizeRowBytes may be ignored by this class and required alignment will be used
  //! instead!
  Standard_EXPORT virtual bool InitTrash(Image_Format        thePixelFormat,
                                         const Standard_Size theSizeX,
                                         const Standard_Size theSizeY,
                                         const Standard_Size theSizeRowBytes = 0) Standard_OVERRIDE;

  //! Initialize by copying data.
  Standard_EXPORT virtual bool InitCopy(const Image_PixMap& theCopy) Standard_OVERRIDE;

  //! Method correctly deallocate internal buffer.
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Performs gamma correction on image.
  //! @param[in] theGamma - gamma value to use; a value of 1.0 leaves the image alone
  Standard_EXPORT bool AdjustGamma(const Standard_Real theGammaCorr);

#if !defined(HAVE_FREEIMAGE) && defined(_WIN32)
  //! Returns image palette.
  IWICPalette* GetPalette() const { return myPalette; }
#endif

private:
  //! Copying allowed only within Handles
  Image_AlienPixMap(const Image_AlienPixMap&);
  Image_AlienPixMap& operator=(const Image_AlienPixMap&);

  //! Wrapper initialization is disallowed for this class (will return false in any case)!
  //! Use only copying and allocation initializers.
  Standard_EXPORT virtual bool InitWrapper(Image_Format        thePixelFormat,
                                           Standard_Byte*      theDataPtr,
                                           const Standard_Size theSizeX,
                                           const Standard_Size theSizeY,
                                           const Standard_Size theSizeRowBytes) Standard_OVERRIDE;

  //! Built-in PPM export
  Standard_EXPORT bool savePPM(const TCollection_AsciiString& theFileName) const;

  FIBITMAP* getImageToDump(const Standard_Integer theFormat);

private:
  FIBITMAP*    myLibImage;
  IWICPalette* myPalette;
};

DEFINE_STANDARD_HANDLE(Image_AlienPixMap, Image_PixMap)

#endif // _Image_AlienPixMap_H__
