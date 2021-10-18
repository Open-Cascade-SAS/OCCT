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

#ifndef Graphic3d_MarkerImage_HeaderFile
#define Graphic3d_MarkerImage_HeaderFile

#include <Aspect_TypeOfMarker.hxx>
#include <Graphic3d_Vec4.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TCollection_AsciiString.hxx>

class Image_PixMap;

//! This class is used to store bitmaps and images for markers rendering.
//! It can convert bitmap texture stored in TColStd_HArray1OfByte to Image_PixMap and vice versa.
class Graphic3d_MarkerImage : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_MarkerImage, Standard_Transient)
public:

  //! Returns a marker image for the marker of the specified type, scale and color.
  Standard_EXPORT static Handle(Graphic3d_MarkerImage) StandardMarker (const Aspect_TypeOfMarker theMarkerType,
                                                                       const Standard_ShortReal  theScale,
                                                                       const Graphic3d_Vec4& theColor);

public:

  //! Constructor from existing pixmap.
  //! @param theImage [in] source image
  //! @param theImageAlpha [in] colorless image
  Standard_EXPORT Graphic3d_MarkerImage (const Handle(Image_PixMap)& theImage,
                                         const Handle(Image_PixMap)& theImageAlpha = Handle(Image_PixMap)());

  //! Creates marker image from array of bytes
  //! (method for compatibility with old markers definition).
  //! @param theBitMap [in] source bitmap stored as array of bytes
  //! @param theWidth  [in] number of bits in a row
  //! @param theHeight [in] number of bits in a column
  Standard_EXPORT Graphic3d_MarkerImage (const Handle(TColStd_HArray1OfByte)& theBitMap,
                                         const Standard_Integer theWidth,
                                         const Standard_Integer theHeight);

  //! Return marker image.
  //! If an instance of the class has been initialized with a bitmap, it will be converted to image.
  Standard_EXPORT const Handle(Image_PixMap)& GetImage();

  //! Return image alpha as grayscale image.
  //! Note that if an instance of the class has been initialized with a bitmap
  //! or with grayscale image this method will return exactly the same image as GetImage()
  Standard_EXPORT const Handle(Image_PixMap)& GetImageAlpha();

  //! Return an unique ID.
  //! This ID will be used to manage resource in graphic driver.
  Standard_EXPORT const TCollection_AsciiString& GetImageId() const;

  //! Return an unique ID.
  //! This ID will be used to manage resource in graphic driver.
  Standard_EXPORT const TCollection_AsciiString& GetImageAlphaId() const;

  //! Return texture size
  Standard_EXPORT void GetTextureSize (Standard_Integer& theWidth,
                                       Standard_Integer& theHeight) const;

  //! Return TRUE if marker image has colors (e.g. RGBA and not grayscale).
  Standard_EXPORT bool IsColoredImage() const;

  //! Return marker image as array of bytes.
  //! If an instance of the class has been initialized with image, it will be converted to bitmap based on the parameter theAlphaValue.
  //! @param theAlphaValue pixels in the image that have alpha value greater than
  //!                      or equal to this parameter will be stored in bitmap as "1",
  //!                      others will be stored as "0"
  //! @param theIsTopDown [in] flag indicating expected rows order in returned bitmap, which is bottom-up by default
  Standard_EXPORT Handle(TColStd_HArray1OfByte) GetBitMapArray (const Standard_Real theAlphaValue = 0.5,
                                                                const Standard_Boolean theIsTopDown = false) const;

protected:

  //! Constructor from existing pixmap with predefined ids.
  Standard_EXPORT Graphic3d_MarkerImage (const TCollection_AsciiString& theId,
                                         const TCollection_AsciiString& theAlphaId,
                                         const Handle(Image_PixMap)& theImage,
                                         const Handle(Image_PixMap)& theImageAlpha = Handle(Image_PixMap)());

private:

  TCollection_AsciiString       myImageId;      //!< resource identifier
  TCollection_AsciiString       myImageAlphaId; //!< resource identifier
  Handle(TColStd_HArray1OfByte) myBitMap;       //!< bytes array with bitmap definition (for compatibility with old code)
  Handle(Image_PixMap)          myImage;        //!< full-color  marker definition
  Handle(Image_PixMap)          myImageAlpha;   //!< alpha-color marker definition (for dynamic hi-lighting)
  Standard_Integer              myMargin;       //!< extra margin from boundaries for bitmap -> point sprite conversion, 1 px by default
  Standard_Integer              myWidth;        //!< marker width
  Standard_Integer              myHeight;       //!< marker height

};

DEFINE_STANDARD_HANDLE (Graphic3d_MarkerImage, Standard_Transient)

#endif // _Graphic3d_MarkerImage_H__
