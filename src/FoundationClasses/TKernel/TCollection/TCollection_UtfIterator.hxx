// Created on: 2013-01-28
// Created by: Kirill GAVRILOV
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

#ifndef TCollection_UtfIterator_HeaderFile
#define TCollection_UtfIterator_HeaderFile

#include <Standard_DomainError.hxx>
#include <Standard_RangeError.hxx>

#include <array>
#include <cstddef>
#include <limits>
#include <cstdint>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

//! Bounded, non-owning iterator over UTF-8, UTF-16, or UTF-32 code units.
//! Input storage must remain alive and unchanged during iteration.
//! Embedded nulls are scalar values; use More() to detect the end.
//! Malformed UTF-8/UTF-16 consumes one code unit and yields char32_t(-1).
//! @code
//! TCollection_UtfIterator<char> anIter(std::string_view("text"));
//! for (; anIter.More(); ++anIter) { const char32_t aScalar = *anIter; }
//! @endcode
template <typename Type>
class TCollection_UtfIterator
{
public:
  //! Explicitly select input whose extent is determined by a terminating null.
  enum class InputMode
  {
    NullTerminated //!< Caller guarantees accessible storage through the terminator.
  };

  //! Construct an empty iterator.
  constexpr TCollection_UtfIterator() noexcept = default;

  //! Adapt a null-terminated string, including its terminating scalar.
  //! The caller guarantees accessible storage through the terminator. nullptr is empty.
  constexpr TCollection_UtfIterator(const Type* theData) { Init(theData); }

  //! Reset from a null-terminated string, including its terminating scalar.
  constexpr void Init(const Type* theData)
  {
    size_t aSize = 0;
    if (theData != nullptr)
    {
      while (theData[aSize] != Type(0))
      {
        ++aSize;
      }
      ++aSize;
    }
    Init(theData, aSize);
    myIsNullTerminated = true;
  }

  //! Construct from an explicit code-unit range. nullptr is allowed only with zero size.
  //! The caller guarantees accessible storage for the entire range; its size must fit in int.
  constexpr TCollection_UtfIterator(const Type* theData, size_t theCodeUnits)
  {
    Init(theData, theCodeUnits);
  }

  //! Construct from a bounded string view, including any embedded nulls.
  template <typename Traits>
  constexpr explicit TCollection_UtfIterator(std::basic_string_view<Type, Traits> theView)
      : TCollection_UtfIterator(theView.data(), theView.size())
  {
  }

  //! Construct from all code units of an array.
  template <size_t N>
  constexpr explicit TCollection_UtfIterator(const std::array<Type, N>& theData)
      : TCollection_UtfIterator(theData.data(), N)
  {
  }

  //! Adapt a trusted null-terminated string; scans for its terminator.
  //! The caller must guarantee accessible storage through the null. nullptr means empty.
  //! Prefer a bounded constructor whenever the length is available.
  constexpr TCollection_UtfIterator(const Type* theData, InputMode)
  {
    size_t aSize = 0;
    if (theData != nullptr)
    {
      while (theData[aSize] != Type(0))
      {
        ++aSize;
      }
    }
    Init(theData, aSize);
  }

  //! Reset to an explicit code-unit range. Throws Standard_DomainError for nullptr/nonzero size.
  constexpr void Init(const Type* theData, size_t theCodeUnits)
  {
    if (theData == nullptr && theCodeUnits != 0)
    {
      throw Standard_DomainError("Null UTF input with nonzero size");
    }
    if (theCodeUnits > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
      throw Standard_RangeError("UTF iterator range exceeds signed indexes");
    }
    myIsNullTerminated = false;
    myPosNext          = theData;      //!< Position after the current value.
    myRemaining        = theCodeUnits; //!< Unread code units after the current value.
    myCharIndex        = 0;            //!< Zero-based index of the current scalar.
    readCurrent();
  }

  //! Reset from a bounded view.
  template <typename Traits>
  constexpr void Init(std::basic_string_view<Type, Traits> theView)
  {
    Init(theView.data(), theView.size());
  }

  //! @return true when positioned on a code point, including an invalid value or U+0000
  constexpr bool More() const noexcept { return myCurrentUnits != 0; }

  //! Advance by one decoded value. Incrementing an exhausted iterator is a no-op.
  constexpr TCollection_UtfIterator& operator++()
  {
    if (More())
    {
      ++myCharIndex;
      readCurrent();
    }
    return *this;
  }

  //! Return the previous iterator and advance.
  constexpr TCollection_UtfIterator operator++(int)
  {
    TCollection_UtfIterator aPrevious = *this;
    ++*this;
    return aPrevious;
  }

  //! Compare positions and remaining bounds.
  constexpr bool operator==(const TCollection_UtfIterator& theOther) const noexcept
  {
    return myPosition == theOther.myPosition && myRemaining == theOther.myRemaining
           && myCurrentUnits == theOther.myCurrentUnits;
  }

  //! @return true for a scalar, including a legacy null terminator; false on malformed input
  constexpr bool IsValid() const noexcept
  {
    return (More() || myIsNullTerminated) && myCharUtf32 <= THE_MAX_CODE_POINT
           && !(myCharUtf32 >= THE_HIGH_SURROGATE_MIN && myCharUtf32 <= THE_LOW_SURROGATE_MAX);
  }

  //! @return the current value, or zero at end; use More() to distinguish embedded nulls
  constexpr char32_t operator*() const noexcept { return myCharUtf32; }

  //! @return read-only position of the current value, or the range end when exhausted
  constexpr const Type* BufferHere() const noexcept { return myPosition; }

  //! @return the current buffer for an originally writable input
  //! Do not write into const storage. Reinitialize the iterator after changing input.
  Type* ChangeBufferHere() noexcept { return const_cast<Type*>(myPosition); }

  //! @return read-only position immediately after the current value
  constexpr const Type* BufferNext() const noexcept { return myPosNext; }

  //! @return the zero-based scalar index, or the number of visited values at end
  constexpr int Index() const noexcept { return static_cast<int>(myCharIndex); }

  //! @return the scalar index as a size_t offset
  constexpr size_t IndexOffset() const noexcept { return myCharIndex; }

  //! @return number of source code units occupied by the current value; zero at end
  constexpr size_t CodeUnits() const noexcept { return myCurrentUnits; }

  //! @return required output bytes, or zero for an invalid value or end
  constexpr int AdvanceBytesUtf8() const;
  //! @return required output bytes, or zero for an invalid value or end
  constexpr int AdvanceBytesUtf16() const;
  //! @return required UTF-16 code units, or zero for an invalid value or end
  constexpr int AdvanceCodeUnitsUtf16() const;

  //! @return required UTF-32 bytes, or zero for invalid bounded input or end
  //! Null-terminated iterators always return four bytes.
  constexpr int AdvanceBytesUtf32() const noexcept
  {
    return myIsNullTerminated || IsValid() ? int(sizeof(char32_t)) : 0;
  }

  //! @return required bytes in the encoding selected by sizeof(TypeWrite)
  template <typename TypeWrite>
  constexpr int AdvanceBytesUtf() const
  {
    if constexpr (sizeof(TypeWrite) == 1)
    {
      return AdvanceBytesUtf8();
    }
    else if constexpr (sizeof(TypeWrite) == 2)
    {
      return AdvanceBytesUtf16();
    }
    else
    {
      static_assert(sizeof(TypeWrite) == 4, "UTF code units must occupy 1, 2, or 4 bytes");
      return AdvanceBytesUtf32();
    }
  }

  //! Encode one value. Capacity is in destination code units, not bytes.
  //! @return position after output; unchanged for invalid/end; nullptr for insufficient capacity
  //! No output is written on failure. No additional terminator is appended.
  template <typename TypeWrite>
  constexpr TypeWrite* GetUtf(TypeWrite* theBuffer, size_t theCapacity) const
  {
    if (!IsValid())
    {
      return theBuffer;
    }
    const size_t aCount = AdvanceBytesUtf<TypeWrite>() / sizeof(TypeWrite);
    if (theBuffer == nullptr || theCapacity < aCount)
    {
      return nullptr;
    }
    if constexpr (sizeof(TypeWrite) == 1)
    {
      return writeUTF8(theBuffer);
    }
    else if constexpr (sizeof(TypeWrite) == 2)
    {
      if (myCharUtf32 <= THE_BMP_MAX)
      {
        *theBuffer++ = static_cast<TypeWrite>(myCharUtf32);
      }
      else
      {
        const char32_t aValue = myCharUtf32 - 0x10000;
        *theBuffer++          = static_cast<TypeWrite>(THE_HIGH_SURROGATE_MIN + (aValue >> 10));
        *theBuffer++          = static_cast<TypeWrite>(THE_LOW_SURROGATE_MIN + (aValue & 0x3FF));
      }
      return theBuffer;
    }
    else
    {
      *theBuffer++ = static_cast<TypeWrite>(myCharUtf32);
      return theBuffer;
    }
  }

  //! Encode one scalar into caller-sized storage.
  //! The caller provides at least AdvanceBytesUtf<TypeWrite>() / sizeof(TypeWrite) units.
  template <typename TypeWrite>
  constexpr TypeWrite* GetUtf(TypeWrite* theBuffer) const
  {
    return GetUtf(theBuffer, AdvanceBytesUtf<TypeWrite>() / sizeof(TypeWrite));
  }

  //! Encode into caller-sized storage. The caller provides AdvanceBytesUtf8() bytes.
  constexpr char* GetUtf8(char* theBuffer) const { return GetUtf(theBuffer); }

  constexpr unsigned char* GetUtf8(unsigned char* theBuffer) const { return GetUtf(theBuffer); }

  //! Encode into caller-sized storage. The caller provides AdvanceCodeUnitsUtf16() units.
  constexpr char16_t* GetUtf16(char16_t* theBuffer) const { return GetUtf(theBuffer); }

  //! Encode into caller-sized storage. The caller provides one UTF-32 unit.
  constexpr char32_t* GetUtf32(char32_t* theBuffer) const { return GetUtf(theBuffer); }

  //! Encode into an array with inferred capacity.
  template <typename TypeWrite, size_t N>
  constexpr TypeWrite* GetUtf(std::array<TypeWrite, N>& theBuffer) const
  {
    return GetUtf(theBuffer.data(), N);
  }

  //! Encode with capacity in code units; follows the GetUtf() failure contract.
  constexpr char* GetUtf8(char* theBuffer, size_t theCapacity) const
  {
    return GetUtf(theBuffer, theCapacity);
  }

  //! Encode into an array with inferred capacity.
  template <size_t N>
  constexpr char* GetUtf8(std::array<char, N>& theBuffer) const
  {
    return GetUtf(theBuffer);
  }

  //! Encode with capacity in code units; follows the GetUtf() failure contract.
  constexpr unsigned char* GetUtf8(unsigned char* theBuffer, size_t theCapacity) const
  {
    return GetUtf(theBuffer, theCapacity);
  }

  //! Encode into an array with inferred capacity.
  template <size_t N>
  constexpr unsigned char* GetUtf8(std::array<unsigned char, N>& theBuffer) const
  {
    return GetUtf(theBuffer);
  }

  //! Encode with capacity in code units; follows the GetUtf() failure contract.
  constexpr char16_t* GetUtf16(char16_t* theBuffer, size_t theCapacity) const
  {
    return GetUtf(theBuffer, theCapacity);
  }

  //! Encode into an array with inferred capacity.
  template <size_t N>
  constexpr char16_t* GetUtf16(std::array<char16_t, N>& theBuffer) const
  {
    return GetUtf(theBuffer);
  }

  //! Encode with capacity in code units; follows the GetUtf() failure contract.
  constexpr char32_t* GetUtf32(char32_t* theBuffer, size_t theCapacity) const
  {
    return GetUtf(theBuffer, theCapacity);
  }

  //! Encode into an array with inferred capacity.
  template <size_t N>
  constexpr char32_t* GetUtf32(std::array<char32_t, N>& theBuffer) const
  {
    return GetUtf(theBuffer);
  }

private:
  //! Load within myRemaining, recovering from malformed input by consuming one unit.
  constexpr void readCurrent()
  {
    myPosition     = myPosNext; //!< Current code-unit position, or the range end.
    myCurrentUnits = 0;         //!< Code units occupied by the current value.
    myCharUtf32    = 0;         //!< Current scalar, invalid marker, or zero at end.
    if (myRemaining == 0)
    {
      return;
    }
    const std::optional<std::pair<char32_t, size_t>> aDecoded =
      decodeCharacter(myPosition, myRemaining);
    myCharUtf32    = aDecoded
                       ? aDecoded->first
                       : THE_INVALID_CODE_POINT; //!< Current scalar, invalid marker, or zero at end.
    myCurrentUnits = aDecoded ? aDecoded->second : 1; //!< Code units occupied by the current value.
    myPosNext += myCurrentUnits;
    myRemaining -= myCurrentUnits;
  }

  //! Decode one value within the supplied nonempty range.
  static constexpr std::optional<std::pair<char32_t, size_t>> decodeCharacter(const Type* theInput,
                                                                              size_t theCodeUnits);

  //! Write UTF-8 after the caller has checked validity and capacity.
  template <typename TypeWrite>
  constexpr TypeWrite* writeUTF8(TypeWrite* theBuffer) const;

  static constexpr char32_t THE_HIGH_SURROGATE_MIN = 0xD800;
  static constexpr char32_t THE_HIGH_SURROGATE_MAX = 0xDBFF;
  static constexpr char32_t THE_LOW_SURROGATE_MIN  = 0xDC00;
  static constexpr char32_t THE_LOW_SURROGATE_MAX  = 0xDFFF;
  static constexpr char32_t THE_BMP_MAX            = 0xFFFF;
  static constexpr char32_t THE_MAX_CODE_POINT     = 0x10FFFF;
  static constexpr char32_t THE_INVALID_CODE_POINT = char32_t(-1);

  const Type* myPosition         = nullptr; //!< Current code-unit position, or the range end.
  const Type* myPosNext          = nullptr; //!< Position after the current value.
  size_t      myRemaining        = 0;       //!< Unread code units after the current value.
  size_t      myCurrentUnits     = 0;       //!< Code units occupied by the current value.
  size_t      myCharIndex        = 0;       //!< Zero-based index of the current scalar.
  char32_t    myCharUtf32        = 0;       //!< Current scalar, invalid marker, or zero at end.
  bool        myIsNullTerminated = false;   //!< Keep null-terminated iterator end conventions.
};

#include <TCollection_UtfIterator.lxx>
#endif // TCollection_UtfIterator_HeaderFile
