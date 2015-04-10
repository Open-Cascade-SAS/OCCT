// Created on: 2013-06-25
// Created by: Dmitry BOBYLEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_MarkerImage_H__
#define _Graphic3d_MarkerImage_H__

#include <TColStd_HArray1OfByte.hxx>
#include <Image_PixMap_Handle.hxx>
#include <Quantity_Parameter.hxx>
#include <TCollection_AsciiString.hxx>

//! This class is used to store bitmaps and images for markers rendering.
//! It can convert bitmap texture stored in TColStd_HArray1OfByte to Image_PixMap and vice versa.
class Graphic3d_MarkerImage : public Standard_Transient
{
public:

  //! @param theImage - source image
  Standard_EXPORT Graphic3d_MarkerImage (const Handle(Image_PixMap)& theImage);

  //! Creates marker image from array of bytes
  //! (method for compatibility with old markers definition).
  //! @param theBitMap - source bitmap stored as array of bytes
  //! @param theWidth  - number of bits in a row
  //! @param theHeight - number of bits in a column
  Standard_EXPORT Graphic3d_MarkerImage (const Handle(TColStd_HArray1OfByte)& theBitMap,
                                         const Standard_Integer& theWidth,
                                         const Standard_Integer& theHeight);

  //! @param theAlphaValue pixels in the image that have alpha value greater than
  //!                      or equal to this parameter will be stored in bitmap as "1",
  //!                      others will be stored as "0"
  //! @return marker image as array of bytes. If an instance of the class has been
  //! initialized with image, it will be converted to bitmap based on the parameter theAlphaValue.
  Standard_EXPORT Handle(TColStd_HArray1OfByte) GetBitMapArray (const Standard_Real& theAlphaValue = 0.5) const;

  //! @return marker image. If an instance of the class has been initialized with a bitmap, it will be
  //! converted to image.
  Standard_EXPORT const Handle(Image_PixMap)& GetImage();

  //! @return image alpha as grayscale image. Note that if an instance of the class has been initialized
  //! with a bitmap or with grayscale image this method will return exactly the same image as GetImage()
  Standard_EXPORT const Handle(Image_PixMap)& GetImageAlpha();

  //! @return an unique ID. This ID will be used to manage resource in graphic driver.
  Standard_EXPORT const TCollection_AsciiString& GetImageId() const;

  //! @return an unique ID. This ID will be used to manage resource in graphic driver.
  Standard_EXPORT const TCollection_AsciiString& GetImageAlphaId() const;

  //! @return texture size
  Standard_EXPORT void GetTextureSize (Standard_Integer& theWidth,
                                       Standard_Integer& theHeight) const;

private:

  TCollection_AsciiString       myImageId;      //!< resource identifier
  TCollection_AsciiString       myImageAlphaId; //!< resource identifier
  Handle(TColStd_HArray1OfByte) myBitMap;       //!< bytes array with bitmap definition (for compatibility with old code)
  Handle(Image_PixMap)          myImage;        //!< full-color  marker definition
  Handle(Image_PixMap)          myImageAlpha;   //!< alpha-color marker definition (for dynamic hi-lighting)
  Standard_Integer              myMargin;       //!< extra margin from boundaries for bitmap -> point sprite conversion, 1 px by default
  Standard_Integer              myWidth;        //!< marker width
  Standard_Integer              myHeight;       //!< marker height

public:

  DEFINE_STANDARD_RTTI(Graphic3d_MarkerImage) // Type definition

};

DEFINE_STANDARD_HANDLE (Graphic3d_MarkerImage, Standard_Transient)

#endif // _Graphic3d_MarkerImage_H__
