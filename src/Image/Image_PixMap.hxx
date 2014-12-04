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

#ifndef _Image_PixMap_H__
#define _Image_PixMap_H__

#include <Image_PixMapData.hxx>
#include <Image_PixMap_Handle.hxx>
#include <Standard_Transient.hxx>
#include <Quantity_Color.hxx>

//! Class represents packed image plane.
class Image_PixMap : public Standard_Transient
{

public:

  //! This enumeration define packed image plane formats
  typedef enum tagFormat {
      ImgUNKNOWN = 0, //!< unsupported or unknown format
      ImgGray    = 1, //!< 1 byte per pixel, intensity of the color
      ImgAlpha,       //!< 1 byte per pixel, transparency
      ImgRGB,         //!< 3 bytes packed RGB image plane
      ImgBGR,         //!< same as RGB but with different components order
      ImgRGB32,       //!< 4 bytes packed RGB image plane (1 extra byte for alignment, may have undefined value)
      ImgBGR32,       //!< same as RGB but with different components order
      ImgRGBA,        //!< 4 bytes packed RGBA image plane
      ImgBGRA,        //!< same as RGBA but with different components order
      ImgGrayF,       //!< 1 float  (4-bytes) per pixel (1-component plane), intensity of the color
      ImgAlphaF,      //!< 1 float  (4-bytes) per pixel (1-component plane), transparency
      ImgRGBF,        //!< 3 floats (12-bytes) RGB image plane
      ImgBGRF,        //!< same as RGBF but with different components order
      ImgRGBAF,       //!< 4 floats (16-bytes) RGBA image plane
      ImgBGRAF,       //!< same as RGBAF but with different components order
  } ImgFormat;

  //! Determine Big-Endian at runtime
  static inline bool IsBigEndianHost()
  {
    union { int myInt; char myChar[sizeof(int)]; } aUnion;
    aUnion.myInt = 1;
    return !aUnion.myChar[0];
  }

public: // high-level API

  inline ImgFormat Format() const
  {
    return myImgFormat;
  }

  //! Override pixel format specified by InitXXX() methods.
  //! Will throw exception if pixel size of new format is not equal to currently initialized format.
  //! Intended to switch formats indicating different interpretation of the same data
  //! (e.g. ImgGray and ImgAlpha).
  Standard_EXPORT void SetFormat (const ImgFormat thePixelFormat);

  //! @return image width in pixels
  inline Standard_Size Width() const
  {
    return myData.SizeX;
  }

  //! @return image height in pixels
  inline Standard_Size Height() const
  {
    return myData.SizeY;
  }

  //! @return image width in pixels
  inline Standard_Size SizeX() const
  {
    return myData.SizeX;
  }

  //! @return image height in pixels
  inline Standard_Size SizeY() const
  {
    return myData.SizeY;
  }

  //! @return width / height.
  inline Standard_Real Ratio() const
  {
    return (SizeY() > 0) ? (Standard_Real(SizeX()) / Standard_Real(SizeY())) : 1.0;
  }

  //! @return true if data is NULL.
  bool IsEmpty() const
  {
    return myData.IsEmpty();
  }

  //! Empty constructor. Initialize the NULL image plane.
  Standard_EXPORT Image_PixMap();

  //! Destructor
  Standard_EXPORT virtual ~Image_PixMap();

  //! Returns the pixel color. This function is relatively slow.
  //! @param theX - column index from left
  //! @param theY - row    index from top
  //! @return the pixel color
  inline Quantity_Color PixelColor (const Standard_Integer theX,
                                    const Standard_Integer theY) const
  {
    Quantity_Parameter aDummy;
    return PixelColor (theX, theY, aDummy);
  }

  //! Returns the pixel color. This function is relatively slow.
  //! theAlpha argument is set to color intensity (0 - transparent, 1 - opaque)
  Standard_EXPORT Quantity_Color PixelColor (const Standard_Integer theX,
                                             const Standard_Integer theY,
                                             Quantity_Parameter&    theAlpha) const;

  //! Initialize image plane as wrapper over alien data.
  //! Data will not be copied! Notice that caller should ensure
  //! that data pointer will not be released during this wrapper lifetime.
  //! You may call InitCopy() to perform data copying.
  Standard_EXPORT virtual bool InitWrapper (ImgFormat            thePixelFormat,
                                            Standard_Byte*       theDataPtr,
                                            const Standard_Size  theSizeX,
                                            const Standard_Size  theSizeY,
                                            const Standard_Size  theSizeRowBytes = 0);

  //! Initialize image plane with required dimensions.
  //! Memory will be left uninitialized (performance trick).
  Standard_EXPORT virtual bool InitTrash (ImgFormat           thePixelFormat,
                                          const Standard_Size theSizeX,
                                          const Standard_Size theSizeY,
                                          const Standard_Size theSizeRowBytes = 0);

  //! Initialize by copying data.
  //! If you want to copy alien data you should create wrapper using InitWrapper() before.
  Standard_EXPORT virtual bool InitCopy (const Image_PixMap& theCopy);

  //! Initialize image plane with required dimensions.
  //! Buffer will be zeroed (black color for most formats).
  Standard_EXPORT bool InitZero (ImgFormat           thePixelFormat,
                                 const Standard_Size theSizeX,
                                 const Standard_Size theSizeY,
                                 const Standard_Size theSizeRowBytes = 0,
                                 const Standard_Byte theValue = 0);

  //! Method correctly deallocate internal buffer.
  Standard_EXPORT virtual void Clear();

public: //! @name low-level API for batch-processing (pixels reading / comparison / modification)

  //! Returns TRUE if image data is stored from Top to the Down.
  //! By default Bottom Up order is used instead
  //! (topmost scanlines starts from the bottom in memory).
  //! which is most image frameworks naturally support.
  //!
  //! Notice that access methods within this class automatically
  //! convert input row-index to apply this flag!
  //! You should use this flag only if interconnect with alien APIs and buffers.
  //! @return true if image data is top-down
  inline bool IsTopDown() const
  {
    return myData.TopToDown == 1;
  }

  //! Setup scanlines order in memory - top-down or bottom-up.
  //! Drawers should explicitly specify this value if current state IsTopDown() was ignored!
  //! @param theIsTopDown top-down flag
  inline void SetTopDown (const bool theIsTopDown)
  {
    myData.SetTopDown (theIsTopDown);
  }

  //! Returns +1 if scanlines ordered in Top->Down order in memory and -1 otherwise.
  //! @return scanline increment for Top->Down iteration
  inline Standard_Size TopDownInc() const
  {
    return myData.TopToDown;
  }

  //! @return data pointer for low-level operations (copying entire buffer, parsing with extra tools etc.).
  inline const Standard_Byte* Data() const
  {
    return myData.Data();
  }

  //! @return data pointer for low-level operations (copying entire buffer, parsing with extra tools etc.).
  inline Standard_Byte* ChangeData()
  {
    return myData.ChangeData();
  }

  //! @return data pointer to requested row (first column).
  inline const Standard_Byte* Row (const Standard_Size theRow) const
  {
    return myData.Row (theRow);
  }

  //! @return data pointer to requested row (first column).
  inline Standard_Byte* ChangeRow (const Standard_Size theRow)
  {
    return myData.ChangeRow (theRow);
  }

  //! @return bytes reserved for one pixel (may include extra bytes for alignment).
  inline Standard_Size SizePixelBytes() const
  {
    return myData.SizeBPP;
  }

  //! @return bytes reserved for one pixel (may include extra bytes for alignment).
  Standard_EXPORT static Standard_Size SizePixelBytes (const Image_PixMap::ImgFormat thePixelFormat);

  //! @return bytes reserved per row.
  //! Could be larger than needed to store packed row (extra bytes for alignment etc.).
  inline Standard_Size SizeRowBytes() const
  {
    return myData.SizeRowBytes;
  }

  //! @return the extra bytes in the row.
  inline Standard_Size RowExtraBytes() const
  {
    return SizeRowBytes() - SizeX() * SizePixelBytes();
  }

  //! Compute the maximal row alignment for current row size.
  //! @return maximal row alignment in bytes (up to 16 bytes).
  inline Standard_Size MaxRowAligmentBytes() const
  {
    return myData.MaxRowAligmentBytes();
  }

  //! @return buffer size
  inline Standard_Size SizeBytes() const
  {
    return myData.Size();
  }

  //! Access image pixel with specified color type.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  template <typename ColorType_t>
  inline const ColorType_t& Value (const Standard_Size theRow,
                                   const Standard_Size theCol) const
  {
    return *reinterpret_cast<const ColorType_t*>(myData.Value (theRow, theCol));
  }

  //! Access image pixel with specified color type.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  template <typename ColorType_t>
  inline ColorType_t& ChangeValue (const Standard_Size theRow,
                                   const Standard_Size theCol)
  {
    return *reinterpret_cast<ColorType_t* >(myData.ChangeValue (theRow, theCol));
  }

protected:

  Image_PixMapData myData;      //!< data buffer
  ImgFormat        myImgFormat; //!< pixel format

private:

  //! Copying allowed only within Handles
  Image_PixMap            (const Image_PixMap& );
  Image_PixMap& operator= (const Image_PixMap& );

public:

  DEFINE_STANDARD_RTTI(Image_PixMap) // Type definition

};

#endif // _Image_PixMap_H__
