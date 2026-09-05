// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _GeomHash_CanonicalHashStream_HeaderFile
#define _GeomHash_CanonicalHashStream_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

//! Provides canonical encoding of scalar and point values for hashing.
//!
//! Integer and floating-point values are written in little-endian byte order.
//! Floating-point signed zeros and NaN values are normalized to stable bit patterns.
//! The byte sink shall provide `result_type`, `AppendBytes()`, and `Finish()` members.
//! @tparam TheByteSink byte-oriented sink receiving the encoded data
template <class TheByteSink>
class GeomHash_CanonicalHashStream
{
public:
  //! Result type returned by the underlying byte sink.
  using result_type = typename TheByteSink::result_type;

  //! Appends an unmodified byte sequence.
  //! @param[in] theData pointer to the first byte
  //! @param[in] theSize number of bytes to append
  void AppendBytes(const void* theData, const size_t theSize)
  {
    myByteSink.AppendBytes(theData, theSize);
  }

  //! Appends an unsigned byte.
  //! @param[in] theValue value to append
  void AppendByte(const uint8_t theValue) { AppendBytes(&theValue, sizeof(theValue)); }

  //! Appends a Boolean as one byte containing zero or one.
  //! @param[in] theValue value to append
  void AppendBool(const bool theValue) { AppendByte(theValue ? 1u : 0u); }

  //! Appends a 32-bit signed integer preserving its unsigned bit pattern.
  //! @param[in] theValue value to append
  void AppendInt(const int theValue)
  {
    static_assert(sizeof(int) == sizeof(uint32_t), "unexpected int size");
    AppendUInt32(static_cast<uint32_t>(theValue));
  }

  //! Appends a 32-bit unsigned integer in little-endian byte order.
  //! @param[in] theValue value to append
  void AppendUInt32(const uint32_t theValue)
  {
    std::array<uint8_t, 4> aBytes;
    for (int aByteIndex = 0; aByteIndex < 4; ++aByteIndex)
    {
      aBytes[static_cast<size_t>(aByteIndex)] = static_cast<uint8_t>(theValue >> (aByteIndex * 8));
    }
    AppendBytes(aBytes.data(), aBytes.size());
  }

  //! Appends a 64-bit unsigned integer in little-endian byte order.
  //! @param[in] theValue value to append
  void AppendUInt64(const uint64_t theValue)
  {
    std::array<uint8_t, 8> aBytes;
    for (int aByteIndex = 0; aByteIndex < 8; ++aByteIndex)
    {
      aBytes[static_cast<size_t>(aByteIndex)] = static_cast<uint8_t>(theValue >> (aByteIndex * 8));
    }
    AppendBytes(aBytes.data(), aBytes.size());
  }

  //! Appends an IEEE-754 single-precision value in canonical form.
  //! Signed zero is encoded as positive zero and all NaNs use one quiet-NaN representation.
  //! @param[in] theValue value to append
  void AppendFloat(float theValue)
  {
    static_assert(std::numeric_limits<float>::is_iec559, "non-IEEE float is unsupported");
    if (std::isnan(theValue))
    {
      AppendUInt32(0x7fc00000u);
      return;
    }
    if (theValue == 0.0f)
    {
      theValue = 0.0f;
    }

    uint32_t aBits = 0;
    static_assert(sizeof(aBits) == sizeof(theValue), "unexpected float size");
    std::memcpy(&aBits, &theValue, sizeof(aBits));
    AppendUInt32(aBits);
  }

  //! Appends an IEEE-754 double-precision value in canonical form.
  //! Signed zero is encoded as positive zero and all NaNs use one quiet-NaN representation.
  //! @param[in] theValue value to append
  void AppendDouble(double theValue)
  {
    static_assert(std::numeric_limits<double>::is_iec559, "non-IEEE double is unsupported");
    if (std::isnan(theValue))
    {
      AppendUInt64(0x7ff8000000000000ull);
      return;
    }
    if (theValue == 0.0)
    {
      theValue = 0.0;
    }

    uint64_t aBits = 0;
    static_assert(sizeof(aBits) == sizeof(theValue), "unexpected double size");
    std::memcpy(&aBits, &theValue, sizeof(aBits));
    AppendUInt64(aBits);
  }

  //! Appends the X, Y, and Z coordinates of a 3D point.
  //! @param[in] thePoint point to append
  void AppendPoint(const gp_Pnt& thePoint)
  {
    AppendDouble(thePoint.X());
    AppendDouble(thePoint.Y());
    AppendDouble(thePoint.Z());
  }

  //! Appends the X and Y coordinates of a 2D point.
  //! @param[in] thePoint point to append
  void AppendPoint2d(const gp_Pnt2d& thePoint)
  {
    AppendDouble(thePoint.X());
    AppendDouble(thePoint.Y());
  }

  //! Finalizes the underlying sink and returns its result.
  //! @return result produced by the byte sink
  [[nodiscard]] result_type Finish() const { return myByteSink.Finish(); }

protected:
  //! Returns the underlying byte sink for derived stream implementations.
  const TheByteSink& ByteSink() const noexcept { return myByteSink; }

private:
  TheByteSink myByteSink;
};

#endif // _GeomHash_CanonicalHashStream_HeaderFile
