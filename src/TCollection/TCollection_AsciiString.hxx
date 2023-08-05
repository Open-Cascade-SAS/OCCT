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
class TCollection_ExtendedString;

//! Class defines a variable-length sequence of 8-bit characters.
//! Despite class name (kept for historical reasons), it is intended to store UTF-8 string, not just ASCII characters.
//! However, multi-byte nature of UTF-8 is not considered by the following methods:
//! - Method ::Length() return the number of bytes, not the number of Unicode symbols.
//! - Methods taking/returning symbol index work with 8-bit code units, not true Unicode symbols,
//!   including ::Remove(), ::SetValue(), ::Value(), ::Search(), ::Trunc() and others.
//! If application needs to process multi-byte Unicode symbols explicitly, NCollection_Utf8Iter class can be used
//! for iterating through Unicode string (UTF-32 code unit will be returned for each position).
//!
//! Class provides editing operations with built-in memory management to make AsciiString objects easier to use than ordinary character arrays.
//! AsciiString objects follow value semantics; in other words, they are the actual strings,
//! not handles to strings, and are copied through assignment.
//! You may use HAsciiString objects to get handles to strings.
class TCollection_AsciiString 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes a AsciiString to an empty AsciiString.
  Standard_EXPORT TCollection_AsciiString();
  
  //! Initializes a AsciiString with a CString.
  Standard_EXPORT TCollection_AsciiString(const Standard_CString message);
  
  //! Initializes a AsciiString with a CString.
  Standard_EXPORT TCollection_AsciiString(const Standard_CString message, const Standard_Integer aLen);
  
  //! Initializes a AsciiString with a single character.
  Standard_EXPORT TCollection_AsciiString(const Standard_Character aChar);
  
  //! Initializes an AsciiString with <length> space allocated.
  //! and filled with <filler>. This is useful for buffers.
  Standard_EXPORT TCollection_AsciiString(const Standard_Integer length, const Standard_Character filler);
  
  //! Initializes an AsciiString with an integer value
  Standard_EXPORT TCollection_AsciiString(const Standard_Integer value);
  
  //! Initializes an AsciiString with a real value
  Standard_EXPORT TCollection_AsciiString(const Standard_Real value);
  
  //! Initializes a AsciiString with another AsciiString.
  Standard_EXPORT TCollection_AsciiString(const TCollection_AsciiString& astring);

  //! Move constructor
  Standard_EXPORT TCollection_AsciiString (TCollection_AsciiString&& theOther) Standard_Noexcept;
  
  //! Initializes a AsciiString with copy of another AsciiString
  //! concatenated with the message character.
  Standard_EXPORT TCollection_AsciiString(const TCollection_AsciiString& astring, const Standard_Character message);
  
  //! Initializes a AsciiString with copy of another AsciiString
  //! concatenated with the message string.
  Standard_EXPORT TCollection_AsciiString(const TCollection_AsciiString& astring, const Standard_CString message);
  
  //! Initializes a AsciiString with copy of another AsciiString
  //! concatenated with the message string.
  Standard_EXPORT TCollection_AsciiString(const TCollection_AsciiString& astring, const TCollection_AsciiString& message);
  
  //! Creation by converting an extended string to an ascii string.
  //! If replaceNonAscii is non-null character, it will be used
  //! in place of any non-ascii character found in the source string.
  //! Otherwise, creates UTF-8 unicode string.
  Standard_EXPORT TCollection_AsciiString(const TCollection_ExtendedString& astring, const Standard_Character replaceNonAscii = 0);

#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
  //! Initialize UTF-8 Unicode string from wide-char string considering it as Unicode string
  //! (the size of wide char is a platform-dependent - e.g. on Windows wchar_t is UTF-16).
  //!
  //! This constructor is unavailable if application is built with deprecated msvc option "-Zc:wchar_t-",
  //! since OCCT itself is never built with this option.
  Standard_EXPORT TCollection_AsciiString (const Standard_WideChar* theStringUtf);
#endif

  //! Appends <other>  to me. This is an unary operator.
  Standard_EXPORT void AssignCat (const Standard_Character other);
void operator += (const Standard_Character other)
{
  AssignCat(other);
}
  
  //! Appends <other>  to me. This is an unary operator.
  Standard_EXPORT void AssignCat (const Standard_Integer other);
void operator += (const Standard_Integer other)
{
  AssignCat(other);
}
  
  //! Appends <other>  to me. This is an unary operator.
  Standard_EXPORT void AssignCat (const Standard_Real other);
void operator += (const Standard_Real other)
{
  AssignCat(other);
}
  
  //! Appends <other>  to me. This is an unary operator.
  //! ex: aString += "Dummy"
  //! To catenate more than one CString, you must put a
  //! AsciiString before.
  //! Example: aString += "Hello " + "Dolly"  IS NOT VALID !
  //! But astring += anotherString + "Hello " + "Dolly" is valid.
  Standard_EXPORT void AssignCat (const Standard_CString other);
void operator += (const Standard_CString other)
{
  AssignCat(other);
}
  
  //! Appends <other> to me. This is an unary operator.
  //! Example: aString += anotherString
  Standard_EXPORT void AssignCat (const TCollection_AsciiString& other);
void operator += (const TCollection_AsciiString& other)
{
  AssignCat(other);
}
  
  //! Converts the first character into its corresponding
  //! upper-case character and the other characters into lowercase
  //! Example: before
  //! me = "hellO "
  //! after
  //! me = "Hello "
  Standard_EXPORT void Capitalize();
  
  //! Appends <other>  to me.
  //! Syntax:
  //! aString = aString + "Dummy"
  //! Example: aString contains "I say "
  //! aString = aString + "Hello " + "Dolly"
  //! gives "I say Hello Dolly"
  //! To catenate more than one CString, you must put a String before.
  //! So the following example is WRONG !
  //! aString = "Hello " + "Dolly"  THIS IS NOT ALLOWED
  //! This rule is applicable to AssignCat (operator +=) too.
    TCollection_AsciiString Cat (const Standard_Character other) const;
  TCollection_AsciiString operator + (const Standard_Character other) const
{
  return Cat(other);
}
  
  //! Appends <other>  to me.
  //! Syntax:
  //! aString = aString + 15;
  //! Example: aString contains "I say "
  //! gives "I say 15"
  //! To catenate more than one CString, you must put a String before.
  //! So the following example is WRONG !
  //! aString = "Hello " + "Dolly"  THIS IS NOT ALLOWED
  //! This rule is applicable to AssignCat (operator +=) too.
    TCollection_AsciiString Cat (const Standard_Integer other) const;
  TCollection_AsciiString operator + (const Standard_Integer other) const
{
  return Cat(other);
}
  
  //! Appends <other>  to me.
  //! Syntax:
  //! aString = aString + 15.15;
  //! Example: aString contains "I say "
  //! gives "I say 15.15"
  //! To catenate more than one CString, you must put a String before.
  //! So the following example is WRONG !
  //! aString = "Hello " + "Dolly"  THIS IS NOT ALLOWED
  //! This rule is applicable to AssignCat (operator +=) too.
    TCollection_AsciiString Cat (const Standard_Real other) const;
  TCollection_AsciiString operator + (const Standard_Real other) const
{
  return Cat(other);
}
  
  //! Appends <other>  to me.
  //! Syntax:
  //! aString = aString + "Dummy"
  //! Example: aString contains "I say "
  //! aString = aString + "Hello " + "Dolly"
  //! gives "I say Hello Dolly"
  //! To catenate more than one CString, you must put a String before.
  //! So the following example is WRONG !
  //! aString = "Hello " + "Dolly"  THIS IS NOT ALLOWED
  //! This rule is applicable to AssignCat (operator +=) too.
    TCollection_AsciiString Cat (const Standard_CString other) const;
  TCollection_AsciiString operator + (const Standard_CString other) const
{
  return Cat(other);
}
  
  //! Appends <other> to me.
  //! Example: aString = aString + anotherString
    TCollection_AsciiString Cat (const TCollection_AsciiString& other) const;
  TCollection_AsciiString operator + (const TCollection_AsciiString& other) const
{
  return Cat(other);
}
  
  //! Modifies this ASCII string so that its length
  //! becomes equal to Width and the new characters
  //! are equal to Filler. New characters are added
  //! both at the beginning and at the end of this string.
  //! If Width is less than the length of this ASCII string, nothing happens.
  //! Example
  //! TCollection_AsciiString
  //! myAlphabet("abcdef");
  //! myAlphabet.Center(9,' ');
  //! assert ( myAlphabet == "
  //! abcdef " );
  Standard_EXPORT void Center (const Standard_Integer Width, const Standard_Character Filler);
  
  //! Substitutes all the characters equal to aChar by NewChar
  //! in the AsciiString <me>.
  //! The substitution can be case sensitive.
  //! If you don't use default case sensitive, no matter whether aChar
  //! is uppercase or not.
  //! Example: me = "Histake" -> ChangeAll('H','M',Standard_True)
  //! gives me = "Mistake"
  Standard_EXPORT void ChangeAll (const Standard_Character aChar, const Standard_Character NewChar, const Standard_Boolean CaseSensitive = Standard_True);
  
  //! Removes all characters contained in <me>.
  //! This produces an empty AsciiString.
  Standard_EXPORT void Clear();
  
  //! Copy <fromwhere> to <me>.
  //! Used as operator =
  //! Example: aString = anotherCString;
  Standard_EXPORT void Copy (const Standard_CString fromwhere);
void operator = (const Standard_CString fromwhere)
{
  Copy(fromwhere);
}
  
  //! Copy <fromwhere> to <me>.
  //! Used as operator =
  //! Example: aString = anotherString;
  Standard_EXPORT void Copy (const TCollection_AsciiString& fromwhere);

  //! Copy assignment operator
  TCollection_AsciiString& operator= (const TCollection_AsciiString& theOther)
  {
    Copy(theOther);
    return *this;
  }

  //! Moves string without reallocations
  Standard_EXPORT void Move (TCollection_AsciiString&& theOther);

  //! Move assignment operator
  TCollection_AsciiString& operator= (TCollection_AsciiString&& theOther) noexcept
  {
    Move(std::forward<TCollection_AsciiString>(theOther));
    return *this;
  }

  //! Exchange the data of two strings (without reallocating memory).
  Standard_EXPORT void Swap (TCollection_AsciiString& theOther);

  //! Frees memory allocated by AsciiString.
  Standard_EXPORT ~TCollection_AsciiString();
  
  //! Returns the index of the first character of <me> that is
  //! present in <Set>.
  //! The search begins to the index FromIndex and ends to the
  //! the index ToIndex.
  //! Returns zero if failure.
  //! Raises an exception if FromIndex or ToIndex is out of range.
  //! Example: before
  //! me = "aabAcAa", S = "Aa", FromIndex = 1, Toindex = 7
  //! after
  //! me = "aabAcAa"
  //! returns
  //! 1
  Standard_EXPORT Standard_Integer FirstLocationInSet (const TCollection_AsciiString& Set, const Standard_Integer FromIndex, const Standard_Integer ToIndex) const;
  
  //! Returns the index of the first character of <me>
  //! that is not present in the set <Set>.
  //! The search begins to the index FromIndex and ends to the
  //! the index ToIndex in <me>.
  //! Returns zero if failure.
  //! Raises an exception if FromIndex or ToIndex is out of range.
  //! Example: before
  //! me = "aabAcAa", S = "Aa", FromIndex = 1, Toindex = 7
  //! after
  //! me = "aabAcAa"
  //! returns
  //! 3
  Standard_EXPORT Standard_Integer FirstLocationNotInSet (const TCollection_AsciiString& Set, const Standard_Integer FromIndex, const Standard_Integer ToIndex) const;
  
  //! Inserts a Character at position <where>.
  //! Example:
  //! aString contains "hy not ?"
  //! aString.Insert(1,'W'); gives "Why not ?"
  //! aString contains "Wh"
  //! aString.Insert(3,'y'); gives "Why"
  //! aString contains "Way"
  //! aString.Insert(2,'h'); gives "Why"
  Standard_EXPORT void Insert (const Standard_Integer where, const Standard_Character what);
  
  //! Inserts a CString at position <where>.
  //! Example:
  //! aString contains "O more"
  //! aString.Insert(2,"nce");  gives "Once more"
  Standard_EXPORT void Insert (const Standard_Integer where, const Standard_CString what);
  
  //! Inserts a AsciiString at position <where>.
  Standard_EXPORT void Insert (const Standard_Integer where, const TCollection_AsciiString& what);
  
  //! Pushing a string after a specific index in the string <me>.
  //! Raises an exception if Index is out of bounds.
  //! -   less than 0 (InsertAfter), or less than 1 (InsertBefore), or
  //! -   greater than the number of characters in this ASCII string.
  //! Example:
  //! before
  //! me = "cde" , Index = 0 , other = "ab"
  //! after
  //! me = "abcde" , other = "ab"
  Standard_EXPORT void InsertAfter (const Standard_Integer Index, const TCollection_AsciiString& other);
  
  //! Pushing a string before a specific index in the string <me>.
  //! Raises an exception if Index is out of bounds.
  //! -   less than 0 (InsertAfter), or less than 1 (InsertBefore), or
  //! -   greater than the number of characters in this ASCII string.
  //! Example:
  //! before
  //! me = "cde" , Index = 1 , other = "ab"
  //! after
  //! me = "abcde" , other = "ab"
  Standard_EXPORT void InsertBefore (const Standard_Integer Index, const TCollection_AsciiString& other);
  
  //! Returns True if the string <me> contains zero character.
  Standard_Boolean IsEmpty() const { return mylength == 0; }

  //! Returns true if the characters in this ASCII string
  //! are identical to the characters in ASCII string other.
  //! Note that this method is an alias of operator ==.
  Standard_EXPORT Standard_Boolean IsEqual (const Standard_CString other) const;
Standard_Boolean operator == (const Standard_CString other) const
{
  return IsEqual(other);
}
  
  //! Returns true if the characters in this ASCII string
  //! are identical to the characters in ASCII string other.
  //! Note that this method is an alias of operator ==.
  Standard_EXPORT Standard_Boolean IsEqual (const TCollection_AsciiString& other) const;
Standard_Boolean operator == (const TCollection_AsciiString& other) const
{
  return IsEqual(other);
}
  
  //! Returns true if there are differences between the
  //! characters in this ASCII string and ASCII string other.
  //! Note that this method is an alias of operator !=
  Standard_EXPORT Standard_Boolean IsDifferent (const Standard_CString other) const;
Standard_Boolean operator != (const Standard_CString other) const
{
  return IsDifferent(other);
}
  
  //! Returns true if there are differences between the
  //! characters in this ASCII string and ASCII string other.
  //! Note that this method is an alias of operator !=
  Standard_EXPORT Standard_Boolean IsDifferent (const TCollection_AsciiString& other) const;
Standard_Boolean operator != (const TCollection_AsciiString& other) const
{
  return IsDifferent(other);
}
  
  //! Returns TRUE if <me> is 'ASCII' less than <other>.
  Standard_EXPORT Standard_Boolean IsLess (const Standard_CString other) const;
Standard_Boolean operator < (const Standard_CString other) const
{
  return IsLess(other);
}
  
  //! Returns TRUE if <me> is 'ASCII' less than <other>.
  Standard_EXPORT Standard_Boolean IsLess (const TCollection_AsciiString& other) const;
Standard_Boolean operator < (const TCollection_AsciiString& other) const
{
  return IsLess(other);
}
  
  //! Returns TRUE if <me> is 'ASCII' greater than <other>.
  Standard_EXPORT Standard_Boolean IsGreater (const Standard_CString other) const;
Standard_Boolean operator > (const Standard_CString other) const
{
  return IsGreater(other);
}
  
  //! Returns TRUE if <me> is 'ASCII' greater than <other>.
  Standard_EXPORT Standard_Boolean IsGreater (const TCollection_AsciiString& other) const;
Standard_Boolean operator > (const TCollection_AsciiString& other) const
{
  return IsGreater(other);
}

  //! Determines whether the beginning of this string instance matches the specified string.
  Standard_EXPORT Standard_Boolean StartsWith (const TCollection_AsciiString& theStartString) const;

  //! Determines whether the end of this string instance matches the specified string.
  Standard_EXPORT Standard_Boolean EndsWith (const TCollection_AsciiString& theEndString) const;

  //! Converts a AsciiString containing a numeric expression to
  //! an Integer.
  //! Example: "215" returns 215.
  Standard_EXPORT Standard_Integer IntegerValue() const;
  
  //! Returns True if the AsciiString contains an integer value.
  //! Note: an integer value is considered to be a real value as well.
  Standard_EXPORT Standard_Boolean IsIntegerValue() const;
  
  //! Returns True if the AsciiString starts with some characters that can be interpreted as integer or real value.
  //! @param theToCheckFull [in] when TRUE, checks if entire string defines a real value;
  //!                            otherwise checks if string starts with a real value
  //! Note: an integer value is considered to be a real value as well.
  Standard_EXPORT Standard_Boolean IsRealValue (Standard_Boolean theToCheckFull = Standard_False) const;
  
  //! Returns True if the AsciiString contains only ASCII characters
  //! between ' ' and '~'.
  //! This means no control character and no extended ASCII code.
  Standard_EXPORT Standard_Boolean IsAscii() const;
  
  //! Removes all space characters in the beginning of the string.
  Standard_EXPORT void LeftAdjust();
  
  //! left justify
  //! Length becomes equal to Width and the new characters are
  //! equal to Filler.
  //! If Width < Length nothing happens.
  //! Raises an exception if Width is less than zero.
  //! Example:
  //! before
  //! me = "abcdef" , Width = 9 , Filler = ' '
  //! after
  //! me = "abcdef   "
  Standard_EXPORT void LeftJustify (const Standard_Integer Width, const Standard_Character Filler);
  
  //! Returns number of characters in <me>.
  //! This is the same functionality as 'strlen' in C.
  //! Example
  //! TCollection_AsciiString myAlphabet("abcdef");
  //! assert ( myAlphabet.Length() == 6 );
  //! -   1 is the position of the first character in this string.
  //! -   The length of this string gives the position of its last character.
  //! -   Positions less than or equal to zero, or
  //! greater than the length of this string are
  //! invalid in functions which identify a character
  //! of this string by its position.
    Standard_Integer Length() const;
  
  //! Returns an index in the string <me> of the first occurrence
  //! of the string S in the string <me> from the starting index
  //! FromIndex to the ending index ToIndex
  //! returns zero if failure
  //! Raises an exception if FromIndex or ToIndex is out of range.
  //! Example:
  //! before
  //! me = "aabAaAa", S = "Aa", FromIndex = 1, ToIndex = 7
  //! after
  //! me = "aabAaAa"
  //! returns
  //! 4
  Standard_EXPORT Standard_Integer Location (const TCollection_AsciiString& other, const Standard_Integer FromIndex, const Standard_Integer ToIndex) const;
  
  //! Returns the index of the nth occurrence of the character C
  //! in the string <me> from the starting index FromIndex to the
  //! ending index ToIndex.
  //! Returns zero if failure.
  //! Raises an exception if FromIndex or ToIndex is out of range.
  //! Example:
  //! before
  //! me = "aabAa", N = 3, C = 'a', FromIndex = 1, ToIndex = 5
  //! after
  //! me = "aabAa"
  //! returns
  //! 5
  Standard_EXPORT Standard_Integer Location (const Standard_Integer N, const Standard_Character C, const Standard_Integer FromIndex, const Standard_Integer ToIndex) const;
  
  //! Converts <me> to its lower-case equivalent.
  //! Example
  //! TCollection_AsciiString myString("Hello Dolly");
  //! myString.UpperCase();
  //! assert ( myString == "HELLO DOLLY" );
  //! myString.LowerCase();
  //! assert ( myString == "hello dolly" );
  Standard_EXPORT void LowerCase();
  
  //! Inserts the string other at the beginning of this ASCII string.
  //! Example
  //! TCollection_AsciiString myAlphabet("cde");
  //! TCollection_AsciiString myBegin("ab");
  //! myAlphabet.Prepend(myBegin);
  //! assert ( myAlphabet == "abcde" );
  Standard_EXPORT void Prepend (const TCollection_AsciiString& other);
  
  //! Displays <me> on a stream.
  Standard_EXPORT void Print (Standard_OStream& astream) const;
friend Standard_EXPORT Standard_OStream& operator << (Standard_OStream& astream,const TCollection_AsciiString& astring);
  
  //! Read <me> from a stream.
  Standard_EXPORT void Read (Standard_IStream& astream);
friend Standard_EXPORT Standard_IStream& operator >> (Standard_IStream& astream, TCollection_AsciiString& astring);
  
  //! Converts an AsciiString containing a numeric expression.
  //! to a Real.
  //! Example: ex: "215" returns 215.0.
  //! ex: "3.14159267" returns 3.14159267.
  Standard_EXPORT Standard_Real RealValue() const;
  
  //! Remove all the occurrences of the character C in the string.
  //! Example:
  //! before
  //! me = "HellLLo", C = 'L' , CaseSensitive = True
  //! after
  //! me = "Hello"
  Standard_EXPORT void RemoveAll (const Standard_Character C, const Standard_Boolean CaseSensitive);
  
  //! Removes every <what> characters from <me>.
  Standard_EXPORT void RemoveAll (const Standard_Character what);
  
  //! Erases <ahowmany> characters from position <where>,
  //! <where> included.
  //! Example:
  //! aString contains "Hello"
  //! aString.Remove(2,2) erases 2 characters from position 2
  //! This gives "Hlo".
  Standard_EXPORT void Remove (const Standard_Integer where, const Standard_Integer ahowmany = 1);
  
  //! Removes all space characters at the end of the string.
  Standard_EXPORT void RightAdjust();
  
  //! Right justify.
  //! Length becomes equal to Width and the new characters are
  //! equal to Filler.
  //! if Width < Length nothing happens.
  //! Raises an exception if Width is less than zero.
  //! Example:
  //! before
  //! me = "abcdef" , Width = 9 , Filler = ' '
  //! after
  //! me = "   abcdef"
  Standard_EXPORT void RightJustify (const Standard_Integer Width, const Standard_Character Filler);
  
  //! Searches a CString in <me> from the beginning
  //! and returns position of first item <what> matching.
  //! it returns -1 if not found.
  //! Example:
  //! aString contains "Sample single test"
  //! aString.Search("le") returns 5
  Standard_EXPORT Standard_Integer Search (const Standard_CString what) const;
  
  //! Searches an AsciiString in <me> from the beginning
  //! and returns position of first item <what> matching.
  //! It returns -1 if not found.
  Standard_EXPORT Standard_Integer Search (const TCollection_AsciiString& what) const;
  
  //! Searches a CString in a AsciiString from the end
  //! and returns position of first item <what> matching.
  //! It returns -1 if not found.
  //! Example:
  //! aString contains "Sample single test"
  //! aString.SearchFromEnd("le") returns 12
  Standard_EXPORT Standard_Integer SearchFromEnd (const Standard_CString what) const;
  
  //! Searches a AsciiString in another AsciiString from the end
  //! and returns position of first item <what> matching.
  //! It returns -1 if not found.
  Standard_EXPORT Standard_Integer SearchFromEnd (const TCollection_AsciiString& what) const;
  
  //! Replaces one character in the AsciiString at position <where>.
  //! If <where> is less than zero or greater than the length of <me>
  //! an exception is raised.
  //! Example:
  //! aString contains "Garbake"
  //! astring.Replace(6,'g')  gives <me> = "Garbage"
  Standard_EXPORT void SetValue (const Standard_Integer where, const Standard_Character what);
  
  //! Replaces a part of <me> by a CString.
  //! If <where> is less than zero or greater than the length of <me>
  //! an exception is raised.
  //! Example:
  //! aString contains "abcde"
  //! aString.SetValue(4,"1234567") gives <me> = "abc1234567"
  Standard_EXPORT void SetValue (const Standard_Integer where, const Standard_CString what);
  
  //! Replaces a part of <me> by another AsciiString.
  Standard_EXPORT void SetValue (const Standard_Integer where, const TCollection_AsciiString& what);
  
  //! Splits a AsciiString into two sub-strings.
  //! Example:
  //! aString contains "abcdefg"
  //! aString.Split(3) gives <me> = "abc" and returns "defg"
  Standard_EXPORT TCollection_AsciiString Split (const Standard_Integer where);
  
  //! Creation of a sub-string of the string <me>.
  //! The sub-string starts to the index Fromindex and ends
  //! to the index ToIndex.
  //! Raises an exception if ToIndex or FromIndex is out of bounds
  //! Example:
  //! before
  //! me = "abcdefg", ToIndex=3, FromIndex=6
  //! after
  //! me = "abcdefg"
  //! returns
  //! "cdef"
    TCollection_AsciiString SubString (const Standard_Integer FromIndex, const Standard_Integer ToIndex) const;
  
  //! Returns pointer to AsciiString (char *).
  //! This is useful for some casual manipulations.
  //! Warning: Because this "char *" is 'const', you can't modify its contents.
    Standard_CString ToCString() const;
  
  //! Extracts <whichone> token from <me>.
  //! By default, the <separators> is set to space and tabulation.
  //! By default, the token extracted is the first one (whichone = 1).
  //! <separators> contains all separators you need.
  //! If no token indexed by <whichone> is found, it returns empty AsciiString.
  //! Example:
  //! aString contains "This is a     message"
  //! aString.Token()  returns "This"
  //! aString.Token(" ",4) returns "message"
  //! aString.Token(" ",2) returns "is"
  //! aString.Token(" ",9) returns ""
  //! Other separators than space character and tabulation are allowed :
  //! aString contains "1234; test:message   , value"
  //! aString.Token("; :,",4) returns "value"
  //! aString.Token("; :,",2) returns "test"
  Standard_EXPORT TCollection_AsciiString Token (const Standard_CString separators = " \t", const Standard_Integer whichone = 1) const;
  
  //! Truncates <me> to <ahowmany> characters.
  //! Example:  me = "Hello Dolly" -> Trunc(3) -> me = "Hel"
  Standard_EXPORT void Trunc (const Standard_Integer ahowmany);
  
  //! Converts <me> to its upper-case equivalent.
  Standard_EXPORT void UpperCase();
  
  //! Length of the string ignoring all spaces (' ') and the
  //! control character at the end.
  Standard_EXPORT Standard_Integer UsefullLength() const;
  
  //! Returns character at position <where> in <me>.
  //! If <where> is less than zero or greater than the length of <me>,
  //! an exception is raised.
  //! Example:
  //! aString contains "Hello"
  //! aString.Value(2) returns 'e'
  Standard_EXPORT Standard_Character Value (const Standard_Integer where) const;
  
  //! Computes a hash code for the given ASCII string
  //! Returns the same integer value as the hash function for TCollection_ExtendedString
  //! @return a computed hash code
  size_t HashCode() const;

  //! Returns True  when the two  strings are the same.
  //! (Just for HashCode for AsciiString)
    static Standard_Boolean IsEqual (const TCollection_AsciiString& string1, const TCollection_AsciiString& string2);
  
  //! Returns True  when the two  strings are the same.
  //! (Just for HashCode for AsciiString)
    static Standard_Boolean IsEqual (const TCollection_AsciiString& string1, const Standard_CString string2);

  //! Returns True if the strings contain same characters.
  Standard_EXPORT static Standard_Boolean IsSameString (const TCollection_AsciiString& theString1,
                                                        const TCollection_AsciiString& theString2,
                                                        const Standard_Boolean theIsCaseSensitive);

friend class TCollection_HAsciiString;

private:

  //! Internal wrapper to allocate on stack or heap
  void allocate(const int theLength);

  //! Internal wrapper to reallocate on stack or heap
  void reallocate(const int theLength);

  //! Internal wrapper to deallocate on stack
  void deallocate();

private:

  Standard_PCharacter mystring{}; //!< NULL-terminated string
  Standard_Integer    mylength{}; //!< length in bytes (excluding terminating NULL symbol)
};

#include <TCollection_AsciiString.lxx>

#endif // _TCollection_AsciiString_HeaderFile
