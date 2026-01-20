// Created on: 1993-02-22
// Created by: Mireille MERCIEN
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _TCollection_ExtendedString_HeaderFile
#define _TCollection_ExtendedString_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Character.hxx>
#include <Standard_ExtCharacter.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_Macro.hxx>

#if Standard_CPP17_OR_HIGHER
  #include <string_view>
#endif

class TCollection_AsciiString;

//! A variable-length sequence of "extended" (UNICODE) characters (16-bit character type).
//! It provides editing operations with built-in memory management
//! to make ExtendedString objects easier to use than ordinary extended character arrays.
//! ExtendedString objects follow "value semantics", that is, they are the actual strings,
//! not handles to strings, and are copied through assignment.
//! You may use HExtendedString objects to get handles to strings.
//!
//! Beware that class can transparently store UTF-16 string with surrogate pairs
//! (Unicode symbol represented by two 16-bit code units).
//! However, surrogate pairs are not considered by the following methods:
//! - Method ::Length() return the number of 16-bit code units, not the number of Unicode symbols.
//! - Methods taking/returning symbol index work with 16-bit code units, not true Unicode symbols,
//!   including ::Remove(), ::SetValue(), ::Value(), ::Search(), ::Trunc() and others.
//! If application needs to process surrogate pairs, NCollection_UtfIterator<char16_t> class can be
//! used for iterating through Unicode string (UTF-32 code unit will be returned for each position).
class TCollection_ExtendedString
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initializes an ExtendedString to an empty ExtendedString.
  Standard_EXPORT TCollection_ExtendedString() noexcept;

  //! Creation by converting a CString to an extended string.
  //! If theIsMultiByte is true then the string is treated as having UTF-8 coding.
  //! If it is not a UTF-8 then theIsMultiByte is ignored and each character is
  //! copied to ExtCharacter.
  //! @param[in] theString the C string to convert
  //! @param[in] theIsMultiByte flag indicating UTF-8 coding
  Standard_EXPORT TCollection_ExtendedString(const char* const theString,
                                             const bool        theIsMultiByte = false);

  //! Creation by converting an ExtString (char16_t*) to an extended string.
  //! @param[in] theString the char16_t string to copy
  Standard_EXPORT TCollection_ExtendedString(const char16_t* const theString);

#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
  //! Initialize from wide-char string considering it as Unicode string
  //! (the size of wide char is a platform-dependent - e.g. on Windows wchar_t is UTF-16).
  //!
  //! This constructor is unavailable if application is built with deprecated msvc option
  //! "-Zc:wchar_t-", since OCCT itself is never built with this option.
  //! @param[in] theStringUtf the wide character string to convert
  Standard_EXPORT TCollection_ExtendedString(const wchar_t* theStringUtf);
#endif

  //! Initializes an ExtendedString with a single ASCII character.
  //! @param[in] theChar the ASCII character to initialize from
  Standard_EXPORT TCollection_ExtendedString(const char theChar);

  //! Initializes an ExtendedString with a single extended character.
  //! @param[in] theChar the extended character to initialize from
  Standard_EXPORT TCollection_ExtendedString(const char16_t theChar);

  //! Initializes an ExtendedString with specified length space allocated
  //! and filled with filler character. This is useful for buffers.
  //! @param[in] theLength the length to allocate
  //! @param[in] theFiller the character to fill with
  Standard_EXPORT TCollection_ExtendedString(const int theLength, const char16_t theFiller);

  //! Initializes an ExtendedString with an integer value.
  //! @param[in] theValue the integer value to convert to string
  Standard_EXPORT TCollection_ExtendedString(const int theValue);

  //! Initializes an ExtendedString with a real value.
  //! @param[in] theValue the real value to convert to string
  Standard_EXPORT TCollection_ExtendedString(const double theValue);

  //! Initializes an ExtendedString with another ExtendedString.
  //! @param[in] theString the string to copy from
  Standard_EXPORT TCollection_ExtendedString(const TCollection_ExtendedString& theString);

  //! Move constructor.
  //! @param[in] theOther the string to move from
  Standard_EXPORT TCollection_ExtendedString(TCollection_ExtendedString&& theOther) noexcept;

  //! Creation by converting an AsciiString to an extended string.
  //! The string is treated as having UTF-8 coding.
  //! If it is not a UTF-8 or multi byte then each character is copied to ExtCharacter.
  //! @param[in] theString the ASCII string to convert
  //! @param[in] theIsMultiByte flag indicating UTF-8 coding
  Standard_EXPORT TCollection_ExtendedString(const TCollection_AsciiString& theString,
                                             const bool                     theIsMultiByte = true);

  //! Initializes an ExtendedString with a char16_t string and explicit length.
  //! @param[in] theString the char16_t string to initialize from
  //! @param[in] theLength the length of the string
  Standard_EXPORT TCollection_ExtendedString(const char16_t* const theString, const int theLength);

  //! Template constructor for char16_t string literals or arrays.
  //! For true string literals (const char16_t[N] in code), the size is computed at runtime
  //! by scanning for null terminator to handle both literals and buffers correctly.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"Hello World");  // Works with string literals
  //! char16_t buffer[100];
  //! // ... fill buffer ...
  //! TCollection_ExtendedString aBufferString(buffer);    // Works with buffers too
  //! ```
  //! @param[in] theLiteral the string literal or char16_t array
  template <std::size_t N>
  TCollection_ExtendedString(const char16_t (&theLiteral)[N])
      : TCollection_ExtendedString(theLiteral, extStringLen(theLiteral))
  {
  }

#if Standard_CPP17_OR_HIGHER
  //! Initializes an ExtendedString from a std::u16string_view.
  //! @param[in] theStringView the string view to copy
  explicit TCollection_ExtendedString(const std::u16string_view& theStringView)
  {
    myLength = static_cast<int>(theStringView.size());
    if (myLength == 0)
    {
      allocate(0);
    }
    else
    {
      allocate(myLength);
      memcpy(myString, theStringView.data(), myLength * sizeof(char16_t));
    }
  }

  //! Assignment from a std::u16string_view.
  //! @param[in] theStringView the string view to copy
  TCollection_ExtendedString& operator=(const std::u16string_view& theStringView)
  {
    const int aNewLen = static_cast<int>(theStringView.size());
    reallocate(aNewLen);
    if (aNewLen > 0)
    {
      memcpy(myString, theStringView.data(), aNewLen * sizeof(char16_t));
    }
    return *this;
  }

  //! Conversion to std::u16string_view.
  //! @return a non-owning view of the string data
  operator std::u16string_view() const noexcept { return std::u16string_view(myString, myLength); }
#endif

  //! Appends the other extended string to this extended string.
  //! Note that this method is an alias of operator +=.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"Hello");
  //! TCollection_ExtendedString anotherString(u" World");
  //! aString += anotherString;
  //! // Result: aString == u"Hello World"
  //! ```
  //! @param[in] theOther the string to append
  Standard_EXPORT void AssignCat(const TCollection_ExtendedString& theOther);

  void operator+=(const TCollection_ExtendedString& theOther) { AssignCat(theOther); }

  //! Appends the utf16 char to this extended string.
  //! @param[in] theChar the character to append
  Standard_EXPORT void AssignCat(const char16_t theChar);

  //! Core implementation: Appends char16_t string (pointer and length) to this extended string.
  //! This is the primary implementation that all other AssignCat overloads redirect to.
  //! @param[in] theString pointer to the string to append
  //! @param[in] theLength length of the string to append
  Standard_EXPORT void AssignCat(const char16_t* const theString, const int theLength);

  //! Appends the char16_t string to this extended string.
  //! @param[in] theString the string to append
  inline void AssignCat(const char16_t* const theString)
  {
    if (theString != nullptr)
    {
      AssignCat(theString, extStringLen(theString));
    }
  }

  //! Appends the char16_t string to this extended string (alias of AssignCat()).
  void operator+=(const char16_t* const theString) { AssignCat(theString); }

  //! Template method for appending char16_t string literals or arrays.
  //! For char arrays (like buffers), scans for null terminator to get actual length.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"Hello");
  //! aString += u" World";  // Works with string literals
  //! char16_t buffer[100];
  //! // ... fill buffer ...
  //! aString += buffer;     // Works with buffers too
  //! ```
  //! @param[in] theLiteral the string literal or char16_t array to append
  template <std::size_t N>
  void AssignCat(const char16_t (&theLiteral)[N])
  {
    AssignCat(theLiteral, extStringLen(theLiteral));
  }

  template <std::size_t N>
  void operator+=(const char16_t (&theLiteral)[N])
  {
    AssignCat(theLiteral, extStringLen(theLiteral));
  }

#if Standard_CPP17_OR_HIGHER
  //! Appends the std::u16string_view to this extended string.
  //! @param[in] theStringView the string view to append
  void AssignCat(const std::u16string_view& theStringView)
  {
    if (theStringView.empty())
    {
      return;
    }
    const int anOtherLen = static_cast<int>(theStringView.size());
    const int anOldLen   = myLength;
    reallocate(myLength + anOtherLen);
    memcpy(myString + anOldLen, theStringView.data(), anOtherLen * sizeof(char16_t));
  }

  //! Appends the std::u16string_view to this extended string (alias of AssignCat()).
  void operator+=(const std::u16string_view& theStringView) { AssignCat(theStringView); }
#endif

  //! Core implementation: Concatenates char16_t string (pointer and length)
  //! and returns a new string.
  //! @param[in] theOther pointer to the string to append
  //! @param[in] theLength length of the string to append
  //! @return new string with theOther appended
  Standard_EXPORT TCollection_ExtendedString Cat(const char16_t* const theOther,
                                                 const int             theLength) const;

  //! Concatenates char16_t string and returns a new string.
  //! @param[in] theOther the null-terminated string to append
  //! @return new string with theOther appended
  TCollection_ExtendedString Cat(const char16_t* const theOther) const
  {
    return Cat(theOther, extStringLen(theOther));
  }

  TCollection_ExtendedString operator+(const char16_t* const theOther) const
  {
    return Cat(theOther);
  }

  //! Appends the other extended string to this string and returns a new string.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"Hello");
  //! TCollection_ExtendedString anotherString(u" World");
  //! TCollection_ExtendedString aResult = aString + anotherString;
  //! // Result: aResult == u"Hello World"
  //! ```
  //! @param[in] theOther the string to append
  //! @return new string with theOther appended
  TCollection_ExtendedString Cat(const TCollection_ExtendedString& theOther) const
  {
    return Cat(theOther.myString, theOther.myLength);
  }

  TCollection_ExtendedString operator+(const TCollection_ExtendedString& theOther) const
  {
    return Cat(theOther);
  }

  //! Substitutes all the characters equal to theChar by theNewChar
  //! in this ExtendedString.
  //! The substitution can be case sensitive.
  //! If you don't use default case sensitive, no matter whether theChar is uppercase or not.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"Histake");
  //! aString.ChangeAll(u'H', u'M');
  //! // Result: aString == u"Mistake"
  //! ```
  //! @param[in] theChar the character to replace
  //! @param[in] theNewChar the replacement character
  Standard_EXPORT void ChangeAll(const char16_t theChar, const char16_t theNewChar);

  //! Removes all characters contained in this string.
  //! This produces an empty ExtendedString.
  Standard_EXPORT void Clear();

  //! Core implementation: Copy from a char16_t pointer with explicit length.
  //! @param[in] theString pointer to the string to copy
  //! @param[in] theLength length of the string to copy
  Standard_EXPORT void Copy(const char16_t* const theString, const int theLength);

  //! Copy from a char16_t pointer.
  //! @param[in] theString the null-terminated string to copy
  void Copy(const char16_t* const theString) { Copy(theString, extStringLen(theString)); }

  //! Copy theFromWhere to this string.
  //! Used as operator =
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString;
  //! TCollection_ExtendedString anotherString(u"Hello World");
  //! aString = anotherString;  // operator=
  //! // Result: aString == u"Hello World"
  //! ```
  //! @param[in] theFromWhere the string to copy from
  void Copy(const TCollection_ExtendedString& theFromWhere)
  {
    if (&theFromWhere != this)
    {
      Copy(theFromWhere.myString, theFromWhere.myLength);
    }
  }

  //! Copy assignment operator.
  TCollection_ExtendedString& operator=(const TCollection_ExtendedString& theOther)
  {
    Copy(theOther);
    return *this;
  }

  //! Assignment from char16_t pointer.
  TCollection_ExtendedString& operator=(const char16_t* const theString)
  {
    Copy(theString);
    return *this;
  }

  //! Moves string without reallocations.
  //! @param[in] theOther the string to move from
  Standard_EXPORT void Move(TCollection_ExtendedString&& theOther);

  //! Move assignment operator.
  TCollection_ExtendedString& operator=(TCollection_ExtendedString&& theOther) noexcept
  {
    Move(std::forward<TCollection_ExtendedString>(theOther));
    return *this;
  }

  //! Exchange the data of two strings (without reallocating memory).
  //! @param[in,out] theOther the string to exchange data with
  Standard_EXPORT void Swap(TCollection_ExtendedString& theOther);

  //! Frees memory allocated by ExtendedString.
  Standard_EXPORT ~TCollection_ExtendedString();

  //! Insert a Character at position theWhere.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"hy not ?");
  //! aString.Insert(1, u'W');
  //! // Result: aString == u"Why not ?"
  //! ```
  //! @param[in] theWhere the position to insert at (1-based)
  //! @param[in] theWhat the character to insert
  Standard_EXPORT void Insert(const int theWhere, const char16_t theWhat);

  //! Core implementation: Insert a char16_t string (pointer and length) at position theWhere.
  //! @param[in] theWhere the position to insert at (1-based)
  //! @param[in] theWhat pointer to the string to insert
  //! @param[in] theLength length of the string to insert
  Standard_EXPORT void Insert(const int             theWhere,
                              const char16_t* const theWhat,
                              const int             theLength);

  //! Insert a char16_t string at position theWhere.
  //! @param[in] theWhere the position to insert at (1-based)
  //! @param[in] theWhat the null-terminated string to insert
  void Insert(const int theWhere, const char16_t* const theWhat)
  {
    Insert(theWhere, theWhat, extStringLen(theWhat));
  }

  //! Insert an ExtendedString at position theWhere.
  //! @param[in] theWhere the position to insert at (1-based)
  //! @param[in] theWhat the string to insert
  void Insert(const int theWhere, const TCollection_ExtendedString& theWhat)
  {
    Insert(theWhere, theWhat.myString, theWhat.myLength);
  }

  //! Returns True if this string contains no characters.
  bool IsEmpty() const { return myLength == 0; }

  //! Core implementation: Returns true if this string equals theOther (pointer and length).
  //! @param[in] theOther pointer to the string to compare with
  //! @param[in] theLength length of the string to compare with
  //! @return true if strings are equal, false otherwise
  Standard_EXPORT bool IsEqual(const char16_t* const theOther, const int theLength) const;

  //! Returns true if this string equals theOther null-terminated string.
  //! Note that this method is an alias of operator ==.
  //! @param[in] theOther the char16_t string to compare with
  //! @return true if strings are equal, false otherwise
  bool IsEqual(const char16_t* const theOther) const
  {
    return IsEqual(theOther, extStringLen(theOther));
  }

  bool operator==(const char16_t* const theOther) const { return IsEqual(theOther); }

  //! Returns true if the characters in this extended
  //! string are identical to the characters in theOther extended string.
  //! Note that this method is an alias of operator ==.
  //! @param[in] theOther the extended string to compare with
  //! @return true if strings are equal, false otherwise
  bool IsEqual(const TCollection_ExtendedString& theOther) const
  {
    return myLength == theOther.myLength && IsEqual(theOther.myString, theOther.myLength);
  }

  bool operator==(const TCollection_ExtendedString& theOther) const { return IsEqual(theOther); }

  //! Core implementation: Returns true if this string differs from theOther (pointer and length).
  //! @param[in] theOther pointer to the string to compare with
  //! @param[in] theLength length of the string to compare with
  //! @return true if strings are different, false otherwise
  Standard_EXPORT bool IsDifferent(const char16_t* const theOther, const int theLength) const;

  //! Returns true if this string differs from theOther null-terminated string.
  //! Note that this method is an alias of operator !=.
  //! @param[in] theOther the char16_t string to compare with
  //! @return true if strings are different, false otherwise
  bool IsDifferent(const char16_t* const theOther) const
  {
    return IsDifferent(theOther, extStringLen(theOther));
  }

  bool operator!=(const char16_t* const theOther) const { return IsDifferent(theOther); }

  //! Returns true if there are differences between the
  //! characters in this extended string and theOther extended string.
  //! Note that this method is an alias of operator !=.
  //! @param[in] theOther the extended string to compare with
  //! @return true if strings are different, false otherwise
  bool IsDifferent(const TCollection_ExtendedString& theOther) const
  {
    return IsDifferent(theOther.myString, theOther.myLength);
  }

  bool operator!=(const TCollection_ExtendedString& theOther) const
  {
    return IsDifferent(theOther);
  }

  //! Core implementation: Returns TRUE if this string is lexicographically less than theOther.
  //! @param[in] theOther pointer to the string to compare with
  //! @param[in] theLength length of the string to compare with
  //! @return true if this string is less than theOther
  Standard_EXPORT bool IsLess(const char16_t* const theOther, const int theLength) const;

  //! Returns TRUE if this string is lexicographically less than theOther.
  //! @param[in] theOther the char16_t string to compare with
  //! @return true if this string is less than theOther
  bool IsLess(const char16_t* const theOther) const
  {
    return IsLess(theOther, extStringLen(theOther));
  }

  bool operator<(const char16_t* const theOther) const { return IsLess(theOther); }

  //! Returns TRUE if this string is lexicographically less than theOther.
  //! @param[in] theOther the extended string to compare with
  //! @return true if this string is less than theOther
  bool IsLess(const TCollection_ExtendedString& theOther) const
  {
    return IsLess(theOther.myString, theOther.myLength);
  }

  bool operator<(const TCollection_ExtendedString& theOther) const { return IsLess(theOther); }

  //! Core implementation: Returns TRUE if this string is lexicographically greater than theOther.
  //! @param[in] theOther pointer to the string to compare with
  //! @param[in] theLength length of the string to compare with
  //! @return true if this string is greater than theOther
  Standard_EXPORT bool IsGreater(const char16_t* const theOther, const int theLength) const;

  //! Returns TRUE if this string is lexicographically greater than theOther.
  //! @param[in] theOther the char16_t string to compare with
  //! @return true if this string is greater than theOther
  bool IsGreater(const char16_t* const theOther) const
  {
    return IsGreater(theOther, extStringLen(theOther));
  }

  bool operator>(const char16_t* const theOther) const { return IsGreater(theOther); }

  //! Returns TRUE if this string is lexicographically greater than theOther.
  //! @param[in] theOther the extended string to compare with
  //! @return true if this string is greater than theOther
  bool IsGreater(const TCollection_ExtendedString& theOther) const
  {
    return IsGreater(theOther.myString, theOther.myLength);
  }

  bool operator>(const TCollection_ExtendedString& theOther) const { return IsGreater(theOther); }

  //! Core implementation: Determines whether this string starts with theStartString.
  //! @param[in] theStartString pointer to the string to check for
  //! @param[in] theLength length of the string to check for
  //! @return true if this string starts with theStartString
  Standard_EXPORT bool StartsWith(const char16_t* const theStartString, const int theLength) const;

  //! Determines whether this string starts with theStartString.
  //! @param[in] theStartString the null-terminated string to check for
  //! @return true if this string starts with theStartString
  bool StartsWith(const char16_t* const theStartString) const
  {
    return StartsWith(theStartString, extStringLen(theStartString));
  }

  //! Determines whether the beginning of this string instance matches the specified string.
  //! @param[in] theStartString the string to check for at the beginning
  //! @return true if this string starts with theStartString
  bool StartsWith(const TCollection_ExtendedString& theStartString) const
  {
    return StartsWith(theStartString.myString, theStartString.myLength);
  }

  //! Core implementation: Determines whether this string ends with theEndString.
  //! @param[in] theEndString pointer to the string to check for
  //! @param[in] theLength length of the string to check for
  //! @return true if this string ends with theEndString
  Standard_EXPORT bool EndsWith(const char16_t* const theEndString, const int theLength) const;

  //! Determines whether this string ends with theEndString.
  //! @param[in] theEndString the null-terminated string to check for
  //! @return true if this string ends with theEndString
  bool EndsWith(const char16_t* const theEndString) const
  {
    return EndsWith(theEndString, extStringLen(theEndString));
  }

  //! Determines whether the end of this string instance matches the specified string.
  //! @param[in] theEndString the string to check for at the end
  //! @return true if this string ends with theEndString
  bool EndsWith(const TCollection_ExtendedString& theEndString) const
  {
    return EndsWith(theEndString.myString, theEndString.myLength);
  }

  //! Returns True if the ExtendedString contains only "Ascii Range" characters.
  //! @return true if string contains only ASCII characters
  Standard_EXPORT bool IsAscii() const;

  //! Returns the number of 16-bit code units
  //! (might be greater than number of Unicode symbols if string contains surrogate pairs).
  //! @return the number of 16-bit code units
  Standard_EXPORT int Length() const;

  //! Displays this string on a stream.
  //! @param[in] theStream the output stream
  Standard_EXPORT void                     Print(Standard_OStream& theStream) const;
  friend Standard_EXPORT Standard_OStream& operator<<(Standard_OStream&                 theStream,
                                                      const TCollection_ExtendedString& theString);

  //! Removes every theWhat characters from this string.
  //! @param[in] theWhat the character to remove
  Standard_EXPORT void RemoveAll(const char16_t theWhat);

  //! Erases theHowMany characters from position theWhere, theWhere included.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"Hello");
  //! aString.Remove(2, 2); // erases 2 characters from position 2
  //! // Result: aString == u"Hlo"
  //! ```
  //! @param[in] theWhere the position to start erasing from (1-based)
  //! @param[in] theHowMany the number of characters to erase
  Standard_EXPORT void Remove(const int theWhere, const int theHowMany = 1);

  //! Core implementation: Searches for theWhat (pointer and length) from the beginning.
  //! @param[in] theWhat pointer to the string to search for
  //! @param[in] theLength length of the string to search for
  //! @return the position of first match (1-based), or -1 if not found
  Standard_EXPORT int Search(const char16_t* const theWhat, const int theLength) const;

  //! Searches for theWhat null-terminated string from the beginning.
  //! @param[in] theWhat the null-terminated string to search for
  //! @return the position of first match (1-based), or -1 if not found
  int Search(const char16_t* const theWhat) const { return Search(theWhat, extStringLen(theWhat)); }

  //! Searches an ExtendedString in this string from the beginning
  //! and returns position of first item theWhat matching.
  //! It returns -1 if not found.
  //! @param[in] theWhat the string to search for
  //! @return the position of first match (1-based), or -1 if not found
  int Search(const TCollection_ExtendedString& theWhat) const
  {
    return Search(theWhat.myString, theWhat.myLength);
  }

  //! Core implementation: Searches for theWhat (pointer and length) from the end.
  //! @param[in] theWhat pointer to the string to search for
  //! @param[in] theLength length of the string to search for
  //! @return the position of first match from end (1-based), or -1 if not found
  Standard_EXPORT int SearchFromEnd(const char16_t* const theWhat, const int theLength) const;

  //! Searches for theWhat null-terminated string from the end.
  //! @param[in] theWhat the null-terminated string to search for
  //! @return the position of first match from end (1-based), or -1 if not found
  int SearchFromEnd(const char16_t* const theWhat) const
  {
    return SearchFromEnd(theWhat, extStringLen(theWhat));
  }

  //! Searches an ExtendedString in this string from the end
  //! and returns position of first item theWhat matching.
  //! It returns -1 if not found.
  //! @param[in] theWhat the string to search for
  //! @return the position of first match from end (1-based), or -1 if not found
  int SearchFromEnd(const TCollection_ExtendedString& theWhat) const
  {
    return SearchFromEnd(theWhat.myString, theWhat.myLength);
  }

  //! Replaces one character in the ExtendedString at position theWhere.
  //! If theWhere is less than zero or greater than the length of this string
  //! an exception is raised.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"Garbake");
  //! aString.SetValue(6, u'g');
  //! // Result: aString == u"Garbage"
  //! ```
  //! @param[in] theWhere the position to replace at (1-based)
  //! @param[in] theWhat the character to replace with
  Standard_EXPORT void SetValue(const int theWhere, const char16_t theWhat);

  //! Core implementation: Replaces a part of this string by char16_t string (pointer and length).
  //! @param[in] theWhere the position to start replacement (1-based)
  //! @param[in] theWhat pointer to the string to replace with
  //! @param[in] theLength length of the string to replace with
  Standard_EXPORT void SetValue(const int             theWhere,
                                const char16_t* const theWhat,
                                const int             theLength);

  //! Replaces a part of this string by a null-terminated char16_t string.
  //! @param[in] theWhere the position to start replacement (1-based)
  //! @param[in] theWhat the null-terminated string to replace with
  void SetValue(const int theWhere, const char16_t* const theWhat)
  {
    SetValue(theWhere, theWhat, extStringLen(theWhat));
  }

  //! Replaces a part of this string by another ExtendedString.
  //! @param[in] theWhere the position to start replacement (1-based)
  //! @param[in] theWhat the string to replace with
  void SetValue(const int theWhere, const TCollection_ExtendedString& theWhat)
  {
    SetValue(theWhere, theWhat.myString, theWhat.myLength);
  }

  //! Copies characters from this string starting from index theFromIndex
  //! to the index theToIndex (inclusive).
  //! Raises an exception if theToIndex or theFromIndex is out of bounds.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"abcdefg");
  //! TCollection_ExtendedString aSubString = aString.SubString(3, 6);
  //! // Result: aSubString == u"cdef"
  //! ```
  //! @param[in] theFromIndex the starting index (1-based)
  //! @param[in] theToIndex the ending index (1-based, inclusive)
  //! @return the substring from theFromIndex to theToIndex
  Standard_EXPORT TCollection_ExtendedString SubString(const int theFromIndex,
                                                       const int theToIndex) const;

  //! Splits this extended string into two sub-strings at position theWhere.
  //! -   The second sub-string (from position theWhere + 1 of this string to the end) is
  //!     returned in a new extended string.
  //! -   This extended string is modified: its last characters are removed, it becomes equal to
  //!     the first sub-string (from the first character to position theWhere).
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"abcdefg");
  //! TCollection_ExtendedString aSecondPart = aString.Split(3);
  //! // Result: aString == u"abc" and aSecondPart == u"defg"
  //! ```
  //! @param[in] theWhere the position to split at (0-based)
  //! @return the second part of the split string
  Standard_EXPORT TCollection_ExtendedString Split(const int theWhere);

  //! Extracts theWhichOne token from this string.
  //! By default, the theSeparators is set to space and tabulation.
  //! By default, the token extracted is the first one (theWhichOne = 1).
  //! theSeparators contains all separators you need.
  //! If no token indexed by theWhichOne is found, it returns an empty ExtendedString.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"This is a     message");
  //! TCollection_ExtendedString aToken1 = aString.Token();
  //! // Result: aToken1 == u"This"
  //!
  //! TCollection_ExtendedString aToken2 = aString.Token(u" ", 4);
  //! // Result: aToken2 == u"message"
  //!
  //! TCollection_ExtendedString aToken3 = aString.Token(u" ", 2);
  //! // Result: aToken3 == u"is"
  //!
  //! TCollection_ExtendedString aToken4 = aString.Token(u" ", 9);
  //! // Result: aToken4 == u""
  //!
  //! TCollection_ExtendedString bString(u"1234; test:message   , value");
  //! TCollection_ExtendedString bToken1 = bString.Token(u"; :,", 4);
  //! // Result: bToken1 == u"value"
  //! ```
  //! @param[in] theSeparators the separator characters
  //! @param[in] theWhichOne the token number to extract (1-based)
  //! @return the extracted token
  Standard_EXPORT TCollection_ExtendedString Token(const char16_t* const theSeparators,
                                                   const int             theWhichOne = 1) const;

  //! Returns pointer to ExtString (char16_t*).
  //! @return the char16_t string representation
  Standard_EXPORT const char16_t* ToExtString() const;

#ifdef _WIN32
  //! Returns pointer to string as wchar_t* on Windows platform where wchar_t* is considered as
  //! UTF-16 string. This method is useful to pass string into wide-char system APIs, and makes
  //! sense only on Windows (other systems use UTF-8 and can miss wide-char functions at all).
  //! @return the wchar_t string representation
  const wchar_t* ToWideString() const { return (const wchar_t*)ToExtString(); }
#endif

  //! Truncates this string to theHowMany characters.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"Hello Dolly");
  //! aString.Trunc(3);
  //! // Result: aString == u"Hel"
  //! ```
  //! @param[in] theHowMany the number of characters to keep
  Standard_EXPORT void Trunc(const int theHowMany);

  //! Returns character at position theWhere in this string.
  //! If theWhere is less than zero or greater than the length of
  //! this string, an exception is raised.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_ExtendedString aString(u"Hello");
  //! char16_t aChar = aString.Value(2);
  //! // Result: aChar == u'e'
  //! ```
  //! @param[in] theWhere the position to get character from (1-based)
  //! @return the character at the specified position
  Standard_EXPORT char16_t Value(const int theWhere) const;

  //! Returns a hashed value for the extended string.
  //! Note: if string is ASCII, the computed value is the same as the value computed with the
  //! HashCode function on a TCollection_AsciiString string composed with equivalent ASCII
  //! characters.
  //! @return a computed hash code
  size_t HashCode() const
  {
    const int aSize = myLength * sizeof(char16_t);
    if (myLength < 2)
    {
      return opencascade::FNVHash::hash_combine(*myString, aSize);
    }
    return opencascade::hashBytes(myString, aSize);
  }

  //! Returns a const reference to a single shared empty string instance.
  //! This method provides access to a static empty string to avoid creating temporary empty
  //! strings. Use this method instead of constructing empty strings when you need a const
  //! reference.
  //!
  //! Example:
  //! ```cpp
  //! const TCollection_ExtendedString& anEmptyStr = TCollection_ExtendedString::EmptyString();
  //! // Use anEmptyStr instead of TCollection_ExtendedString()
  //! ```
  //! @return const reference to static empty string
  Standard_EXPORT static const TCollection_ExtendedString& EmptyString() noexcept;

  //! Returns true if the characters in this extended
  //! string are identical to the characters in the other extended string.
  //! Note that this method is an alias of operator ==.
  //! @param[in] theString1 first string to compare
  //! @param[in] theString2 second string to compare
  //! @return true if strings are equal
  static bool IsEqual(const TCollection_ExtendedString& theString1,
                      const TCollection_ExtendedString& theString2)
  {
    return theString1.IsEqual(theString2);
  }

  //! Converts the internal myString to UTF8 coding and
  //! returns length of the out CString. A memory for the
  //! theCString should be allocated before call!
  //! @param[in,out] theCString pointer to the output buffer
  //! @return length of the UTF-8 string
  Standard_EXPORT int ToUTF8CString(Standard_PCharacter& theCString) const;

  //! Returns expected CString length in UTF8 coding (like strlen, without null terminator).
  //! It can be used for memory calculation before converting to CString containing symbols in UTF8
  //! coding. For external allocation, use: char* buf = new char[str.LengthOfCString() + 1];
  //! @return expected UTF-8 string length
  Standard_EXPORT int LengthOfCString() const;

  //! Removes all space characters in the beginning of the string.
  Standard_EXPORT void LeftAdjust();

  //! Removes all space characters at the end of the string.
  Standard_EXPORT void RightAdjust();

  //! Left justify.
  //! Length becomes equal to theWidth and the new characters are
  //! equal to theFiller.
  //! If theWidth < Length nothing happens.
  //! @param[in] theWidth the desired width of the string
  //! @param[in] theFiller the character to fill with
  Standard_EXPORT void LeftJustify(const int theWidth, const char16_t theFiller);

  //! Right justify.
  //! Length becomes equal to theWidth and the new characters are
  //! equal to theFiller.
  //! If theWidth < Length nothing happens.
  //! @param[in] theWidth the desired width of the string
  //! @param[in] theFiller the character to fill with
  Standard_EXPORT void RightJustify(const int theWidth, const char16_t theFiller);

  //! Modifies this string so that its length becomes equal to theWidth
  //! and the new characters are equal to theFiller.
  //! New characters are added both at the beginning and at the end of this string.
  //! If theWidth is less than the length of this string, nothing happens.
  //! @param[in] theWidth the desired width of the string
  //! @param[in] theFiller the character to fill with
  Standard_EXPORT void Center(const int theWidth, const char16_t theFiller);

  //! Converts the first character into its corresponding
  //! upper-case character and the other characters into lowercase.
  //! @note Only ASCII characters (a-z, A-Z) are affected by case conversion.
  Standard_EXPORT void Capitalize();

  //! Core implementation: Inserts char16_t string (pointer and length) at the beginning.
  //! @param[in] theOther pointer to the string to prepend
  //! @param[in] theLength length of the string to prepend
  Standard_EXPORT void Prepend(const char16_t* const theOther, const int theLength);

  //! Inserts a null-terminated char16_t string at the beginning.
  //! @param[in] theOther the null-terminated string to prepend
  void Prepend(const char16_t* const theOther) { Prepend(theOther, extStringLen(theOther)); }

  //! Inserts the other extended string at the beginning of this string.
  //! @param[in] theOther the string to prepend
  void Prepend(const TCollection_ExtendedString& theOther)
  {
    Prepend(theOther.myString, theOther.myLength);
  }

  //! Returns the index of the first character of this string that is
  //! present in theSet.
  //! The search begins at index theFromIndex and ends at index theToIndex.
  //! Returns zero if failure.
  //! @param[in] theSet the set of characters to search for
  //! @param[in] theFromIndex the starting index for search (1-based)
  //! @param[in] theToIndex the ending index for search (1-based)
  //! @return the index of first character found in set, or 0 if not found
  Standard_EXPORT int FirstLocationInSet(const TCollection_ExtendedString& theSet,
                                         const int                         theFromIndex,
                                         const int                         theToIndex) const;

  //! Returns the index of the first character of this string that is
  //! NOT present in theSet.
  //! The search begins at index theFromIndex and ends at index theToIndex.
  //! Returns zero if failure.
  //! @param[in] theSet the set of characters to check against
  //! @param[in] theFromIndex the starting index for search (1-based)
  //! @param[in] theToIndex the ending index for search (1-based)
  //! @return the index of first character not in set, or 0 if not found
  Standard_EXPORT int FirstLocationNotInSet(const TCollection_ExtendedString& theSet,
                                            const int                         theFromIndex,
                                            const int                         theToIndex) const;

  //! Converts this extended string containing a numeric expression to an Integer.
  //! @return the integer value
  Standard_EXPORT int IntegerValue() const;

  //! Returns True if this extended string contains an integer value.
  //! @return true if string represents an integer value
  Standard_EXPORT bool IsIntegerValue() const;

  //! Converts this extended string containing a numeric expression to a Real.
  //! @return the real value
  Standard_EXPORT double RealValue() const;

  //! Returns True if this extended string starts with characters that can be
  //! interpreted as a real value.
  //! @param[in] theToCheckFull when TRUE, checks if entire string defines a real value;
  //!                           otherwise checks if string starts with a real value
  //! @return true if string represents a real value
  Standard_EXPORT bool IsRealValue(bool theToCheckFull = false) const;

  //! Returns True if the strings contain same characters.
  //! @param[in] theOther the string to compare with
  //! @param[in] theIsCaseSensitive flag indicating case sensitivity
  //! @note When case-insensitive, only ASCII characters (a-z, A-Z) are affected.
  //! @return true if strings contain same characters
  Standard_EXPORT bool IsSameString(const TCollection_ExtendedString& theOther,
                                    const bool                        theIsCaseSensitive) const;

private:
  //! Returns true if the input CString was successfully converted to UTF8 coding.
  bool ConvertToUnicode(const char* const theString);

  //! Internal wrapper to allocate on stack or heap
  void allocate(const int theLength);

  //! Internal wrapper to reallocate on stack or heap
  void reallocate(const int theLength);

  //! Internal wrapper to deallocate on stack
  void deallocate();

  //! Helper function to compute length of char16_t string (like strlen for char).
  //! Scans for null terminator to find actual string length.
  //! @param[in] theString the char16_t string
  //! @return length of string (excluding null terminator)
  static int extStringLen(const char16_t* const theString)
  {
    if (theString == nullptr)
    {
      return 0;
    }
    int aLen = 0;
    while (theString[aLen] != u'\0')
    {
      ++aLen;
    }
    return aLen;
  }

private:
  char16_t* myString{}; //!< NULL-terminated string
  int       myLength{}; //!< length in 16-bit code units (excluding terminating NULL symbol)
};

namespace std
{
template <>
struct hash<TCollection_ExtendedString>
{
  size_t operator()(const TCollection_ExtendedString& theString) const
  {
    return theString.HashCode();
  }
};
} // namespace std

#endif // _TCollection_ExtendedString_HeaderFile
