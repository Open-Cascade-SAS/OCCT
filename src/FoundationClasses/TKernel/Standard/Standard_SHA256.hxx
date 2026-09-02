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

#ifndef _Standard_SHA256_HeaderFile
#define _Standard_SHA256_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>

#include <array>
#include <cstddef>
#include <cstdint>

//! Incremental SHA-256 calculation over an exact byte sequence.
class Standard_SHA256
{
public:
  DEFINE_STANDARD_ALLOC

  //! Fixed 256-bit digest.
  class Digest
  {
  public:
    DEFINE_STANDARD_ALLOC

    //! Return lowercase hexadecimal text without an algorithm prefix.
    [[nodiscard]] Standard_EXPORT TCollection_AsciiString ToString() const;

    //! Return the digest bytes in network order.
    [[nodiscard]] const std::array<uint8_t, 32>& Bytes() const noexcept { return myBytes; }

    friend bool operator==(const Digest& theLeft, const Digest& theRight) noexcept
    {
      return theLeft.myBytes == theRight.myBytes;
    }

    friend bool operator!=(const Digest& theLeft, const Digest& theRight) noexcept
    {
      return !(theLeft == theRight);
    }

  private:
    friend class Standard_SHA256;
    std::array<uint8_t, 32> myBytes{};
  };

  //! Construct an empty calculation.
  Standard_EXPORT Standard_SHA256();

  //! Append bytes.
  //! @return false for a null non-empty block or an input length overflow
  [[nodiscard]] Standard_EXPORT bool Append(const void* theData, size_t theSize);

  //! Finish the calculation without changing accumulated state.
  //! @return false if the total input length cannot be represented by SHA-256
  [[nodiscard]] Standard_EXPORT bool Finish(Digest& theDigest) const;

  //! Calculate one complete byte sequence.
  [[nodiscard]] Standard_EXPORT static bool Hash(const void* theData,
                                                 size_t      theSize,
                                                 Digest&     theDigest);

private:
  Standard_EXPORT void processBlock(const uint8_t* theBlock);

  std::array<uint32_t, 8> myState;
  std::array<uint8_t, 64> myBuffer{};
  uint64_t                myByteCount = 0;
  size_t                  myBufferSize = 0;
};

#endif // _Standard_SHA256_HeaderFile
