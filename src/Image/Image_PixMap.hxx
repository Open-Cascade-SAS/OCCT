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

#ifndef _Image_PixMap_H__
#define _Image_PixMap_H__

#include <Image_PixMapData.hxx>
#include <Standard_Transient.hxx>
#include <Quantity_Color.hxx>

//! Class represents packed image plane.
class Image_PixMap : public Standard_Transient
{

public:

  //! This enumeration define packed image plane formats
  typedef enum tagFormat {
      ImgUNKNOWN = 0, //!< unsupported or unknown format
      ImgGray    = 1, //!< 1 byte per pixel
      ImgRGB,         //!< 3 bytes packed RGB image plane
      ImgBGR,         //!< same as RGB but with different components order
      ImgRGB32,       //!< 4 bytes packed RGB image plane (1 extra byte for alignment, may have undefined value)
      ImgBGR32,       //!< same as RGB but with different components order
      ImgRGBA,        //!< 4 bytes packed RGBA image plane
      ImgBGRA,        //!< same as RGBA but with different components order
      ImgGrayF,       //!< 1 float  (4-bytes) per pixel (1-component plane)
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

  //! @return image width in pixels
  inline Standard_Size Width() const
  {
    return myData.mySizeX;
  }

  //! @return image height in pixels
  inline Standard_Size Height() const
  {
    return myData.mySizeY;
  }

  //! @return image width in pixels
  inline Standard_Size SizeX() const
  {
    return myData.mySizeX;
  }

  //! @return image height in pixels
  inline Standard_Size SizeY() const
  {
    return myData.mySizeY;
  }

  //! @return width / height.
  inline Standard_Real Ratio() const
  {
    return (myData.mySizeY > 0) ? (Standard_Real(myData.mySizeX) / Standard_Real(myData.mySizeY)) : 1.0;
  }

  //! @return true if data is NULL.
  bool IsEmpty() const
  {
    return myData.myDataPtr == NULL;
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
  Standard_EXPORT virtual void Clear (ImgFormat thePixelFormat = ImgGray);

public: // low-level API for batch-processing (pixels reading / comparison / modification)

  //! Returns true if image data stored from Top to the Down (default).
  //! Some external APIs can return bottom-up data instead
  //! (topmost scanlines starts from the bottom in memory).
  //! Notice that access methods within this class automatically
  //! convert input row-index to apply this flag!
  //! You should use this flag only if interconnect with alien APIs and buffers.
  //! @return true if image data is top-down.
  inline bool IsTopDown() const
  {
    return myData.myTopToDown == 1;
  }

  //! Setup scanlines order in memory - top-down or bottom-up.
  //! Drawers should explicitly specify this value if current state IsTopDown() was ignored!
  //! @param theIsTopDown - top-down flag.
  inline void SetTopDown (bool theIsTopDown)
  {
    myData.myTopToDown = (theIsTopDown ? 1 : Standard_Size(-1));
    setTopDown();
  }

  //! Returns +1 if scanlines ordered in Top->Down order in memory and -1 otherwise.
  //! @return scanline increment for Top->Down iteration
  inline Standard_Size TopDownInc() const
  {
    return myData.myTopToDown;
  }

  //! @return data pointer for low-level operations (copying entire buffer, parsing with extra tools etc.).
  inline const Standard_Byte* Data() const
  {
    return myData.myDataPtr;
  }

  //! @return data pointer for low-level operations (copying entire buffer, parsing with extra tools etc.).
  inline Standard_Byte* ChangeData()
  {
    return myData.myDataPtr;
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
    return myData.mySizeBPP;
  }

  //! @return bytes reserved for one pixel (may include extra bytes for alignment).
  static Standard_Size SizePixelBytes (const Image_PixMap::ImgFormat thePixelFormat);

  //! @return bytes reserved per row.
  //! Could be larger than needed to store packed row (extra bytes for alignment etc.).
  inline Standard_Size SizeRowBytes() const
  {
    return myData.mySizeRowBytes;
  }

  //! @return the extra bytes in the row.
  inline Standard_Size RowExtraBytes() const
  {
    return myData.mySizeRowBytes - myData.mySizeX * myData.mySizeBPP;
  }

  //! Compute the maximal row alignment for current row size.
  //! @return maximal row alignment in bytes (up to 16 bytes).
  inline Standard_Size MaxRowAligmentBytes() const
  {
    return myData.MaxRowAligmentBytes();
  }

  inline Standard_Size SizeBytes() const
  {
    return myData.SizeBytes();
  }

  //! Access image buffer for write/read operations with specified color type.
  template <typename ColorType_t>
  inline Image_PixMapData<ColorType_t>& EditData()
  {
    return *(Image_PixMapData<ColorType_t>* )&myData;
  }

  //! Access image buffer for read operations with specified color type.
  template <typename ColorType_t>
  inline const Image_PixMapData<ColorType_t>& ReadData() const
  {
    return *(Image_PixMapData<ColorType_t>* )&myData;
  }

  //! Access image pixel with specified color type.
  template <typename ColorType_t>
  inline const ColorType_t& Value (const Standard_Size theRow,
                                   const Standard_Size theCol) const
  {
    return ((Image_PixMapData<ColorType_t>* )&myData)->Value (theRow, theCol);
  }

protected:

  //! Setup pixel format
  Standard_EXPORT void setFormat (ImgFormat thePixelFormat);

  //! Auxiliary method to setup myTopRowPtr
  Standard_EXPORT void setTopDown();

protected:

  Image_PixMapData<Standard_Byte> myData;
  ImgFormat                       myImgFormat;    //!< pixel format
  bool                            myIsOwnPointer; //!< if data was allocated by this class - flag is true

private:

  //! Copying allowed only within Handles
  Image_PixMap            (const Image_PixMap& );
  Image_PixMap& operator= (const Image_PixMap& );

public:

  DEFINE_STANDARD_RTTI(Image_PixMap) // Type definition

};

DEFINE_STANDARD_HANDLE(Image_PixMap, Standard_Transient)

#endif // _Image_PixMap_H__
