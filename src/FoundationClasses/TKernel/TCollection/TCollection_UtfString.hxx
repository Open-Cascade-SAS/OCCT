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
#include <NCollection_Allocator.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>

#include <optional>
#include <string>
#include <string_view>
#include <utility>

//! Owning Unicode string with validated scalar values and cached scalar count.
//! Storage is contiguous and null-terminated; View() preserves embedded nulls.
//! Dynamic character storage uses Standard::AllocateOptimal and Standard::Free.
//! Indexes count Unicode scalars, not grapheme clusters. Views and iterators are
//! invalidated by mutation. Concurrent reads are safe; mutations require external synchronization.
template <typename Type>
class TCollection_UtfString
{
public:
  using ViewType = std::basic_string_view<Type>;

  //! Construct an empty string.
  TCollection_UtfString()                                        = default;
  TCollection_UtfString(const TCollection_UtfString&)            = default;
  TCollection_UtfString& operator=(const TCollection_UtfString&) = default;

  //! Transfer storage and leave the source empty and usable.
  TCollection_UtfString(TCollection_UtfString&& theOther) noexcept
      : myString(std::move(theOther.myString)),
        myLength(std::exchange(theOther.myLength, 0))
  {
    theOther.myString.clear();
  }

  //! Transfer storage and leave the source empty and usable; self-move is a no-op.
  TCollection_UtfString& operator=(TCollection_UtfString&& theOther) noexcept
  {
    if (this != &theOther)
    {
      myString = std::move(theOther.myString);
      myLength = std::exchange(theOther.myLength, 0);
      theOther.myString.clear();
    }
    return *this;
  }

  //! Copy a trusted null-terminated Unicode string; nullptr means empty.
  //! The caller must guarantee storage through the terminator.
  //! Throws Standard_DomainError for malformed input. Prefer a view when length is known.
  template <typename TypeFrom>
  TCollection_UtfString(const TypeFrom* theString)
  {
    if (!FromUnicode(theString))
    {
      throw Standard_DomainError("Malformed Unicode string");
    }
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

  //! @return a bounded iterator over all scalar values, including embedded nulls
  TCollection_UtfIterator<Type> Iterator() const { return {myString.data(), myString.size()}; }

  //! @return a read-only view, excluding the extra terminator
  ViewType View() const noexcept { return {myString.data(), myString.size()}; }

  //! @return number of stored code units, excluding the extra terminator
  size_t CodeUnits() const noexcept { return myString.size(); }

  //! @return storage size in bytes, excluding the extra terminator
  size_t Size() const noexcept { return myString.size() * sizeof(Type); }

  //! @return number of Unicode scalar values, including embedded nulls
  size_t Length() const noexcept { return myLength; }

  //! @return true when no code units are stored
  bool IsEmpty() const noexcept { return myString.empty(); }

  //! Retrieve a scalar by zero-based index; throws Standard_OutOfRange at or past Length().
  //! Linear time; use Iterator() for sequential access.
  char32_t GetChar(size_t theIndex) const;

  //! Retrieve a scalar by zero-based index; throws Standard_OutOfRange when absent.
  char32_t operator[](size_t theIndex) const { return GetChar(theIndex); }

  //! @return a bounded view of one scalar, or no value at or past Length()
  //! Linear time. Mutation invalidates the returned view.
  std::optional<ViewType> GetCharView(size_t theIndex) const;
  //! @return a validated substring of scalar indexes [theStart, theEnd)
  //! Throws Standard_OutOfRange for reversed or out-of-range indexes.
  TCollection_UtfString SubString(size_t theStart, size_t theEnd) const;

  //! Copy exactly theCodeUnits units. Malformed input returns false without changing this string.
  //! nullptr is accepted only with zero size. Allocation failures propagate without mutation.
  template <typename TypeFrom>
  bool FromUnicode(const TypeFrom* theString, size_t theCodeUnits);

  //! Copy a bounded Unicode view, preserving embedded nulls.
  template <typename TypeFrom, typename Traits>
  bool FromUnicode(std::basic_string_view<TypeFrom, Traits> theView)
  {
    return FromUnicode(theView.data(), theView.size());
  }

  //! Adapt a trusted null-terminated string; nullptr clears the string.
  template <typename TypeFrom>
  bool FromUnicode(const TypeFrom* theString)
  {
    size_t aSize = 0;
    if (theString != nullptr)
    {
      while (theString[aSize] != TypeFrom(0))
      {
        ++aSize;
      }
    }
    return FromUnicode(theString, aSize);
  }

  //! Copy from the current C locale (POSIX) or system ANSI code page (Windows).
  //! Conversion failure leaves this string unchanged. Windows retains system substitution rules.
  //! Locale changes must be externally synchronized with conversion.
  bool FromLocale(std::string_view theString);

  //! Adapt a trusted null-terminated locale string; nullptr clears the string.
  bool FromLocale(const char* theString)
  {
    return FromLocale(theString != nullptr ? std::string_view(theString) : std::string_view());
  }

  //! Convert to the current C locale (POSIX) or system ANSI code page (Windows), with a terminator.
  //! Capacity includes the terminator. Failure leaves the output unchanged.
  bool ToLocale(char* theBuffer, size_t theCapacity) const;

  //! @return a null-terminated read-only buffer; always non-null, including after a move
  //! Embedded nulls require View() instead of C-string traversal.
  const Type* ToCString() const noexcept { return myString.c_str(); }

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
    myString.clear();
    myLength = 0;
  }

  //! Copy with the strong exception guarantee.
  TCollection_UtfString& Assign(const TCollection_UtfString& theOther) { return *this = theOther; }

  //! Exchange storage and cached scalar counts without allocation.
  void Swap(TCollection_UtfString& theOther) noexcept
  {
    myString.swap(theOther.myString);
    std::swap(myLength, theOther.myLength);
  }

  //! Assign a trusted null-terminated Unicode string; throws on malformed input.
  template <typename TypeFrom>
  TCollection_UtfString& operator=(const TypeFrom* theString)
  {
    TCollection_UtfString aCopy(theString);
    Swap(aCopy);
    return *this;
  }

  //! Append a validated string; supports self-append and retains capacity when possible.
  TCollection_UtfString& operator+=(const TCollection_UtfString& theOther)
  {
    const size_t aLength = theOther.myLength;
    myString.append(theOther.myString);
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
    return myString == theOther.myString;
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
  using StorageType = std::basic_string<Type, std::char_traits<Type>, NCollection_Allocator<Type>>;

  StorageType myString;
  size_t      myLength = 0;
};

#include <TCollection_UtfString.lxx>
#endif // TCollection_UtfString_HeaderFile
