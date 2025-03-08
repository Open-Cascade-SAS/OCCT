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

#ifndef Image_PixMap_HeaderFile
#define Image_PixMap_HeaderFile

#include <Image_CompressedFormat.hxx>
#include <Image_PixMapData.hxx>
#include <Standard_Transient.hxx>
#include <Quantity_ColorRGBA.hxx>

//! Class represents packed image plane.
class Image_PixMap : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Image_PixMap, Standard_Transient)
public:
  //! Determine Big-Endian at runtime
  static inline bool IsBigEndianHost()
  {
    union {
      int  myInt;
      char myChar[sizeof(int)];
    } aUnion;

    aUnion.myInt = 1;
    return !aUnion.myChar[0];
  }

  //! Return bytes reserved for one pixel (may include extra bytes for alignment).
  Standard_EXPORT static Standard_Size SizePixelBytes(const Image_Format thePixelFormat);

  //! Auxiliary method for swapping bytes between RGB and BGR formats.
  //! This method modifies the image data but does not change pixel format!
  //! Method will fail if pixel format is not one of the following:
  //!  - Image_Format_RGB32 / Image_Format_BGR32
  //!  - Image_Format_RGBA  / Image_Format_BGRA
  //!  - Image_Format_RGB   / Image_Format_BGR
  //!  - Image_Format_RGBF  / Image_Format_BGRF
  //!  - Image_Format_RGBAF / Image_Format_BGRAF
  Standard_EXPORT static bool SwapRgbaBgra(Image_PixMap& theImage);

  //! Convert image to Black/White.
  Standard_EXPORT static void ToBlackWhite(Image_PixMap& theImage);

  //! Reverse line order as it draws it from bottom to top.
  Standard_EXPORT static bool FlipY(Image_PixMap& theImage);

  //! Return default image data allocator.
  Standard_EXPORT static const Handle(NCollection_BaseAllocator)& DefaultAllocator();

  //! Return string representation of pixel format.
  Standard_EXPORT static Standard_CString ImageFormatToString(Image_Format theFormat);

  //! Return string representation of compressed pixel format.
  Standard_EXPORT static Standard_CString ImageFormatToString(Image_CompressedFormat theFormat);

  //! Convert raw pixel value into Quantity_ColorRGBA. This function is relatively slow.
  //! @param[in] theRawValue pointer to pixel definition
  //! @param[in] theFormat pixel format
  //! @param[in] theToLinearize when TRUE, the color stored in non-linear color space (e.g.
  //! Image_Format_RGB) will be linearized
  //! @return the pixel color
  Standard_EXPORT static Quantity_ColorRGBA ColorFromRawPixel(
    const Standard_Byte*   theRawValue,
    const Image_Format     theFormat,
    const Standard_Boolean theToLinearize = false);

  //! Set raw pixel value from Quantity_ColorRGBA. This function is relatively slow.
  //! @param[out] theRawValue pointer to pixel definition to modify
  //! @param[in]  theFormat pixel format
  //! @param[in]  theColor color value to convert from
  //! @param[in] theToDeLinearize when TRUE, the gamma correction will be applied for storing in
  //! non-linear color space (e.g. Image_Format_RGB)
  Standard_EXPORT static void ColorToRawPixel(Standard_Byte*            theRawValue,
                                              const Image_Format        theFormat,
                                              const Quantity_ColorRGBA& theColor,
                                              const Standard_Boolean    theToDeLinearize = false);

public: // high-level API
  //! Return pixel format.
  Image_Format Format() const { return myImgFormat; }

  //! Override pixel format specified by InitXXX() methods.
  //! Will throw exception if pixel size of new format is not equal to currently initialized format.
  //! Intended to switch formats indicating different interpretation of the same data
  //! (e.g. ImgGray and ImgAlpha).
  Standard_EXPORT void SetFormat(const Image_Format thePixelFormat);

  //! Return image width in pixels.
  Standard_Size Width() const { return myData.SizeX; }

  //! Return image height in pixels.
  Standard_Size Height() const { return myData.SizeY; }

  //! Return image depth in pixels.
  Standard_Size Depth() const { return myData.SizeZ; }

  //! Return image width in pixels.
  Standard_Size SizeX() const { return myData.SizeX; }

  //! Return image height in pixels.
  Standard_Size SizeY() const { return myData.SizeY; }

  //! Return image depth in pixels.
  Standard_Size SizeZ() const { return myData.SizeZ; }

  //! Return image width x height x depth in pixels.
  NCollection_Vec3<Standard_Size> SizeXYZ() const
  {
    return NCollection_Vec3<Standard_Size>(myData.SizeX, myData.SizeY, myData.SizeZ);
  }

  //! Return width / height.
  Standard_Real Ratio() const
  {
    return (SizeY() > 0) ? (Standard_Real(SizeX()) / Standard_Real(SizeY())) : 1.0;
  }

  //! Return true if data is NULL.
  bool IsEmpty() const { return myData.IsEmpty(); }

  //! Empty constructor. Initialize the NULL image plane.
  Standard_EXPORT Image_PixMap();

  //! Destructor
  Standard_EXPORT virtual ~Image_PixMap();

  //! Returns the pixel color. This function is relatively slow.
  //! Beware that this method takes coordinates in opposite order in contrast to ::Value() and
  //! ::ChangeValue().
  //! @param[in] theX column index from left, starting from 0
  //! @param[in] theY row    index from top,  starting from 0
  //! @param[in] theToLinearize when TRUE, the color stored in non-linear color space (e.g.
  //! Image_Format_RGB) will be linearized
  //! @return the pixel color
  Quantity_ColorRGBA PixelColor(Standard_Integer theX,
                                Standard_Integer theY,
                                Standard_Boolean theToLinearize = false) const
  {
    if (IsEmpty() || theX < 0 || (Standard_Size)theX >= SizeX() || theY < 0
        || (Standard_Size)theY >= SizeY())
    {
      return Quantity_ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f); // transparent
    }

    const Standard_Byte* aRawPixel = RawValueXY(theX, theY);
    return ColorFromRawPixel(aRawPixel, myImgFormat, theToLinearize);
  }

  //! Sets the pixel color. This function is relatively slow.
  //! Beware that this method takes coordinates in opposite order in contrast to ::Value() and
  //! ::ChangeValue().
  //! @param[in] theX column index from left
  //! @param[in] theY row    index from top
  //! @param[in] theColor color to store
  //! @param[in] theToDeLinearize when TRUE, the gamma correction will be applied for storing in
  //! non-linear color space (e.g. Image_Format_RGB)
  void SetPixelColor(const Standard_Integer theX,
                     const Standard_Integer theY,
                     const Quantity_Color&  theColor,
                     const Standard_Boolean theToDeLinearize = false)
  {
    SetPixelColor(theX, theY, Quantity_ColorRGBA(theColor, 1.0f), theToDeLinearize);
  }

  //! Sets the pixel color. This function is relatively slow.
  //! Beware that this method takes coordinates in opposite order in contrast to ::Value() and
  //! ::ChangeValue().
  //! @param[in] theX column index from left
  //! @param[in] theY row    index from top
  //! @param[in] theColor color to store
  //! @param[in] theToDeLinearize when TRUE, the gamma correction will be applied for storing in
  //! non-linear color space (e.g. Image_Format_RGB)
  void SetPixelColor(const Standard_Integer    theX,
                     const Standard_Integer    theY,
                     const Quantity_ColorRGBA& theColor,
                     const Standard_Boolean    theToDeLinearize = false)
  {
    if (IsEmpty() || theX < 0 || Standard_Size(theX) >= SizeX() || theY < 0
        || Standard_Size(theY) >= SizeY())
    {
      return;
    }

    Standard_Byte* aRawPixel = ChangeRawValueXY(theX, theY);
    ColorToRawPixel(aRawPixel, myImgFormat, theColor, theToDeLinearize);
  }

  //! Initialize image plane as wrapper over alien data.
  //! Data will not be copied! Notice that caller should ensure
  //! that data pointer will not be released during this wrapper lifetime.
  //! You may call InitCopy() to perform data copying.
  Standard_EXPORT virtual bool InitWrapper(Image_Format        thePixelFormat,
                                           Standard_Byte*      theDataPtr,
                                           const Standard_Size theSizeX,
                                           const Standard_Size theSizeY,
                                           const Standard_Size theSizeRowBytes = 0);

  //! Initialize image plane with required dimensions.
  //! Memory will be left uninitialized (performance trick).
  Standard_EXPORT virtual bool InitTrash(Image_Format        thePixelFormat,
                                         const Standard_Size theSizeX,
                                         const Standard_Size theSizeY,
                                         const Standard_Size theSizeRowBytes = 0);

  //! Initialize by copying data.
  //! If you want to copy alien data you should create wrapper using InitWrapper() before.
  Standard_EXPORT virtual bool InitCopy(const Image_PixMap& theCopy);

  //! Initialize image plane with required dimensions.
  //! Buffer will be zeroed (black color for most formats).
  bool InitZero(Image_Format        thePixelFormat,
                const Standard_Size theSizeX,
                const Standard_Size theSizeY,
                const Standard_Size theSizeRowBytes = 0,
                const Standard_Byte theValue        = 0)
  {
    return InitZero3D(thePixelFormat,
                      NCollection_Vec3<Standard_Size>(theSizeX, theSizeY, 1),
                      theSizeRowBytes,
                      theValue);
  }

  //! Method correctly deallocate internal buffer.
  Standard_EXPORT virtual void Clear();

public:
  //! Initialize 2D/3D image as wrapper over alien data.
  //! Data will not be copied! Notice that caller should ensure
  //! that data pointer will not be released during this wrapper lifetime.
  //! You may call InitCopy() to perform data copying.
  Standard_EXPORT virtual bool InitWrapper3D(Image_Format                           thePixelFormat,
                                             Standard_Byte*                         theDataPtr,
                                             const NCollection_Vec3<Standard_Size>& theSizeXYZ,
                                             const Standard_Size theSizeRowBytes = 0);

  //! Initialize 2D/3D image with required dimensions.
  //! Memory will be left uninitialized (performance trick).
  Standard_EXPORT virtual bool InitTrash3D(Image_Format                           thePixelFormat,
                                           const NCollection_Vec3<Standard_Size>& theSizeXYZ,
                                           const Standard_Size theSizeRowBytes = 0);

  //! Initialize 2D/3D image with required dimensions.
  //! Buffer will be zeroed (black color for most formats).
  Standard_EXPORT bool InitZero3D(Image_Format                           thePixelFormat,
                                  const NCollection_Vec3<Standard_Size>& theSizeXYZ,
                                  const Standard_Size                    theSizeRowBytes = 0,
                                  const Standard_Byte                    theValue        = 0);

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
  bool IsTopDown() const { return myData.TopToDown == 1; }

  //! Setup scanlines order in memory - top-down or bottom-up.
  //! Drawers should explicitly specify this value if current state IsTopDown() was ignored!
  //! @param theIsTopDown top-down flag
  void SetTopDown(const bool theIsTopDown) { myData.SetTopDown(theIsTopDown); }

  //! Returns +1 if scanlines ordered in Top->Down order in memory and -1 otherwise.
  //! @return scanline increment for Top->Down iteration
  Standard_Size TopDownInc() const { return myData.TopToDown; }

  //! Return data pointer for low-level operations (copying entire buffer, parsing with extra tools
  //! etc.).
  const Standard_Byte* Data() const { return myData.Data(); }

  //! Return data pointer for low-level operations (copying entire buffer, parsing with extra tools
  //! etc.).
  Standard_Byte* ChangeData() { return myData.ChangeData(); }

  //! Return data pointer to requested row (first column).
  //! Indexation starts from 0.
  const Standard_Byte* Row(Standard_Size theRow) const { return myData.Row(theRow); }

  //! Return data pointer to requested row (first column).
  //! Indexation starts from 0.
  Standard_Byte* ChangeRow(Standard_Size theRow) { return myData.ChangeRow(theRow); }

  //! Return data pointer to requested 2D slice.
  //! Indexation starts from 0.
  const Standard_Byte* Slice(Standard_Size theSlice) const { return myData.Slice(theSlice); }

  //! Return data pointer to requested 2D slice.
  //! Indexation starts from 0.
  Standard_Byte* ChangeSlice(Standard_Size theSlice) { return myData.ChangeSlice(theSlice); }

  //! Return data pointer to requested row (first column).
  //! Indexation starts from 0.
  const Standard_Byte* SliceRow(Standard_Size theSlice, Standard_Size theRow) const
  {
    return myData.SliceRow(theSlice, theRow);
  }

  //! Return data pointer to requested row (first column).
  //! Indexation starts from 0.
  Standard_Byte* ChangeSliceRow(Standard_Size theSlice, Standard_Size theRow)
  {
    return myData.ChangeSliceRow(theSlice, theRow);
  }

  //! Return bytes reserved for one pixel (may include extra bytes for alignment).
  Standard_Size SizePixelBytes() const { return myData.SizeBPP; }

  //! Return bytes reserved per row.
  //! Could be larger than needed to store packed row (extra bytes for alignment etc.).
  Standard_Size SizeRowBytes() const { return myData.SizeRowBytes; }

  //! Return the extra bytes in the row.
  Standard_Size RowExtraBytes() const { return SizeRowBytes() - SizeX() * SizePixelBytes(); }

  //! Compute the maximal row alignment for current row size.
  //! @return maximal row alignment in bytes (up to 16 bytes).
  Standard_Size MaxRowAligmentBytes() const { return myData.MaxRowAligmentBytes(); }

  //! Return number of bytes per 2D slice.
  Standard_Size SizeSliceBytes() const { return myData.SizeSliceBytes; }

  //! Return buffer size
  Standard_Size SizeBytes() const { return myData.Size(); }

public:
  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in the decreasing majority following memory layout -
  //! e.g. row first, column next.
  template <typename ColorType_t>
  const ColorType_t& Value(Standard_Size theRow, Standard_Size theCol) const
  {
    return *reinterpret_cast<const ColorType_t*>(myData.Value(theRow, theCol));
  }

  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in the decreasing majority following memory layout -
  //! e.g. row first, column next.
  template <typename ColorType_t>
  ColorType_t& ChangeValue(Standard_Size theRow, Standard_Size theCol)
  {
    return *reinterpret_cast<ColorType_t*>(myData.ChangeValue(theRow, theCol));
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in the decreasing majority following memory layout -
  //! e.g. row first, column next.
  const Standard_Byte* RawValue(Standard_Size theRow, Standard_Size theCol) const
  {
    return myData.Value(theRow, theCol);
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in the decreasing majority following memory layout -
  //! e.g. row first, column next.
  Standard_Byte* ChangeRawValue(Standard_Size theRow, Standard_Size theCol)
  {
    return myData.ChangeValue(theRow, theCol);
  }

  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y order.
  template <typename ColorType_t>
  const ColorType_t& ValueXY(Standard_Size theX, Standard_Size theY) const
  {
    return *reinterpret_cast<const ColorType_t*>(myData.ValueXY(theX, theY));
  }

  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y order.
  template <typename ColorType_t>
  ColorType_t& ChangeValueXY(Standard_Size theX, Standard_Size theY)
  {
    return *reinterpret_cast<ColorType_t*>(myData.ChangeValueXY(theX, theY));
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y order.
  const Standard_Byte* RawValueXY(Standard_Size theX, Standard_Size theY) const
  {
    return myData.ValueXY(theX, theY);
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y order.
  Standard_Byte* ChangeRawValueXY(Standard_Size theX, Standard_Size theY)
  {
    return myData.ChangeValueXY(theX, theY);
  }

public:
  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y, Z order.
  template <typename ColorType_t>
  const ColorType_t& ValueXYZ(Standard_Size theX, Standard_Size theY, Standard_Size theZ) const
  {
    return *reinterpret_cast<const ColorType_t*>(myData.ValueXYZ(theX, theY, theZ));
  }

  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y, Z order.
  template <typename ColorType_t>
  ColorType_t& ChangeValueXYZ(Standard_Size theX, Standard_Size theY, Standard_Size theZ)
  {
    return *reinterpret_cast<ColorType_t*>(myData.ChangeValueXYZ(theX, theY, theZ));
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y, Z order.
  const Standard_Byte* RawValueXYZ(Standard_Size theX, Standard_Size theY, Standard_Size theZ) const
  {
    return myData.ValueXYZ(theX, theY, theZ);
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y, Z order.
  Standard_Byte* ChangeRawValueXYZ(Standard_Size theX, Standard_Size theY, Standard_Size theZ)
  {
    return myData.ChangeValueXYZ(theX, theY, theZ);
  }

public:
  //! Convert 16-bit half-float value into 32-bit float (simple conversion).
  static float ConvertFromHalfFloat(const uint16_t theHalf)
  {
    union FloatUint32 {
      float    Float32;
      uint32_t UInt32;
    };

    const uint32_t e = (theHalf & 0x7C00) >> 10; // exponent
    const uint32_t m = (theHalf & 0x03FF) << 13; // mantissa
    FloatUint32    mf, aRes;
    mf.Float32 = (float)m;
    // clang-format off
    const uint32_t v = mf.UInt32 >> 23; // evil log2 bit hack to count leading zeros in denormalized format
    aRes.UInt32 = (theHalf & 0x8000)<<16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000)); // sign : normalized : denormalized
    // clang-format on
    return aRes.Float32;
  }

  //! Convert 32-bit float value into IEEE-754 16-bit floating-point format without infinity:
  //! 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits.
  static uint16_t ConvertToHalfFloat(const float theFloat)
  {
    union FloatUint32 {
      float    Float32;
      uint32_t UInt32;
    };

    FloatUint32 anInput;
    anInput.Float32 = theFloat;
    // clang-format off
    const uint32_t b = anInput.UInt32 + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
    const uint32_t e = (b & 0x7F800000) >> 23; // exponent
    const uint32_t m =  b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
    return (uint16_t)((b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13)
         | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF); // sign : normalized : denormalized : saturate
    // clang-format on
  }

protected:
  Image_PixMapData myData;      //!< data buffer
  Image_Format     myImgFormat; //!< pixel format

private:
  //! Copying allowed only within Handles
  Image_PixMap(const Image_PixMap&);
  Image_PixMap& operator=(const Image_PixMap&);
};

DEFINE_STANDARD_HANDLE(Image_PixMap, Standard_Transient)

#endif // _Image_PixMap_H__
