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

#ifndef _NCollection_UtfString_H__
#define _NCollection_UtfString_H__

#include "NCollection_UtfIterator.hxx"

#include <Standard.hxx>

#include <cstring>
#include <cstdlib>

//! This template class represent constant UTF-* string.
//! String stored in memory continuously, always NULL-terminated
//! and can be used as standard C-string using ToCString() method.
//!
//! Notice that changing the string is not allowed
//! and any modifications should produce new string.
template<typename Type>
class NCollection_UtfString
{

public:

  NCollection_UtfIterator<Type> Iterator() const
  {
    return NCollection_UtfIterator<Type> (myString);
  }

  //! @return the size of the buffer, excluding NULL-termination symbol
  Standard_Integer Size() const
  {
    return mySize;
  }

  //! @return the length of the string in Unicode symbols
  Standard_Integer Length() const
  {
    return myLength;
  }

  //! Retrieve Unicode symbol at specified position.
  //! Warning! This is a slow access. Iterator should be used for consecutive parsing.
  //! @param theCharIndex the index of the symbol, should be lesser than Length()
  //! @return the Unicode symbol value
  Standard_Utf32Char GetChar (const Standard_Integer theCharIndex) const;

  //! Retrieve string buffer at specified position.
  //! Warning! This is a slow access. Iterator should be used for consecutive parsing.
  //! @param theCharIndex the index of the symbol, should be lesser than Length()
  //! @return the pointer to the symbol
  const Type* GetCharBuffer (const Standard_Integer theCharIndex) const;

  //! Retrieve Unicode symbol at specified position.
  //! Warning! This is a slow access. Iterator should be used for consecutive parsing.
  Standard_Utf32Char operator[] (const Standard_Integer theCharIndex) const
  {
    return GetChar (theCharIndex);
  }

  //! Initialize empty string.
  NCollection_UtfString();

  //! Copy constructor.
  //! @param theCopy string to copy.
  NCollection_UtfString (const NCollection_UtfString& theCopy);

  //! Copy constructor from NULL-terminated UTF-8 string.
  //! @param theCopyUtf8 NULL-terminated UTF-8 string to copy
  //! @param theLength   the length limit in Unicode symbols (NOT bytes!)
  NCollection_UtfString (const char*            theCopyUtf8,
                         const Standard_Integer theLength = -1);

  //! Copy constructor from NULL-terminated UTF-16 string.
  //! @param theCopyUtf16 NULL-terminated UTF-16 string to copy
  //! @param theLength    the length limit in Unicode symbols (NOT bytes!)
  NCollection_UtfString (const Standard_Utf16Char* theCopyUtf16,
                         const Standard_Integer    theLength = -1);

  //! Copy constructor from NULL-terminated UTF-32 string.
  //! @param theCopyUtf32 NULL-terminated UTF-32 string to copy
  //! @param theLength    the length limit in Unicode symbols (NOT bytes!)
  NCollection_UtfString (const Standard_Utf32Char* theCopyUtf32,
                         const Standard_Integer    theLength = -1);

#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED) || (defined(_MSC_VER) && _MSC_VER >= 1900)
  //! Copy constructor from NULL-terminated wide UTF string.
  //! @param theCopyUtfWide NULL-terminated wide UTF string to copy
  //! @param theLength      the length limit in Unicode symbols (NOT bytes!)
  //!
  //! This constructor is undefined if Standard_WideChar is the same type as Standard_Utf16Char.
  NCollection_UtfString (const Standard_WideChar* theCopyUtfWide,
                         const Standard_Integer   theLength = -1);
#endif

  //! Copy from NULL-terminated Unicode string.
  //! @param theStringUtf NULL-terminated Unicode string
  //! @param theLength    the length limit in Unicode symbols
  template <typename TypeFrom>
  void FromUnicode (const TypeFrom*        theStringUtf,
                    const Standard_Integer theLength = -1);

  //! Copy from NULL-terminated multibyte string in system locale.
  //! You should avoid this function unless extreme necessity.
  //! @param theString NULL-terminated multibyte string
  //! @param theLength the length limit in Unicode symbols
  void FromLocale (const char*            theString,
                   const Standard_Integer theLength = -1);

  //! Destructor.
  ~NCollection_UtfString();

  //! Compares this string with another one.
  bool IsEqual (const NCollection_UtfString& theCompare) const;

  //! Returns the substring.
  //! @param theStart start index (inclusive) of subString
  //! @param theEnd   end index   (exclusive) of subString
  //! @return the substring
  NCollection_UtfString SubString (const Standard_Integer theStart,
                                   const Standard_Integer theEnd) const;

  //! Returns NULL-terminated Unicode string.
  //! Should not be modifed or deleted!
  //! @return (const Type* ) pointer to string
  const Type* ToCString() const
  {
    return myString;
  }

  //! @return copy in UTF-8 format
  const NCollection_UtfString<Standard_Utf8Char> ToUtf8() const;

  //! @return copy in UTF-16 format
  const NCollection_UtfString<Standard_Utf16Char> ToUtf16() const;

  //! @return copy in UTF-32 format
  const NCollection_UtfString<Standard_Utf32Char> ToUtf32() const;

  //! @return copy in wide format (UTF-16 on Windows and UTF-32 on Linux)
  const NCollection_UtfString<Standard_WideChar> ToUtfWide() const;

  //! Converts the string into multibyte string.
  //! You should avoid this function unless extreme necessity.
  //! @param theBuffer    output buffer
  //! @param theSizeBytes buffer size in bytes
  //! @return true on success
  bool ToLocale (char*                  theBuffer,
                 const Standard_Integer theSizeBytes) const;

  //! @return true if string is empty
  bool IsEmpty() const
  {
    return myString[0] == Type(0);
  }

  //! Zero string.
  void Clear();

public: //! @name assign operators

  //! Copy from another string.
  const NCollection_UtfString& operator= (const NCollection_UtfString& theOther);

  //! Copy from UTF-8 NULL-terminated string.
  const NCollection_UtfString& operator= (const char* theStringUtf8);

  //! Copy from wchar_t UTF NULL-terminated string.
  const NCollection_UtfString& operator= (const Standard_WideChar* theStringUtfWide);

  //! Join strings.
  NCollection_UtfString& operator+= (const NCollection_UtfString& theAppend);

  //! Join two strings.
  friend NCollection_UtfString operator+ (const NCollection_UtfString& theLeft,
                                          const NCollection_UtfString& theRight)
  {
    NCollection_UtfString aSumm;
    strFree (aSumm.myString);
    aSumm.mySize   = theLeft.mySize   + theRight.mySize;
    aSumm.myLength = theLeft.myLength + theRight.myLength;
    aSumm.myString = strAlloc (aSumm.mySize);

    // copy bytes
    strCopy ((Standard_Byte* )aSumm.myString,                  (const Standard_Byte* )theLeft.myString,  theLeft.mySize);
    strCopy ((Standard_Byte* )aSumm.myString + theLeft.mySize, (const Standard_Byte* )theRight.myString, theRight.mySize);
    return aSumm;
  }

public: //! @name compare operators

  bool operator== (const NCollection_UtfString& theCompare) const
  {
    return IsEqual (theCompare);
  }
  bool operator!= (const NCollection_UtfString& theCompare) const;

private: //! @name low-level methods

  //! Compute advance for specified string.
  //! @param theStringUtf pointer to the NULL-terminated Unicode string
  //! @param theLengthMax length limit (to cut the string), set to -1 to compute up to NULL-termination symbol
  //! @param theSizeBytes advance in bytes (out)
  //! @param theLength    string length (out)
  template<typename TypeFrom>
  static void strGetAdvance (const TypeFrom*        theStringUtf,
                             const Standard_Integer theLengthMax,
                             Standard_Integer&      theSizeBytes,
                             Standard_Integer&      theLength);

  //! Allocate NULL-terminated string buffer.
  static Type* strAlloc (const Standard_Size theSizeBytes)
  {
    Type* aPtr = (Type* )Standard::Allocate (theSizeBytes + sizeof(Type));
    if (aPtr != NULL)
    {
      // always NULL-terminate the string
      aPtr[theSizeBytes / sizeof(Type)] = Type(0);
    }
    return aPtr;
  }

  //! Release string buffer and nullify the pointer.
  static void strFree (Type*& thePtr)
  {
    Standard::Free (thePtr);
  }

  //! Provides bytes interface to avoid incorrect pointer arithmetics.
  static void strCopy (Standard_Byte*         theStrDst,
                       const Standard_Byte*   theStrSrc,
                       const Standard_Integer theSizeBytes)
  {
    std::memcpy (theStrDst, theStrSrc, (Standard_Size )theSizeBytes);
  }

  //! Compare two Unicode strings per-byte.
  static bool strAreEqual (const Type*            theString1,
                           const Standard_Integer theSizeBytes1,
                           const Type*            theString2,
                           const Standard_Integer theSizeBytes2)
  {
    return (theSizeBytes1 == theSizeBytes2)
        && (std::memcmp (theString1, theString2, (Standard_Size )theSizeBytes1) == 0);
  }

private: //! @name private fields

  Type*            myString; //!< string buffer
  Standard_Integer mySize;   //!< buffer size in bytes, excluding NULL-termination symbol
  Standard_Integer myLength; //!< length of the string in Unicode symbols (cached value, excluding NULL-termination symbol)

};

typedef NCollection_UtfString<Standard_Utf8Char>  NCollection_Utf8String;
typedef NCollection_UtfString<Standard_Utf16Char> NCollection_Utf16String;
typedef NCollection_UtfString<Standard_Utf32Char> NCollection_Utf32String;
typedef NCollection_UtfString<Standard_WideChar>  NCollection_UtfWideString;

// template implementation (inline methods)
#include "NCollection_UtfString.lxx"

#endif // _NCollection_UtfString_H__
