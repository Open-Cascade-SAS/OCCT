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
  Standard_EXPORT static size_t SizePixelBytes(const Image_Format thePixelFormat);

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
  Standard_EXPORT static const occ::handle<NCollection_BaseAllocator>& DefaultAllocator();

  //! Return string representation of pixel format.
  Standard_EXPORT static const char* ImageFormatToString(Image_Format theFormat);

  //! Return string representation of compressed pixel format.
  Standard_EXPORT static const char* ImageFormatToString(Image_CompressedFormat theFormat);

  //! Convert raw pixel value into Quantity_ColorRGBA. This function is relatively slow.
  //! @param[in] theRawValue pointer to pixel definition
  //! @param[in] theFormat pixel format
  //! @param[in] theToLinearize when TRUE, the color stored in non-linear color space (e.g.
  //! Image_Format_RGB) will be linearized
  //! @return the pixel color
  Standard_EXPORT static Quantity_ColorRGBA ColorFromRawPixel(const uint8_t*     theRawValue,
                                                              const Image_Format theFormat,
                                                              const bool theToLinearize = false);

  //! Set raw pixel value from Quantity_ColorRGBA. This function is relatively slow.
  //! @param[out] theRawValue pointer to pixel definition to modify
  //! @param[in]  theFormat pixel format
  //! @param[in]  theColor color value to convert from
  //! @param[in] theToDeLinearize when TRUE, the gamma correction will be applied for storing in
  //! non-linear color space (e.g. Image_Format_RGB)
  Standard_EXPORT static void ColorToRawPixel(uint8_t*                  theRawValue,
                                              const Image_Format        theFormat,
                                              const Quantity_ColorRGBA& theColor,
                                              const bool                theToDeLinearize = false);

public: // high-level API
  //! Return pixel format.
  Image_Format Format() const { return myImgFormat; }

  //! Override pixel format specified by InitXXX() methods.
  //! Will throw exception if pixel size of new format is not equal to currently initialized format.
  //! Intended to switch formats indicating different interpretation of the same data
  //! (e.g. ImgGray and ImgAlpha).
  Standard_EXPORT void SetFormat(const Image_Format thePixelFormat);

  //! Return image width in pixels.
  size_t Width() const { return myData.SizeX; }

  //! Return image height in pixels.
  size_t Height() const { return myData.SizeY; }

  //! Return image depth in pixels.
  size_t Depth() const { return myData.SizeZ; }

  //! Return image width in pixels.
  size_t SizeX() const { return myData.SizeX; }

  //! Return image height in pixels.
  size_t SizeY() const { return myData.SizeY; }

  //! Return image depth in pixels.
  size_t SizeZ() const { return myData.SizeZ; }

  //! Return image width x height x depth in pixels.
  NCollection_Vec3<size_t> SizeXYZ() const
  {
    return NCollection_Vec3<size_t>(myData.SizeX, myData.SizeY, myData.SizeZ);
  }

  //! Return width / height.
  double Ratio() const { return (SizeY() > 0) ? (double(SizeX()) / double(SizeY())) : 1.0; }

  //! Return true if data is NULL.
  bool IsEmpty() const { return myData.IsEmpty(); }

  //! Empty constructor. Initialize the NULL image plane.
  Standard_EXPORT Image_PixMap();

  //! Destructor
  Standard_EXPORT ~Image_PixMap() override;

  //! Returns the pixel color. This function is relatively slow.
  //! Beware that this method takes coordinates in opposite order in contrast to ::Value() and
  //! ::ChangeValue().
  //! @param[in] theX column index from left, starting from 0
  //! @param[in] theY row    index from top,  starting from 0
  //! @param[in] theToLinearize when TRUE, the color stored in non-linear color space (e.g.
  //! Image_Format_RGB) will be linearized
  //! @return the pixel color
  Quantity_ColorRGBA PixelColor(int theX, int theY, bool theToLinearize = false) const
  {
    if (IsEmpty() || theX < 0 || (size_t)theX >= SizeX() || theY < 0 || (size_t)theY >= SizeY())
    {
      return Quantity_ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f); // transparent
    }

    const uint8_t* aRawPixel = RawValueXY(theX, theY);
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
  void SetPixelColor(const int             theX,
                     const int             theY,
                     const Quantity_Color& theColor,
                     const bool            theToDeLinearize = false)
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
  void SetPixelColor(const int                 theX,
                     const int                 theY,
                     const Quantity_ColorRGBA& theColor,
                     const bool                theToDeLinearize = false)
  {
    if (IsEmpty() || theX < 0 || size_t(theX) >= SizeX() || theY < 0 || size_t(theY) >= SizeY())
    {
      return;
    }

    uint8_t* aRawPixel = ChangeRawValueXY(theX, theY);
    ColorToRawPixel(aRawPixel, myImgFormat, theColor, theToDeLinearize);
  }

  //! Initialize image plane as wrapper over alien data.
  //! Data will not be copied! Notice that caller should ensure
  //! that data pointer will not be released during this wrapper lifetime.
  //! You may call InitCopy() to perform data copying.
  Standard_EXPORT virtual bool InitWrapper(Image_Format thePixelFormat,
                                           uint8_t*     theDataPtr,
                                           const size_t theSizeX,
                                           const size_t theSizeY,
                                           const size_t theSizeRowBytes = 0);

  //! Initialize image plane with required dimensions.
  //! Memory will be left uninitialized (performance trick).
  Standard_EXPORT virtual bool InitTrash(Image_Format thePixelFormat,
                                         const size_t theSizeX,
                                         const size_t theSizeY,
                                         const size_t theSizeRowBytes = 0);

  //! Initialize by copying data.
  //! If you want to copy alien data you should create wrapper using InitWrapper() before.
  Standard_EXPORT virtual bool InitCopy(const Image_PixMap& theCopy);

  //! Initialize image plane with required dimensions.
  //! Buffer will be zeroed (black color for most formats).
  bool InitZero(Image_Format  thePixelFormat,
                const size_t  theSizeX,
                const size_t  theSizeY,
                const size_t  theSizeRowBytes = 0,
                const uint8_t theValue        = 0)
  {
    return InitZero3D(thePixelFormat,
                      NCollection_Vec3<size_t>(theSizeX, theSizeY, 1),
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
  Standard_EXPORT virtual bool InitWrapper3D(Image_Format                    thePixelFormat,
                                             uint8_t*                        theDataPtr,
                                             const NCollection_Vec3<size_t>& theSizeXYZ,
                                             const size_t                    theSizeRowBytes = 0);

  //! Initialize 2D/3D image with required dimensions.
  //! Memory will be left uninitialized (performance trick).
  Standard_EXPORT virtual bool InitTrash3D(Image_Format                    thePixelFormat,
                                           const NCollection_Vec3<size_t>& theSizeXYZ,
                                           const size_t                    theSizeRowBytes = 0);

  //! Initialize 2D/3D image with required dimensions.
  //! Buffer will be zeroed (black color for most formats).
  Standard_EXPORT bool InitZero3D(Image_Format                    thePixelFormat,
                                  const NCollection_Vec3<size_t>& theSizeXYZ,
                                  const size_t                    theSizeRowBytes = 0,
                                  const uint8_t                   theValue        = 0);

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
  size_t TopDownInc() const { return myData.TopToDown; }

  //! Return data pointer for low-level operations (copying entire buffer, parsing with extra tools
  //! etc.).
  const uint8_t* Data() const { return myData.Data(); }

  //! Return data pointer for low-level operations (copying entire buffer, parsing with extra tools
  //! etc.).
  uint8_t* ChangeData() { return myData.ChangeData(); }

  //! Return data pointer to requested row (first column).
  //! Indexation starts from 0.
  const uint8_t* Row(size_t theRow) const { return myData.Row(theRow); }

  //! Return data pointer to requested row (first column).
  //! Indexation starts from 0.
  uint8_t* ChangeRow(size_t theRow) { return myData.ChangeRow(theRow); }

  //! Return data pointer to requested 2D slice.
  //! Indexation starts from 0.
  const uint8_t* Slice(size_t theSlice) const { return myData.Slice(theSlice); }

  //! Return data pointer to requested 2D slice.
  //! Indexation starts from 0.
  uint8_t* ChangeSlice(size_t theSlice) { return myData.ChangeSlice(theSlice); }

  //! Return data pointer to requested row (first column).
  //! Indexation starts from 0.
  const uint8_t* SliceRow(size_t theSlice, size_t theRow) const
  {
    return myData.SliceRow(theSlice, theRow);
  }

  //! Return data pointer to requested row (first column).
  //! Indexation starts from 0.
  uint8_t* ChangeSliceRow(size_t theSlice, size_t theRow)
  {
    return myData.ChangeSliceRow(theSlice, theRow);
  }

  //! Return bytes reserved for one pixel (may include extra bytes for alignment).
  size_t SizePixelBytes() const { return myData.SizeBPP; }

  //! Return bytes reserved per row.
  //! Could be larger than needed to store packed row (extra bytes for alignment etc.).
  size_t SizeRowBytes() const { return myData.SizeRowBytes; }

  //! Return the extra bytes in the row.
  size_t RowExtraBytes() const { return SizeRowBytes() - SizeX() * SizePixelBytes(); }

  //! Compute the maximal row alignment for current row size.
  //! @return maximal row alignment in bytes (up to 16 bytes).
  size_t MaxRowAligmentBytes() const { return myData.MaxRowAligmentBytes(); }

  //! Return number of bytes per 2D slice.
  size_t SizeSliceBytes() const { return myData.SizeSliceBytes; }

  //! Return buffer size
  size_t SizeBytes() const { return myData.Size(); }

public:
  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in the decreasing majority following memory layout -
  //! e.g. row first, column next.
  template <typename ColorType_t>
  const ColorType_t& Value(size_t theRow, size_t theCol) const
  {
    return *reinterpret_cast<const ColorType_t*>(myData.Value(theRow, theCol));
  }

  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in the decreasing majority following memory layout -
  //! e.g. row first, column next.
  template <typename ColorType_t>
  ColorType_t& ChangeValue(size_t theRow, size_t theCol)
  {
    return *reinterpret_cast<ColorType_t*>(myData.ChangeValue(theRow, theCol));
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in the decreasing majority following memory layout -
  //! e.g. row first, column next.
  const uint8_t* RawValue(size_t theRow, size_t theCol) const
  {
    return myData.Value(theRow, theCol);
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in the decreasing majority following memory layout -
  //! e.g. row first, column next.
  uint8_t* ChangeRawValue(size_t theRow, size_t theCol)
  {
    return myData.ChangeValue(theRow, theCol);
  }

  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y order.
  template <typename ColorType_t>
  const ColorType_t& ValueXY(size_t theX, size_t theY) const
  {
    return *reinterpret_cast<const ColorType_t*>(myData.ValueXY(theX, theY));
  }

  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y order.
  template <typename ColorType_t>
  ColorType_t& ChangeValueXY(size_t theX, size_t theY)
  {
    return *reinterpret_cast<ColorType_t*>(myData.ChangeValueXY(theX, theY));
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y order.
  const uint8_t* RawValueXY(size_t theX, size_t theY) const { return myData.ValueXY(theX, theY); }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y order.
  uint8_t* ChangeRawValueXY(size_t theX, size_t theY) { return myData.ChangeValueXY(theX, theY); }

public:
  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y, Z order.
  template <typename ColorType_t>
  const ColorType_t& ValueXYZ(size_t theX, size_t theY, size_t theZ) const
  {
    return *reinterpret_cast<const ColorType_t*>(myData.ValueXYZ(theX, theY, theZ));
  }

  //! Access image pixel with specified color type.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y, Z order.
  template <typename ColorType_t>
  ColorType_t& ChangeValueXYZ(size_t theX, size_t theY, size_t theZ)
  {
    return *reinterpret_cast<ColorType_t*>(myData.ChangeValueXYZ(theX, theY, theZ));
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y, Z order.
  const uint8_t* RawValueXYZ(size_t theX, size_t theY, size_t theZ) const
  {
    return myData.ValueXYZ(theX, theY, theZ);
  }

  //! Access image pixel as raw data pointer.
  //! Indexation starts from 0.
  //! This method does not perform any type checks - use on own risk (check Format() before)!
  //! WARNING: Input parameters are defined in traditional X, Y, Z order.
  uint8_t* ChangeRawValueXYZ(size_t theX, size_t theY, size_t theZ)
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
  Image_PixMap(const Image_PixMap&) = delete;
  Image_PixMap& operator=(const Image_PixMap&) = delete;
};

#endif // _Image_PixMap_H__
