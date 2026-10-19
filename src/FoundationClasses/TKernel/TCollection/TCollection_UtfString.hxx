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

#ifndef TCollection_UtfString_HeaderFile
#define TCollection_UtfString_HeaderFile

#include <TCollection_UtfIterator.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>

#include <optional>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>

//! Owning Unicode string with validated scalar values and cached scalar count.
//! Storage is contiguous and null-terminated; View() preserves embedded nulls.
//! Character storage uses NCollection_LinearVector.
//! Byte counts and scalar counts fit in int. Indexes count scalars, not grapheme clusters. Views
//! and iterators are invalidated by mutation. Concurrent reads are safe; mutations require external
//! synchronization.
//! @code
//! TCollection_UtfString<char> aText(std::u16string_view(u"text"));
//! const TCollection_UtfString<char16_t> aUtf16 = aText.ToUtf16();
//! @endcode
template <typename Type>
class TCollection_UtfString
{
public:
  using ViewType = std::basic_string_view<Type>;

  //! Construct an empty string.
  TCollection_UtfString() = default;
  //! Copy the code units and cached scalar count.
  TCollection_UtfString(const TCollection_UtfString&) = default;

  //! Replace contents with an independent copy; allocation failure leaves this string unchanged.
  const TCollection_UtfString& operator=(const TCollection_UtfString& theOther)
  {
    return Assign(theOther);
  }

  //! Transfer storage and leave the source empty and usable.
  TCollection_UtfString(TCollection_UtfString&& theOther) noexcept
      : myString(std::move(theOther.myString)),
        myLength(std::exchange(theOther.myLength, 0))
  {
    theOther.Clear();
  }

  //! Exchange contents with the source; self-move is a no-op.
  TCollection_UtfString& operator=(TCollection_UtfString&& theOther) noexcept
  {
    if (this != &theOther)
    {
      Swap(theOther);
    }
    return *this;
  }

  //! Copy a trusted null-terminated Unicode string; nullptr means empty.
  //! The caller must guarantee storage through the terminator.
  //! Throws Standard_DomainError for malformed input. Prefer a view when length is known.
  template <typename TypeFrom>
  TCollection_UtfString(const TypeFrom* theString)
  {
    FromUnicode(theString, -1);
  }

  //! Copy a null-terminated prefix of at most theLength Unicode scalars.
  //! Non-positive lengths select the whole string. nullptr means empty.
  //! Malformed input throws Standard_DomainError. Use size_t for counted code units.
  template <typename TypeFrom>
  TCollection_UtfString(const TypeFrom* theString, int theLength)
  {
    FromUnicode(theString, theLength);
  }

  //! Copy exactly theCodeUnits input units, including embedded nulls.
  //! Throws Standard_DomainError for malformed input or nullptr with nonzero size.
  template <typename TypeFrom>
  TCollection_UtfString(const TypeFrom* theString, size_t theCodeUnits)
  {
    if (!FromUnicode(theString, theCodeUnits))
    {
      throw Standard_DomainError("Malformed Unicode string");
    }
  }

  //! Copy a bounded Unicode view; throws Standard_DomainError for malformed input.
  template <typename TypeFrom, typename Traits>
  explicit TCollection_UtfString(std::basic_string_view<TypeFrom, Traits> theView)
      : TCollection_UtfString(theView.data(), theView.size())
  {
  }

  //! @return an iterator over null-terminated text, including the first null scalar
  TCollection_UtfIterator<Type> Iterator() const
  {
    return TCollection_UtfIterator<Type>(ToCString());
  }

  //! @return a bounded iterator over all stored scalars, including embedded nulls
  TCollection_UtfIterator<Type> BoundedIterator() const { return {ToCString(), CodeUnits()}; }

  //! @return a read-only view, excluding the extra terminator
  ViewType View() const noexcept { return {ToCString(), CodeUnits()}; }

  //! @return number of stored code units, excluding the extra terminator
  size_t CodeUnits() const noexcept { return myString.IsEmpty() ? 0 : myString.Size() - 1; }

  //! @return storage size in bytes, excluding the extra terminator
  int Size() const noexcept { return static_cast<int>(ByteSize()); }

  //! @return storage size in bytes as a size_t count
  size_t ByteSize() const noexcept { return CodeUnits() * sizeof(Type); }

  //! @return number of Unicode scalar values, including embedded nulls
  int Length() const noexcept { return static_cast<int>(myLength); }

  //! @return the number of stored Unicode scalars as a size_t count
  size_t ScalarCount() const noexcept { return myLength; }

  //! @return true when no code units are stored
  bool IsEmpty() const noexcept { return myLength == 0; }

  //! Retrieve a scalar by zero-based index; throws Standard_OutOfRange at or past Length().
  //! Linear time; use Iterator() for sequential access.
  char32_t GetChar(size_t theIndex) const;

  //! Retrieve a scalar by zero-based index; throws Standard_OutOfRange when absent.
  char32_t operator[](size_t theIndex) const { return GetChar(theIndex); }

  //! @return the scalar at a signed index, or zero when the index is absent
  char32_t GetChar(int theIndex) const
  {
    return theIndex < 0 || static_cast<size_t>(theIndex) >= ScalarCount()
             ? 0
             : GetChar(static_cast<size_t>(theIndex));
  }

  //! @return the scalar at a signed index, or zero when the index is absent
  char32_t operator[](int theIndex) const { return GetChar(theIndex); }

  //! @return the borrowed buffer at a scalar index, or nullptr when absent
  //! Mutation invalidates the pointer.
  const Type* GetCharBuffer(int theIndex) const
  {
    if (theIndex < 0)
    {
      return nullptr;
    }
    const std::optional<ViewType> aView = GetCharView(static_cast<size_t>(theIndex));
    return aView ? aView->data() : nullptr;
  }

  //! Copy a signed scalar interval. Missing end positions are clamped to Length().
  //! Negative starts select the first scalar and keep theEnd - theStart as the count.
  //! Reversed or missing intervals are empty.
  TCollection_UtfString SubString(int theStart, int theEnd) const
  {
    if (theStart >= theEnd || static_cast<size_t>(std::max(theStart, 0)) >= ScalarCount())
    {
      return {};
    }
    const size_t aStart = static_cast<size_t>(std::max(theStart, 0));
    const size_t aCount = static_cast<size_t>(static_cast<int64_t>(theEnd) - theStart);
    return SubString(aStart, aStart + std::min(aCount, ScalarCount() - aStart));
  }

  //! @return a bounded view of one scalar, or no value at or past Length()
  //! Linear time. Mutation invalidates the returned view.
  std::optional<ViewType> GetCharView(size_t theIndex) const;
  //! @return a validated substring of scalar indexes [theStart, theEnd)
  //! Throws Standard_OutOfRange for reversed or out-of-range indexes.
  TCollection_UtfString SubString(size_t theStart, size_t theEnd) const;

  //! Copy a null-terminated prefix of at most theLength Unicode scalars.
  //! Non-positive lengths select the whole string. nullptr clears the string.
  //! Malformed input throws Standard_DomainError without changing this string.
  template <typename TypeFrom>
  void FromUnicode(const TypeFrom* theString, int theLength = -1)
  {
    if (theString == nullptr)
    {
      Clear();
      return;
    }
    TCollection_UtfIterator<TypeFrom> anIter(
      theString,
      TCollection_UtfIterator<TypeFrom>::InputMode::NullTerminated);
    while (anIter.More()
           && (theLength <= 0 || anIter.IndexOffset() < static_cast<size_t>(theLength)))
    {
      if (!anIter.IsValid())
      {
        throw Standard_DomainError("Malformed Unicode string");
      }
      ++anIter;
    }
    const size_t aUnits = static_cast<size_t>(anIter.BufferHere() - theString);
    if (!FromUnicode(theString, aUnits))
    {
      throw Standard_DomainError("Malformed Unicode string");
    }
  }

  //! Copy exactly theCodeUnits units. Malformed input returns false without changing this string.
  //! nullptr is accepted only with zero size. Allocation failures propagate without mutation.
  template <typename TypeFrom>
  [[nodiscard]] bool FromUnicode(const TypeFrom* theString, size_t theCodeUnits);

  //! Copy a bounded Unicode view, preserving embedded nulls.
  template <typename TypeFrom, typename Traits>
  [[nodiscard]] bool FromUnicode(std::basic_string_view<TypeFrom, Traits> theView)
  {
    return FromUnicode(theView.data(), theView.size());
  }

  //! Copy from the current C locale (POSIX) or system ANSI code page (Windows).
  //! Conversion failure leaves this string unchanged. Windows retains system substitution rules.
  //! Locale changes must be externally synchronized with conversion.
  [[nodiscard]] bool FromLocale(std::string_view theString);

  //! Copy at most theLength locale characters; non-positive lengths select the whole string.
  //! Conversion failure clears this string.
  void FromLocale(const char* theString, int theLength = -1)
  {
#if defined(__ANDROID__)
    FromUnicode(theString, theLength);
#else
    TCollection_UtfString aCopy;
    if (!aCopy.FromLocale(theString != nullptr ? std::string_view(theString) : std::string_view()))
    {
      Clear();
      return;
    }
    if (theLength > 0 && static_cast<size_t>(theLength) < aCopy.ScalarCount())
    {
      aCopy = aCopy.SubString(size_t(0), static_cast<size_t>(theLength));
    }
    Swap(aCopy);
#endif
  }

  //! Convert to a signed byte capacity, including the terminator.
  //! Failure clears the first byte of a positive-size buffer, except on Android.
  //! On POSIX, empty output returns false, as in the null-terminated locale API.
  bool ToLocale(char* theBuffer, int theCapacity) const
  {
#if defined(__ANDROID__)
    const TCollection_UtfString<char> aUtf8 = ToUtf8();
    if (theBuffer == nullptr || theCapacity <= 0
        || aUtf8.CodeUnits() >= static_cast<size_t>(theCapacity))
    {
      return false;
    }
    std::copy_n(aUtf8.ToCString(), aUtf8.CodeUnits() + 1, theBuffer);
    return true;
#else
  #if !defined(_WIN32)
    if (IsEmpty())
    {
      if (theBuffer != nullptr && theCapacity > 0)
      {
        theBuffer[0] = '\0';
      }
      return false;
    }
  #endif
    if (theCapacity > 0 && ToLocale(theBuffer, static_cast<size_t>(theCapacity)))
    {
      return true;
    }
    if (theBuffer != nullptr && theCapacity > 0)
    {
      theBuffer[0] = '\0';
    }
    return false;
#endif
  }

  //! Convert to the current C locale (POSIX) or system ANSI code page (Windows), with a terminator.
  //! Capacity includes the terminator. Failure leaves the output unchanged.
  [[nodiscard]] bool ToLocale(char* theBuffer, size_t theCapacity) const;

  //! @return a null-terminated read-only buffer; always non-null, including after a move
  //! Embedded nulls require View() instead of C-string traversal.
  const Type* ToCString() const noexcept
  {
    return myString.IsEmpty() ? &THE_EMPTY : myString.Data();
  }

  //! @return a validated copy in the requested encoding
  TCollection_UtfString<char> ToUtf8() const { return TCollection_UtfString<char>(View()); }

  TCollection_UtfString<char16_t> ToUtf16() const
  {
    return TCollection_UtfString<char16_t>(View());
  }

  TCollection_UtfString<char32_t> ToUtf32() const
  {
    return TCollection_UtfString<char32_t>(View());
  }

  TCollection_UtfString<wchar_t> ToUtfWide() const
  {
    return TCollection_UtfString<wchar_t>(View());
  }

  //! Clear contents while retaining reusable capacity.
  void Clear() noexcept
  {
    myString.Clear(false);
    myLength = 0;
  }

  //! Copy with the strong exception guarantee.
  const TCollection_UtfString& Assign(const TCollection_UtfString& theOther)
  {
    if (this != &theOther)
    {
      TCollection_UtfString aCopy(theOther);
      Swap(aCopy);
    }
    return *this;
  }

  //! Exchange storage and cached scalar counts without allocation.
  void Swap(TCollection_UtfString& theOther) noexcept
  {
    std::swap(myString, theOther.myString);
    std::swap(myLength, theOther.myLength);
  }

  //! Assign a trusted null-terminated Unicode string; throws on malformed input.
  template <typename TypeFrom>
  const TCollection_UtfString& operator=(const TypeFrom* theString)
  {
    TCollection_UtfString aCopy(theString);
    Swap(aCopy);
    return *this;
  }

  //! Append a validated string; supports self-append and retains capacity when possible.
  TCollection_UtfString& operator+=(const TCollection_UtfString& theOther)
  {
    const size_t aLength      = theOther.myLength;
    const size_t aUnits       = CodeUnits();
    const size_t anAddedUnits = theOther.CodeUnits();
    if (anAddedUnits == 0)
    {
      return *this;
    }
    if (anAddedUnits > maxCodeUnits() - aUnits)
    {
      throw Standard_RangeError("Appended Unicode string is too large");
    }
    myString.Resize(aUnits + anAddedUnits + 1);
    // Resolve the source after growth so self-append does not keep an invalid pointer.
    std::copy_n(theOther.ToCString(), anAddedUnits, myString.Data() + aUnits);
    myString[aUnits + anAddedUnits] = Type(0);
    myLength += aLength;
    return *this;
  }

  friend TCollection_UtfString operator+(TCollection_UtfString        theLeft,
                                         const TCollection_UtfString& theRight)
  {
    theLeft += theRight;
    return theLeft;
  }

  //! Compare all code units, including embedded nulls.
  bool IsEqual(const TCollection_UtfString& theOther) const noexcept
  {
    return View() == theOther.View();
  }

  bool operator==(const TCollection_UtfString& theOther) const noexcept
  {
    return IsEqual(theOther);
  }

  bool operator!=(const TCollection_UtfString& theOther) const noexcept
  {
    return !IsEqual(theOther);
  }

private:
  using StorageType = NCollection_LinearVector<Type>;

  //! Maximum stored units keeping signed byte counts and iterator indexes representable.
  static constexpr size_t maxCodeUnits() noexcept
  {
    return std::min(static_cast<size_t>(std::numeric_limits<int>::max()) / sizeof(Type),
                    static_cast<size_t>(std::numeric_limits<std::ptrdiff_t>::max()))
           - 1;
  }

  static constexpr Type THE_EMPTY = Type(0); //!< Terminator for empty or moved strings.
  StorageType myString; //!< Code units and terminator; empty storage represents an empty string.
  size_t      myLength = 0; //!< Number of stored Unicode scalar values.
};

#include <TCollection_UtfString.lxx>
#endif // TCollection_UtfString_HeaderFile
