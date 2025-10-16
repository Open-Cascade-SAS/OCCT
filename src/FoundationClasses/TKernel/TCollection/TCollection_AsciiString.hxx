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

#ifndef _TCollection_AsciiString_HeaderFile
#define _TCollection_AsciiString_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_PCharacter.hxx>
#include <Standard_CString.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Standard_Macro.hxx>

#include <string_view>

class TCollection_ExtendedString;

//! Class defines a variable-length sequence of 8-bit characters.
//! Despite class name (kept for historical reasons), it is intended to store UTF-8 string, not just
//! ASCII characters. However, multi-byte nature of UTF-8 is not considered by the following
//! methods:
//! - Method ::Length() return the number of bytes, not the number of Unicode symbols.
//! - Methods taking/returning symbol index work with 8-bit code units, not true Unicode symbols,
//!   including ::Remove(), ::SetValue(), ::Value(), ::Search(), ::Trunc() and others.
//! If application needs to process multi-byte Unicode symbols explicitly, NCollection_Utf8Iter
//! class can be used for iterating through Unicode string (UTF-32 code unit will be returned for
//! each position).
//!
//! Class provides editing operations with built-in memory management to make AsciiString objects
//! easier to use than ordinary character arrays. AsciiString objects follow value semantics; in
//! other words, they are the actual strings, not handles to strings, and are copied through
//! assignment. You may use HAsciiString objects to get handles to strings.
class TCollection_AsciiString
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initializes a AsciiString to an empty AsciiString.
  Standard_EXPORT TCollection_AsciiString();

  //! Initializes a AsciiString with a string_view.
  //! @param[in] theStringView the string view to initialize from
  Standard_EXPORT TCollection_AsciiString(const std::string_view& theStringView);

  //! Initializes a AsciiString with a CString.
  //! @param[in] theMessage the C string to initialize from
  //! @param[in] theLength the length of the string
  Standard_EXPORT TCollection_AsciiString(const Standard_CString theMessage,
                                          const Standard_Integer theLength);

  //! Initializes a AsciiString with a single character.
  //! @param[in] theChar the character to initialize from
  Standard_EXPORT TCollection_AsciiString(const Standard_Character theChar);

  //! Initializes an AsciiString with specified length space allocated
  //! and filled with filler character. This is useful for buffers.
  //! @param[in] theLength the length to allocate
  //! @param[in] theFiller the character to fill with
  Standard_EXPORT TCollection_AsciiString(const Standard_Integer   theLength,
                                          const Standard_Character theFiller);

  //! Initializes an AsciiString with an integer value
  //! @param[in] theValue the integer value to convert to string
  Standard_EXPORT TCollection_AsciiString(const Standard_Integer theValue);

  //! Initializes an AsciiString with a real value
  //! @param[in] theValue the real value to convert to string
  Standard_EXPORT TCollection_AsciiString(const Standard_Real theValue);

  //! Initializes a AsciiString with another AsciiString.
  //! @param[in] theString the string to copy from
  Standard_EXPORT TCollection_AsciiString(const TCollection_AsciiString& theString);

  //! Move constructor
  //! @param[in] theOther the string to move from
  Standard_EXPORT TCollection_AsciiString(TCollection_AsciiString&& theOther) Standard_Noexcept;

  //! Initializes a AsciiString with copy of another AsciiString
  //! concatenated with the message character.
  //! @param[in] theString the string to copy
  //! @param[in] theChar the character to append
  Standard_EXPORT TCollection_AsciiString(const TCollection_AsciiString& theString,
                                          const Standard_Character       theChar);

  //! Initializes a AsciiString with copy of another AsciiString
  //! concatenated with the message string.
  //! @param[in] theString the string to copy
  //! @param[in] theMessage the C string to append
  Standard_EXPORT TCollection_AsciiString(const TCollection_AsciiString& theString,
                                          const Standard_CString         theMessage);

  //! Initializes a AsciiString with copy of another AsciiString
  //! concatenated with the message string.
  //! @param[in] theString the string to copy
  //! @param[in] theOtherString the string to append
  Standard_EXPORT TCollection_AsciiString(const TCollection_AsciiString& theString,
                                          const TCollection_AsciiString& theOtherString);

  //! Creation by converting an extended string to an ascii string.
  //! If replaceNonAscii is non-null character, it will be used
  //! in place of any non-ascii character found in the source string.
  //! Otherwise, creates UTF-8 unicode string.
  //! @param[in] theExtendedString the extended string to convert
  //! @param[in] theReplaceNonAscii replacement character for non-ASCII characters
  explicit Standard_EXPORT TCollection_AsciiString(const TCollection_ExtendedString& theExtendedString,
                                                   const Standard_Character          theReplaceNonAscii = 0);

#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
  //! Initialize UTF-8 Unicode string from wide-char string considering it as Unicode string
  //! (the size of wide char is a platform-dependent - e.g. on Windows wchar_t is UTF-16).
  //!
  //! This constructor is unavailable if application is built with deprecated msvc option
  //! "-Zc:wchar_t-", since OCCT itself is never built with this option.
  //! @param[in] theStringUtf the wide character string to convert
  Standard_EXPORT TCollection_AsciiString(const Standard_WideChar* theStringUtf);
#endif

  //! Template constructor for string literals with compile-time size deduction.
  //! This optimization avoids runtime strlen() calls for string literals.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Hello");  // Size known at compile time
  //! ```
  //! @param[in] theLiteral the string literal
  template <std::size_t N>
  TCollection_AsciiString(const char (&theLiteral)[N])
    : TCollection_AsciiString(std::string_view(theLiteral, N - 1)) // Exclude null terminator
  {
  }

  //! Appends other character to this string. This is an unary operator.
  //! @param[in] theOther the character to append
  Standard_EXPORT void AssignCat(const Standard_Character theOther);

  void operator+=(const Standard_Character theOther) { AssignCat(theOther); }

  //! Appends other integer to this string. This is an unary operator.
  //! @param[in] theOther the integer to append
  Standard_EXPORT void AssignCat(const Standard_Integer theOther);

  void operator+=(const Standard_Integer theOther) { AssignCat(theOther); }

  //! Appends other real number to this string. This is an unary operator.
  //! @param[in] theOther the real number to append
  Standard_EXPORT void AssignCat(const Standard_Real theOther);

  void operator+=(const Standard_Real theOther) { AssignCat(theOther); }


  //! Appends other string to this string. This is an unary operator.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Hello");
  //! TCollection_AsciiString anotherString(" World");
  //! aString += anotherString;
  //! // Result: aString == "Hello World"
  //! ```
  //! @param[in] theOther the string to append
  Standard_EXPORT void AssignCat(const TCollection_AsciiString& theOther);

  void operator+=(const TCollection_AsciiString& theOther) { AssignCat(theOther); }

  //! Appends string view to this ASCII string. This is an unary operator.
  //! @param[in] theStringView the string view to append
  Standard_EXPORT void AssignCat(const std::string_view& theStringView);

  void operator+=(const std::string_view& theStringView) { AssignCat(theStringView); }

  //! Template method for appending string literals with compile-time size deduction.
  //! This optimization avoids runtime strlen() calls and unnecessary conversions.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Hello");
  //! aString += " World";  // Size known at compile time, no strlen() call
  //! ```
  //! @param[in] theLiteral the string literal to append
  template <std::size_t N>
  void AssignCat(const char (&theLiteral)[N])
  {
    AssignCat(std::string_view(theLiteral, N - 1)); // Exclude null terminator
  }

  template <std::size_t N>
  void operator+=(const char (&theLiteral)[N])
  {
    AssignCat(theLiteral);
  }

  //! Converts the first character into its corresponding
  //! upper-case character and the other characters into lowercase
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("hellO ");
  //! aString.Capitalize();
  //! // Result: aString == "Hello "
  //! ```
  Standard_EXPORT void Capitalize();

  //! Appends other character to this string.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("I say ");
  //! TCollection_AsciiString aResult = aString + '!';
  //! // Result: aResult == "I say !"
  //!
  //! // To catenate more, you must put a String before.
  //! // "Hello " + "Dolly" // THIS IS NOT ALLOWED
  //! // This rule is applicable to AssignCat (operator +=) too.
  //! ```
  //! @param[in] theOther the character to append
  //! @return new string with character appended
  TCollection_AsciiString Cat(const Standard_Character theOther) const;

  TCollection_AsciiString operator+(const Standard_Character theOther) const
  {
    return Cat(theOther);
  }

  //! Appends other integer to this string.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("I say ");
  //! TCollection_AsciiString aResult = aString + 15;
  //! // Result: aResult == "I say 15"
  //! ```
  //! @param[in] theOther the integer to append
  //! @return new string with integer appended
  TCollection_AsciiString Cat(const Standard_Integer theOther) const;

  TCollection_AsciiString operator+(const Standard_Integer theOther) const { return Cat(theOther); }

  //! Appends other real number to this string.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("I say ");
  //! TCollection_AsciiString aResult = aString + 15.15;
  //! // Result: aResult == "I say 15.15"
  //! ```
  //! @param[in] theOther the real number to append
  //! @return new string with real number appended
  TCollection_AsciiString Cat(const Standard_Real theOther) const;

  TCollection_AsciiString operator+(const Standard_Real theOther) const { return Cat(theOther); }


  //! Appends other string to this string.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Hello");
  //! TCollection_AsciiString anotherString(" World");
  //! TCollection_AsciiString aResult = aString + anotherString;
  //! // Result: aResult == "Hello World"
  //! ```
  //! @param[in] theOther the string to append
  //! @return new string with other string appended
  TCollection_AsciiString Cat(const TCollection_AsciiString& theOther) const;

  TCollection_AsciiString operator+(const TCollection_AsciiString& theOther) const
  {
    return Cat(theOther);
  }

  //! Appends string view to this ASCII string.
  //! @param[in] theStringView the string view to append
  //! @return new string with string view appended
  TCollection_AsciiString Cat(const std::string_view& theStringView) const;

  TCollection_AsciiString operator+(const std::string_view& theStringView) const
  {
    return Cat(theStringView);
  }

  //! Template method for concatenating string literals with compile-time size deduction.
  //! This optimization avoids runtime strlen() calls and unnecessary conversions.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Hello");
  //! TCollection_AsciiString aResult = aString + " World";  // Size known at compile time
  //! // Result: aResult == "Hello World"
  //! ```
  //! @param[in] theLiteral the string literal to concatenate
  //! @return new string with literal appended
  template <std::size_t N>
  TCollection_AsciiString Cat(const char (&theLiteral)[N]) const
  {
    return Cat(std::string_view(theLiteral, N - 1)); // Exclude null terminator
  }

  template <std::size_t N>
  TCollection_AsciiString operator+(const char (&theLiteral)[N]) const
  {
    return Cat(theLiteral);
  }

  //! Modifies this ASCII string so that its length
  //! becomes equal to Width and the new characters
  //! are equal to Filler. New characters are added
  //! both at the beginning and at the end of this string.
  //! If Width is less than the length of this ASCII string, nothing happens.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString anAlphabet("abcdef");
  //! anAlphabet.Center(9, ' ');
  //! // Result: anAlphabet == " abcdef "
  //! ```
  //! @param[in] theWidth the desired width
  //! @param[in] theFiller the character to fill with
  Standard_EXPORT void Center(const Standard_Integer theWidth, const Standard_Character theFiller);

  //! Substitutes all the characters equal to aChar by NewChar
  //! in this AsciiString.
  //! The substitution can be case sensitive.
  //! If you don't use default case sensitive, no matter whether aChar
  //! is uppercase or not.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Histake");
  //! aString.ChangeAll('H', 'M', Standard_True);
  //! // Result: aString == "Mistake"
  //! ```
  //! @param[in] theChar the character to replace
  //! @param[in] theNewChar the replacement character
  //! @param[in] theCaseSensitive flag indicating case sensitivity
  Standard_EXPORT void ChangeAll(const Standard_Character theChar,
                                 const Standard_Character theNewChar,
                                 const Standard_Boolean   theCaseSensitive = Standard_True);

  //! Removes all characters contained in this string.
  //! This produces an empty AsciiString.
  Standard_EXPORT void Clear();


  //! Copy string view to this ASCII string.
  //! Used as operator =
  //! @param[in] theStringView the string view to copy from
  Standard_EXPORT void Copy(const std::string_view& theStringView);

  void operator=(const std::string_view& theStringView) { Copy(theStringView); }

  //! Template method for copying string literals with compile-time size deduction.
  //! This optimization avoids runtime strlen() calls and unnecessary conversions.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString;
  //! aString = "Hello World";  // Size known at compile time
  //! // Result: aString == "Hello World"
  //! ```
  //! @param[in] theLiteral the string literal to copy from
  template <std::size_t N>
  void Copy(const char (&theLiteral)[N])
  {
    Copy(std::string_view(theLiteral, N - 1)); // Exclude null terminator
  }

  template <std::size_t N>
  void operator=(const char (&theLiteral)[N])
  {
    Copy(theLiteral);
  }

  //! Copy fromwhere to this string.
  //! Used as operator =
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString;
  //! TCollection_AsciiString anotherString("Hello World");
  //! aString = anotherString;  // operator=
  //! // Result: aString == "Hello World"
  //! ```
  Standard_EXPORT void Copy(const TCollection_AsciiString& theFromWhere);

  //! Copy assignment operator
  TCollection_AsciiString& operator=(const TCollection_AsciiString& theOther)
  {
    Copy(theOther);
    return *this;
  }

  //! Moves string without reallocations
  //! @param[in] theOther the string to move from
  Standard_EXPORT void Move(TCollection_AsciiString&& theOther);

  //! Move assignment operator
  TCollection_AsciiString& operator=(TCollection_AsciiString&& theOther) noexcept
  {
    Move(std::forward<TCollection_AsciiString>(theOther));
    return *this;
  }

  //! Exchange the data of two strings (without reallocating memory).
  //! @param[in,out] theOther the string to exchange data with
  Standard_EXPORT void Swap(TCollection_AsciiString& theOther);

  //! Frees memory allocated by AsciiString.
  Standard_EXPORT ~TCollection_AsciiString();

  //! Returns the index of the first character of this string that is
  //! present in Set.
  //! The search begins to the index FromIndex and ends to the
  //! the index ToIndex.
  //! Returns zero if failure.
  //! Raises an exception if FromIndex or ToIndex is out of range.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("aabAcAa");
  //! TCollection_AsciiString aSet("Aa");
  //! Standard_Integer anIndex = aString.FirstLocationInSet(aSet, 1, 7);
  //! // Result: anIndex == 1
  //! ```
  //! @param[in] theSet the set of characters to search for
  //! @param[in] theFromIndex the starting index for search
  //! @param[in] theToIndex the ending index for search
  //! @return the index of first character found in set, or 0 if not found
  Standard_EXPORT Standard_Integer FirstLocationInSet(const TCollection_AsciiString& theSet,
                                                      const Standard_Integer         theFromIndex,
                                                      const Standard_Integer theToIndex) const;

  //! Returns the index of the first character of this string
  //! that is not present in the set Set.
  //! The search begins to the index FromIndex and ends to the
  //! the index ToIndex in this string.
  //! Returns zero if failure.
  //! Raises an exception if FromIndex or ToIndex is out of range.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("aabAcAa");
  //! TCollection_AsciiString aSet("Aa");
  //! Standard_Integer anIndex = aString.FirstLocationNotInSet(aSet, 1, 7);
  //! // Result: anIndex == 3
  //! ```
  //! @param[in] theSet the set of characters to check against
  //! @param[in] theFromIndex the starting index for search
  //! @param[in] theToIndex the ending index for search
  //! @return the index of first character not in set, or 0 if not found
  Standard_EXPORT Standard_Integer FirstLocationNotInSet(const TCollection_AsciiString& theSet,
                                                         const Standard_Integer theFromIndex,
                                                         const Standard_Integer theToIndex) const;

  //! Inserts a Character at position where.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("hy not ?");
  //! aString.Insert(1, 'W');
  //! // Result: aString == "Why not ?"
  //!
  //! TCollection_AsciiString bString("Wh");
  //! bString.Insert(3, 'y');
  //! // Result: bString == "Why"
  //! ```
  //! @param[in] theWhere the position to insert at
  //! @param[in] theWhat the character to insert
  Standard_EXPORT void Insert(const Standard_Integer theWhere, const Standard_Character theWhat);


  //! Inserts a AsciiString at position where.
  //! @param[in] theWhere the position to insert at
  //! @param[in] theWhat the ASCII string to insert
  Standard_EXPORT void Insert(const Standard_Integer         theWhere,
                              const TCollection_AsciiString& theWhat);

  //! Inserts a string_view at position theWhere.
  //! @param[in] theWhere position to insert at
  //! @param[in] theStringView the string view to insert
  Standard_EXPORT void Insert(const Standard_Integer  theWhere,
                              const std::string_view& theStringView);

  //! Template method for inserting string literals with compile-time size deduction.
  //! This optimization avoids runtime strlen() calls and unnecessary conversions.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("O more");
  //! aString.Insert(2, "nce");  // Size known at compile time
  //! // Result: aString == "Once more"
  //! ```
  //! @param[in] theWhere the position to insert at
  //! @param[in] theLiteral the string literal to insert
  template <std::size_t N>
  void Insert(const Standard_Integer theWhere, const char (&theLiteral)[N])
  {
    Insert(theWhere, std::string_view(theLiteral, N - 1)); // Exclude null terminator
  }

  //! Pushing a string after a specific index in this string.
  //! Raises an exception if Index is out of bounds.
  //! -   less than 0 (InsertAfter), or less than 1 (InsertBefore), or
  //! -   greater than the number of characters in this ASCII string.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("cde");
  //! TCollection_AsciiString anOther("ab");
  //! aString.InsertAfter(0, anOther);
  //! // Result: aString == "abcde"
  //! ```
  //! @param[in] theIndex the index to insert after
  //! @param[in] theOther the string to insert
  Standard_EXPORT void InsertAfter(const Standard_Integer         theIndex,
                                   const TCollection_AsciiString& theOther);

  //! Pushing a string before a specific index in this string.
  //! Raises an exception if Index is out of bounds.
  //! -   less than 0 (InsertAfter), or less than 1 (InsertBefore), or
  //! -   greater than the number of characters in this ASCII string.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("cde");
  //! TCollection_AsciiString anOther("ab");
  //! aString.InsertBefore(1, anOther);
  //! // Result: aString == "abcde"
  //! ```
  //! @param[in] theIndex the index to insert before
  //! @param[in] theOther the string to insert
  Standard_EXPORT void InsertBefore(const Standard_Integer         theIndex,
                                    const TCollection_AsciiString& theOther);

  //! Returns True if this string contains zero character.
  Standard_Boolean IsEmpty() const { return myLength == 0; }


  //! Returns true if the characters in this ASCII string
  //! are identical to the characters in ASCII string other.
  //! Note that this method is an alias of operator ==.
  //! @param[in] theOther the ASCII string to compare with
  //! @return true if strings are equal, false otherwise
  Standard_EXPORT Standard_Boolean IsEqual(const TCollection_AsciiString& theOther) const;

  Standard_Boolean operator==(const TCollection_AsciiString& theOther) const
  {
    return IsEqual(theOther);
  }

  //! Returns true if the characters in this ASCII string
  //! are identical to the characters in string_view.
  //! @param[in] theStringView the string view to compare with
  //! @return true if strings are equal, false otherwise
  Standard_EXPORT Standard_Boolean IsEqual(const std::string_view& theStringView) const;


  Standard_Boolean operator==(const std::string_view& theStringView) const
  {
    return IsEqual(theStringView);
  }

  //! Template method for comparing with string literals with compile-time optimization.
  //! This optimization avoids runtime strlen() calls and unnecessary conversions.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Hello");
  //! bool isEqual = aString.IsEqual("Hello");  // Size known at compile time
  //! bool isEqual2 = (aString == "Hello");     // Same optimization
  //! ```
  //! @param[in] theLiteral the string literal to compare with
  //! @return true if strings are equal, false otherwise
  template <std::size_t N>
  Standard_Boolean IsEqual(const char (&theLiteral)[N]) const
  {
    return IsEqual(std::string_view(theLiteral, N - 1)); // Exclude null terminator
  }

  template <std::size_t N>
  Standard_Boolean operator==(const char (&theLiteral)[N]) const
  {
    return IsEqual(theLiteral);
  }


  //! Returns true if there are differences between the
  //! characters in this ASCII string and ASCII string other.
  //! Note that this method is an alias of operator !=
  //! @param[in] theOther the ASCII string to compare with
  //! @return true if strings are different, false otherwise
  Standard_EXPORT Standard_Boolean IsDifferent(const TCollection_AsciiString& theOther) const;

  Standard_Boolean operator!=(const TCollection_AsciiString& theOther) const
  {
    return IsDifferent(theOther);
  }

  //! Returns true if there are differences between the
  //! characters in this ASCII string and string_view.
  //! @param[in] theStringView the string view to compare with
  //! @return true if strings are different, false otherwise
  Standard_EXPORT Standard_Boolean IsDifferent(const std::string_view& theStringView) const;

  Standard_Boolean operator!=(const std::string_view& theStringView) const
  {
    return IsDifferent(theStringView);
  }

  //! Template method for comparing difference with string literals with compile-time optimization.
  //! @param[in] theLiteral the string literal to compare with
  //! @return true if strings are different, false otherwise
  template <std::size_t N>
  Standard_Boolean IsDifferent(const char (&theLiteral)[N]) const
  {
    return IsDifferent(std::string_view(theLiteral, N - 1)); // Exclude null terminator
  }

  template <std::size_t N>
  Standard_Boolean operator!=(const char (&theLiteral)[N]) const
  {
    return IsDifferent(theLiteral);
  }


  //! Returns TRUE if this string is 'ASCII' less than other.
  //! @param[in] theOther the ASCII string to compare with
  //! @return true if this string is lexicographically less than other
  Standard_EXPORT Standard_Boolean IsLess(const TCollection_AsciiString& theOther) const;

  Standard_Boolean operator<(const TCollection_AsciiString& theOther) const
  {
    return IsLess(theOther);
  }

  //! Returns TRUE if this ASCII string is lexicographically less than theStringView.
  //! @param[in] theStringView the string view to compare with
  //! @return true if this string is lexicographically less than theStringView
  Standard_EXPORT Standard_Boolean IsLess(const std::string_view& theStringView) const;

  Standard_Boolean operator<(const std::string_view& theStringView) const
  {
    return IsLess(theStringView);
  }

  //! Template method for lexicographic comparison with string literals with compile-time optimization.
  //! @param[in] theLiteral the string literal to compare with
  //! @return true if this string is lexicographically less than literal
  template <std::size_t N>
  Standard_Boolean IsLess(const char (&theLiteral)[N]) const
  {
    return IsLess(std::string_view(theLiteral, N - 1)); // Exclude null terminator
  }

  template <std::size_t N>
  Standard_Boolean operator<(const char (&theLiteral)[N]) const
  {
    return IsLess(theLiteral);
  }


  //! Returns TRUE if this string is 'ASCII' greater than other.
  //! @param[in] theOther the ASCII string to compare with
  //! @return true if this string is lexicographically greater than other
  Standard_EXPORT Standard_Boolean IsGreater(const TCollection_AsciiString& theOther) const;

  Standard_Boolean operator>(const TCollection_AsciiString& theOther) const
  {
    return IsGreater(theOther);
  }

  //! Returns TRUE if this ASCII string is lexicographically greater than theStringView.
  //! @param[in] theStringView the string view to compare with
  //! @return true if this string is lexicographically greater than theStringView
  Standard_EXPORT Standard_Boolean IsGreater(const std::string_view& theStringView) const;

  Standard_Boolean operator>(const std::string_view& theStringView) const
  {
    return IsGreater(theStringView);
  }

  //! Template method for lexicographic greater comparison with string literals with compile-time optimization.
  //! @param[in] theLiteral the string literal to compare with
  //! @return true if this string is lexicographically greater than literal
  template <std::size_t N>
  Standard_Boolean IsGreater(const char (&theLiteral)[N]) const
  {
    return IsGreater(std::string_view(theLiteral, N - 1)); // Exclude null terminator
  }

  template <std::size_t N>
  Standard_Boolean operator>(const char (&theLiteral)[N]) const
  {
    return IsGreater(theLiteral);
  }

  //! Determines whether the beginning of this string instance matches the specified string.
  //! @param[in] theStartString the string to check for at the beginning
  //! @return true if this string starts with theStartString
  Standard_EXPORT Standard_Boolean StartsWith(const TCollection_AsciiString& theStartString) const;


  //! Determines whether the beginning of this string instance matches the specified string_view.
  //! @param[in] theStartString the string view to check for at the beginning
  //! @return true if this string starts with theStartString
  Standard_EXPORT Standard_Boolean StartsWith(const std::string_view& theStartString) const;

  //! Determines whether the end of this string instance matches the specified string.
  //! @param[in] theEndString the string to check for at the end
  //! @return true if this string ends with theEndString
  Standard_EXPORT Standard_Boolean EndsWith(const TCollection_AsciiString& theEndString) const;


  //! Determines whether the end of this string instance matches the specified string_view.
  //! @param[in] theEndString the string view to check for at the end
  //! @return true if this string ends with theEndString
  Standard_EXPORT Standard_Boolean EndsWith(const std::string_view& theEndString) const;

  //! Template method for checking if string starts with a literal with compile-time optimization.
  //! @param[in] theLiteral the string literal to check for at the beginning
  //! @return true if this string starts with literal
  template <std::size_t N>
  Standard_Boolean StartsWith(const char (&theLiteral)[N]) const
  {
    return StartsWith(std::string_view(theLiteral, N - 1)); // Exclude null terminator
  }

  //! Template method for checking if string ends with a literal with compile-time optimization.
  //! @param[in] theLiteral the string literal to check for at the end
  //! @return true if this string ends with literal
  template <std::size_t N>
  Standard_Boolean EndsWith(const char (&theLiteral)[N]) const
  {
    return EndsWith(std::string_view(theLiteral, N - 1)); // Exclude null terminator
  }

  //! Converts a AsciiString containing a numeric expression to an Integer.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("215");
  //! Standard_Integer anInt = aString.IntegerValue();
  //! // Result: anInt == 215
  //! ```
  //! @return the integer value of the string
  Standard_EXPORT Standard_Integer IntegerValue() const;

  //! Returns True if the AsciiString contains an integer value.
  //! Note: an integer value is considered to be a real value as well.
  //! @return true if string represents an integer value
  Standard_EXPORT Standard_Boolean IsIntegerValue() const;

  //! Returns True if the AsciiString starts with some characters that can be interpreted as integer
  //! or real value.
  //! @param[in] theToCheckFull  when TRUE, checks if entire string defines a real value;
  //!                            otherwise checks if string starts with a real value
  //! Note: an integer value is considered to be a real value as well.
  //! @return true if string represents a real value
  Standard_EXPORT Standard_Boolean
    IsRealValue(Standard_Boolean theToCheckFull = Standard_False) const;

  //! Returns True if the AsciiString contains only ASCII characters
  //! between ' ' and '~'.
  //! This means no control character and no extended ASCII code.
  //! @return true if string contains only ASCII characters
  Standard_EXPORT Standard_Boolean IsAscii() const;

  //! Removes all space characters in the beginning of the string.
  Standard_EXPORT void LeftAdjust();

  //! left justify
  //! Length becomes equal to Width and the new characters are
  //! equal to Filler.
  //! If Width < Length nothing happens.
  //! Raises an exception if Width is less than zero.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("abcdef");
  //! aString.LeftJustify(9, ' ');
  //! // Result: aString == "abcdef   "
  //! ```
  //! @param[in] theWidth the desired width
  //! @param[in] theFiller the character to fill with
  Standard_EXPORT void LeftJustify(const Standard_Integer   theWidth,
                                   const Standard_Character theFiller);

  //! Returns number of characters in this string.
  //! This is the same functionality as 'strlen' in C.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString anAlphabet("abcdef");
  //! Standard_Integer aLength = anAlphabet.Length();
  //! // Result: aLength == 6
  //! ```
  //! -   1 is the position of the first character in this string.
  //! -   The length of this string gives the position of its last character.
  //! -   Positions less than or equal to zero, or
  //! greater than the length of this string are
  //! invalid in functions which identify a character
  //! of this string by its position.
  //! @return the number of characters in the string
  Standard_Integer Length() const;

  //! Returns an index in this string of the first occurrence
  //! of the string S in this string from the starting index
  //! FromIndex to the ending index ToIndex
  //! returns zero if failure
  //! Raises an exception if FromIndex or ToIndex is out of range.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("aabAaAa");
  //! TCollection_AsciiString aSearchString("Aa");
  //! Standard_Integer anIndex = aString.Location(aSearchString, 1, 7);
  //! // Result: anIndex == 4
  //! ```
  //! @param[in] theOther the string to search for
  //! @param[in] theFromIndex the starting index for search
  //! @param[in] theToIndex the ending index for search
  //! @return the index of first occurrence, or 0 if not found
  Standard_EXPORT Standard_Integer Location(const TCollection_AsciiString& theOther,
                                            const Standard_Integer         theFromIndex,
                                            const Standard_Integer         theToIndex) const;

  //! Returns the index of the nth occurrence of the character C
  //! in this string from the starting index FromIndex to the
  //! ending index ToIndex.
  //! Returns zero if failure.
  //! Raises an exception if FromIndex or ToIndex is out of range.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("aabAa");
  //! Standard_Integer anIndex = aString.Location(3, 'a', 1, 5);
  //! // Result: anIndex == 5
  //! ```
  //! @param[in] theN the occurrence number to find
  //! @param[in] theC the character to search for
  //! @param[in] theFromIndex the starting index for search
  //! @param[in] theToIndex the ending index for search
  //! @return the index of the nth occurrence, or 0 if not found
  Standard_EXPORT Standard_Integer Location(const Standard_Integer   theN,
                                            const Standard_Character theC,
                                            const Standard_Integer   theFromIndex,
                                            const Standard_Integer   theToIndex) const;

  //! Converts this string to its lower-case equivalent.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Hello Dolly");
  //! aString.UpperCase();
  //! // Result: aString == "HELLO DOLLY"
  //! aString.LowerCase();
  //! // Result: aString == "hello dolly"
  //! ```
  Standard_EXPORT void LowerCase();

  //! Inserts the string other at the beginning of this ASCII string.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString anAlphabet("cde");
  //! TCollection_AsciiString aBegin("ab");
  //! anAlphabet.Prepend(aBegin);
  //! // Result: anAlphabet == "abcde"
  //! ```
  //! @param[in] theOther the string to prepend
  Standard_EXPORT void Prepend(const TCollection_AsciiString& theOther);

  //! Displays this string on a stream.
  //! @param[in] theStream the output stream
  Standard_EXPORT void                     Print(Standard_OStream& theStream) const;
  friend Standard_EXPORT Standard_OStream& operator<<(Standard_OStream&              theStream,
                                                      const TCollection_AsciiString& theString);

  //! Read this string from a stream.
  //! @param[in] theStream the input stream
  Standard_EXPORT void                     Read(Standard_IStream& theStream);
  friend Standard_EXPORT Standard_IStream& operator>>(Standard_IStream&        theStream,
                                                      TCollection_AsciiString& theString);

  //! Converts an AsciiString containing a numeric expression to a Real.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString1("215");
  //! Standard_Real aReal1 = aString1.RealValue();
  //! // Result: aReal1 == 215.0
  //!
  //! TCollection_AsciiString aString2("3.14159267");
  //! Standard_Real aReal2 = aString2.RealValue();
  //! // Result: aReal2 == 3.14159267
  //! ```
  //! @return the real value of the string
  Standard_EXPORT Standard_Real RealValue() const;

  //! Remove all the occurrences of the character C in the string.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("HellLLo");
  //! aString.RemoveAll('L', Standard_True);
  //! // Result: aString == "Hello"
  //! ```
  //! @param[in] theC the character to remove
  //! @param[in] theCaseSensitive flag indicating case sensitivity
  Standard_EXPORT void RemoveAll(const Standard_Character theC,
                                 const Standard_Boolean   theCaseSensitive);

  //! Removes every what characters from this string.
  //! @param[in] theWhat the character to remove
  Standard_EXPORT void RemoveAll(const Standard_Character theWhat);

  //! Erases ahowmany characters from position where,
  //! where included.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Hello");
  //! aString.Remove(2, 2); // erases 2 characters from position 2
  //! // Result: aString == "Hlo"
  //! ```
  //! @param[in] theWhere the position to start erasing from
  //! @param[in] theHowMany the number of characters to erase
  Standard_EXPORT void Remove(const Standard_Integer theWhere,
                              const Standard_Integer theHowMany = 1);

  //! Removes all space characters at the end of the string.
  Standard_EXPORT void RightAdjust();

  //! Right justify.
  //! Length becomes equal to Width and the new characters are
  //! equal to Filler.
  //! if Width < Length nothing happens.
  //! Raises an exception if Width is less than zero.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("abcdef");
  //! aString.RightJustify(9, ' ');
  //! // Result: aString == "   abcdef"
  //! ```
  //! @param[in] theWidth the desired width
  //! @param[in] theFiller the character to fill with
  Standard_EXPORT void RightJustify(const Standard_Integer   theWidth,
                                    const Standard_Character theFiller);


  //! Searches an AsciiString in this string from the beginning
  //! and returns position of first item what matching.
  //! It returns -1 if not found.
  //! @param[in] theWhat the ASCII string to search for
  //! @return the position of first match, or -1 if not found
  Standard_EXPORT Standard_Integer Search(const TCollection_AsciiString& theWhat) const;


  //! Searches a AsciiString in another AsciiString from the end
  //! and returns position of first item what matching.
  //! It returns -1 if not found.
  //! @param[in] theWhat the ASCII string to search for
  //! @return the position of first match from end, or -1 if not found
  Standard_EXPORT Standard_Integer SearchFromEnd(const TCollection_AsciiString& theWhat) const;

  //! Replaces one character in the AsciiString at position where.
  //! If where is less than zero or greater than the length of this string
  //! an exception is raised.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Garbake");
  //! aString.SetValue(6, 'g');
  //! // Result: aString == "Garbage"
  //! ```
  //! @param[in] theWhere the position to replace at
  //! @param[in] theWhat the character to replace with
  Standard_EXPORT void SetValue(const Standard_Integer theWhere, const Standard_Character theWhat);


  //! Replaces a part of this string by another AsciiString.
  //! @param[in] theWhere the position to start replacement
  //! @param[in] theWhat the ASCII string to replace with
  Standard_EXPORT void SetValue(const Standard_Integer         theWhere,
                                const TCollection_AsciiString& theWhat);

  //! Replaces a part of this ASCII string with a string_view.
  //! @param[in] theWhere position to start replacement
  //! @param[in] theStringView the string view to replace with
  Standard_EXPORT void SetValue(const Standard_Integer  theWhere,
                                const std::string_view& theStringView);


  //! Splits a AsciiString into two sub-strings.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("abcdefg");
  //! TCollection_AsciiString aSecondPart = aString.Split(3);
  //! // Result: aString == "abc" and aSecondPart == "defg"
  //! ```
  //! @param[in] theWhere the position to split at
  //! @return the second part of the split string
  Standard_EXPORT TCollection_AsciiString Split(const Standard_Integer theWhere);

  //! Creation of a sub-string of this string.
  //! The sub-string starts to the index Fromindex and ends
  //! to the index ToIndex.
  //! Raises an exception if ToIndex or FromIndex is out of bounds
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("abcdefg");
  //! TCollection_AsciiString aSubString = aString.SubString(3, 6);
  //! // Result: aSubString == "cdef"
  //! ```
  //! @param[in] theFromIndex the starting index
  //! @param[in] theToIndex the ending index
  //! @return the substring from FromIndex to ToIndex
  TCollection_AsciiString SubString(const Standard_Integer theFromIndex,
                                    const Standard_Integer theToIndex) const;

  //! Returns pointer to AsciiString (char *).
  //! This is useful for some casual manipulations.
  //! Warning: Because this "char *" is 'const', you can't modify its contents.
  //! @return the C string representation
  Standard_CString ToCString() const;

  //! Returns string_view for this AsciiString.
  //! This provides a lightweight, non-owning view of the string data.
  //! @return the string_view representation
  explicit operator std::string_view() const;

  //! Extracts whichone token from this string.
  //! By default, the separators is set to space and tabulation.
  //! By default, the token extracted is the first one (whichone = 1).
  //! separators contains all separators you need.
  //! If no token indexed by whichone is found, it returns empty AsciiString.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("This is a     message");
  //! TCollection_AsciiString aToken1 = aString.Token();
  //! // Result: aToken1 == "This"
  //!
  //! TCollection_AsciiString aToken2 = aString.Token(" ", 4);
  //! // Result: aToken2 == "message"
  //!
  //! TCollection_AsciiString aToken3 = aString.Token(" ", 2);
  //! // Result: aToken3 == "is"
  //!
  //! TCollection_AsciiString aToken4 = aString.Token(" ", 9);
  //! // Result: aToken4 == ""
  //!
  //! TCollection_AsciiString bString("1234; test:message   , value");
  //! TCollection_AsciiString bToken1 = bString.Token("; :,", 4);
  //! // Result: bToken1 == "value"
  //!
  //! TCollection_AsciiString bToken2 = bString.Token("; :,", 2);
  //! // Result: bToken2 == "test"
  //! ```
  //! @param[in] theSeparators the separator characters
  //! @param[in] theWhichOne the token number to extract
  Standard_EXPORT TCollection_AsciiString Token(const Standard_CString theSeparators = " \t",
                                                const Standard_Integer theWhichOne   = 1) const;

  //! Truncates this string to ahowmany characters.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Hello Dolly");
  //! aString.Trunc(3);
  //! // Result: aString == "Hel"
  //! ```
  //! @param[in] theHowMany the number of characters to keep
  Standard_EXPORT void Trunc(const Standard_Integer theHowMany);

  //! Converts this string to its upper-case equivalent.
  Standard_EXPORT void UpperCase();

  //! Length of the string ignoring all spaces (' ') and the
  //! control character at the end.
  //! @return the useful length of the string
  Standard_EXPORT Standard_Integer UsefullLength() const;

  //! Returns character at position where in this string.
  //! If where is less than zero or greater than the length of this string,
  //! an exception is raised.
  //!
  //! Example:
  //! ```cpp
  //! TCollection_AsciiString aString("Hello");
  //! Standard_Character aChar = aString.Value(2);
  //! // Result: aChar == 'e'
  //! ```
  //! @param[in] theWhere the position to get character from
  //! @return the character at the specified position
  Standard_EXPORT Standard_Character Value(const Standard_Integer theWhere) const;

  //! Computes a hash code for the given ASCII string
  //! Returns the same integer value as the hash function for TCollection_ExtendedString
  //! @return a computed hash code
  size_t HashCode() const;

  //! Returns True  when the two  strings are the same.
  //! (Just for HashCode for AsciiString)
  //! @param[in] string1 first string to compare
  //! @param[in] string2 second string to compare
  //! @return true if strings are equal
  static Standard_Boolean IsEqual(const TCollection_AsciiString& string1,
                                  const TCollection_AsciiString& string2);

  //! Returns True  when the two  strings are the same.
  //! (Just for HashCode for AsciiString)
  //! @param[in] string1 first string to compare
  //! @param[in] string2 second C string to compare
  //! @return true if strings are equal
  static Standard_Boolean IsEqual(const TCollection_AsciiString& string1,
                                  const Standard_CString         string2);

  //! Returns True when the ASCII string and string_view are the same.
  //! (Just for HashCode for AsciiString)
  //! @param[in] theString1 first string to compare
  //! @param[in] theStringView second string view to compare
  //! @return true if strings are equal
  static Standard_Boolean IsEqual(const TCollection_AsciiString& theString1,
                                  const std::string_view&        theStringView);

  //! Returns True when the string_view and ASCII string are the same.
  //! (Just for HashCode for AsciiString)
  //! @param[in] theStringView first string view to compare
  //! @param[in] theString2 second string to compare
  //! @return true if strings are equal
  static Standard_Boolean IsEqual(const std::string_view&        theStringView,
                                  const TCollection_AsciiString& theString2);

  //! Returns True if the strings contain same characters.
  //! @param[in] theString1 first string to compare
  //! @param[in] theString2 second string to compare
  //! @param[in] theIsCaseSensitive flag indicating case sensitivity
  //! @return true if strings contain same characters
  Standard_EXPORT static Standard_Boolean IsSameString(const TCollection_AsciiString& theString1,
                                                       const TCollection_AsciiString& theString2,
                                                       const Standard_Boolean theIsCaseSensitive);

  //! Returns True if the string and C string contain same characters.
  //! @param[in] theString1 first string to compare
  //! @param[in] theCString second C string to compare
  //! @param[in] theIsCaseSensitive flag indicating case sensitivity
  //! @return true if strings contain same characters
  Standard_EXPORT static Standard_Boolean IsSameString(const TCollection_AsciiString& theString1,
                                                       const Standard_CString         theCString,
                                                       const Standard_Boolean theIsCaseSensitive);

  //! Returns True if the C string and string contain same characters.
  //! @param[in] theCString first C string to compare
  //! @param[in] theString2 second string to compare
  //! @param[in] theIsCaseSensitive flag indicating case sensitivity
  //! @return true if strings contain same characters
  Standard_EXPORT static Standard_Boolean IsSameString(const Standard_CString         theCString,
                                                       const TCollection_AsciiString& theString2,
                                                       const Standard_Boolean theIsCaseSensitive);

  //! Returns True if the string and string_view contain same characters.
  //! @param[in] theString1 first string to compare
  //! @param[in] theStringView second string view to compare
  //! @param[in] theIsCaseSensitive flag indicating case sensitivity
  //! @return true if strings contain same characters
  Standard_EXPORT static Standard_Boolean IsSameString(const TCollection_AsciiString& theString1,
                                                       const std::string_view&        theStringView,
                                                       const Standard_Boolean theIsCaseSensitive);

  //! Returns True if the string_view and string contain same characters.
  //! @param[in] theStringView first string view to compare
  //! @param[in] theString2 second string to compare
  //! @param[in] theIsCaseSensitive flag indicating case sensitivity
  //! @return true if strings contain same characters
  Standard_EXPORT static Standard_Boolean IsSameString(const std::string_view&        theStringView,
                                                       const TCollection_AsciiString& theString2,
                                                       const Standard_Boolean theIsCaseSensitive);

  //! Returns True if the two C strings contain same characters.
  //! @param[in] theCString1 first C string to compare
  //! @param[in] theCString2 second C string to compare
  //! @param[in] theIsCaseSensitive flag indicating case sensitivity
  //! @return true if strings contain same characters
  Standard_EXPORT static Standard_Boolean IsSameString(const Standard_CString theCString1,
                                                       const Standard_CString theCString2,
                                                       const Standard_Boolean theIsCaseSensitive);

  //! Returns True if the two string_views contain same characters.
  //! @param[in] theStringView1 first string view to compare
  //! @param[in] theStringView2 second string view to compare
  //! @param[in] theIsCaseSensitive flag indicating case sensitivity
  //! @return true if strings contain same characters
  Standard_EXPORT static Standard_Boolean IsSameString(const std::string_view& theStringView1,
                                                       const std::string_view& theStringView2,
                                                       const Standard_Boolean  theIsCaseSensitive);

  friend class TCollection_HAsciiString;

private:
  //! Internal wrapper to allocate on stack or heap
  void allocate(const int theLength);

  //! Internal wrapper to reallocate on stack or heap
  void reallocate(const int theLength);

  //! Internal wrapper to deallocate on stack
  void deallocate();

private:
  Standard_PCharacter myString{}; //!< NULL-terminated string
  Standard_Integer    myLength{}; //!< length in bytes (excluding terminating NULL symbol)
};

#include <TCollection_AsciiString.lxx>

#endif // _TCollection_AsciiString_HeaderFile
