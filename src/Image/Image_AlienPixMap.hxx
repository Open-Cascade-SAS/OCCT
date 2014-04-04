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

#ifndef _Image_AlienPixMap_H__
#define _Image_AlienPixMap_H__

#include <Image_PixMap.hxx>

class TCollection_AsciiString;
struct FIBITMAP;

//! Image class that support file reading/writing operations using auxiliary image library.
//! Supported image formats:
//! - *.bmp - bitmap image, lossless format without compression.
//! - *.ppm - PPM (Portable Pixmap Format), lossless format without compression.
//! - *.png - PNG (Portable Network Graphics) lossless format with compression.
//! - *.jpg, *.jpe, *.jpeg - JPEG/JIFF (Joint Photographic Experts Group) lossy format (compressed with quality losses). YUV color space used (automatically converted from/to RGB).
//! - *.tif, *.tiff - TIFF (Tagged Image File Format).
//! - *.tga - TGA (Truevision Targa Graphic), lossless format.
//! - *.gif - GIF (Graphical Interchange Format), lossy format. Color stored using pallete (up to 256 distinct colors).
//! - *.exr - OpenEXR high dynamic-range format (supports float pixel formats). 
class Image_AlienPixMap : public Image_PixMap
{

public:

  //! Empty constructor.
  Standard_EXPORT Image_AlienPixMap();

  //! Destructor
  Standard_EXPORT virtual ~Image_AlienPixMap();

  //! Read image data from file.
  Standard_EXPORT bool Load (const TCollection_AsciiString& theFileName);

  //! Write image data to file using file extension to determine compression format.
  Standard_EXPORT bool Save (const TCollection_AsciiString& theFileName);

  //! Initialize image plane with required dimensions.
  //! thePixelFormat - if specified pixel format doesn't supported by image library
  //!                  than nearest supported will be used instead!
  //! theSizeRowBytes - may be ignored by this class and required alignemnt will be used instead!
  Standard_EXPORT virtual bool InitTrash (ImgFormat           thePixelFormat,
                                          const Standard_Size theSizeX,
                                          const Standard_Size theSizeY,
                                          const Standard_Size theSizeRowBytes = 0);

  //! Initialize by copying data.
  Standard_EXPORT virtual bool InitCopy (const Image_PixMap& theCopy);

  //! Method correctly deallocate internal buffer.
  Standard_EXPORT virtual void Clear();

  //! Performs gamma correction on image.
  //! theGamma - gamma value to use; a value of 1.0 leaves the image alone
  Standard_EXPORT bool AdjustGamma (const Standard_Real theGammaCorr);

private:

  FIBITMAP* myLibImage;

private:

  //! Copying allowed only within Handles
  Image_AlienPixMap            (const Image_AlienPixMap& );
  Image_AlienPixMap& operator= (const Image_AlienPixMap& );

  //! Wrapper initialization is disallowed for this class (will return false in any case)!
  //! Use only copying and allocation initializers.
  Standard_EXPORT virtual bool InitWrapper (ImgFormat           thePixelFormat,
                                            Standard_Byte*      theDataPtr,
                                            const Standard_Size theSizeX,
                                            const Standard_Size theSizeY,
                                            const Standard_Size theSizeRowBytes);

  //! Built-in PPM export
  Standard_EXPORT bool savePPM (const TCollection_AsciiString& theFileName) const;

public:

  DEFINE_STANDARD_RTTI(Image_AlienPixMap) // Type definition

};

DEFINE_STANDARD_HANDLE(Image_AlienPixMap, Image_PixMap)

#endif // _Image_AlienPixMap_H__
