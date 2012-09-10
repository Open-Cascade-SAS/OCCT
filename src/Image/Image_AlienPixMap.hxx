// Created on: 2012-07-18
// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
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

#ifndef _Image_AlienPixMap_H__
#define _Image_AlienPixMap_H__

#include <Image_PixMap.hxx>
#include <Image_TypeOfImage.hxx>

class TCollection_AsciiString;
struct FIBITMAP;

//! Image class that support file reading/writing operations using auxiliary image library.
//! Notice that supported images format could be limited.
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
  Standard_EXPORT virtual void Clear (ImgFormat thePixelFormat = ImgGray);

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
