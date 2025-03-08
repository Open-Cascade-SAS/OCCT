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

#ifndef NCollection_UtfIterator_HeaderFile
#define NCollection_UtfIterator_HeaderFile

#include <Standard_Handle.hxx>

//! Template class for Unicode strings support.
//!
//! It defines an iterator and provide correct way to read multi-byte text (UTF-8 and UTF-16)
//! and convert it from one to another.
//! The current value of iterator is returned as UTF-32 Unicode symbol.
//!
//! Here and below term "Unicode symbol" is used as
//! synonym of "Unicode code point".
template <typename Type>
class NCollection_UtfIterator
{

public:
  //! Constructor.
  //! @param theString buffer to iterate
  NCollection_UtfIterator(const Type* theString)
      : myPosition(theString),
        myPosNext(theString),
        myCharIndex(0),
        myCharUtf32(0)
  {
    if (theString != NULL)
    {
      ++(*this);
      myCharIndex = 0;
    }
  }

  //! Initialize iterator within specified NULL-terminated string.
  void Init(const Type* theString)
  {
    myPosition  = theString;
    myPosNext   = theString;
    myCharUtf32 = 0;
    if (theString != NULL)
    {
      ++(*this);
    }
    myCharIndex = 0;
  }

  //! Pre-increment operator. Reads the next unicode symbol.
  //! Notice - no protection against overrun!
  NCollection_UtfIterator& operator++()
  {
    myPosition = myPosNext;
    ++myCharIndex;
    readNext(static_cast<const typename CharTypeChooser<Type>::type*>(0));
    return *this;
  }

  //! Post-increment operator.
  //! Notice - no protection against overrun!
  NCollection_UtfIterator operator++(int)
  {
    NCollection_UtfIterator aCopy = *this;
    ++*this;
    return aCopy;
  }

  //! Equality operator.
  bool operator==(const NCollection_UtfIterator& theRight) const
  {
    return myPosition == theRight.myPosition;
  }

  //! Return true if Unicode symbol is within valid range.
  bool IsValid() const { return myCharUtf32 <= UTF32_MAX_LEGAL; }

  //! Dereference operator.
  //! @return the UTF-32 codepoint of the symbol currently pointed by iterator.
  Standard_Utf32Char operator*() const { return myCharUtf32; }

  //! Buffer-fetching getter.
  const Type* BufferHere() const { return myPosition; }

  //! Buffer-fetching getter. Dangerous! Iterator should be reinitialized on buffer change.
  Type* ChangeBufferHere() { return (Type*)myPosition; }

  //! Buffer-fetching getter.
  const Type* BufferNext() const { return myPosNext; }

  //! @return the index displacement from iterator initialization
  //!         (first symbol has index 0)
  Standard_Integer Index() const { return myCharIndex; }

  //! @return the advance in bytes to store current symbol in UTF-8.
  //! 0 means an invalid symbol;
  //! 1-4 bytes are valid range.
  Standard_Integer AdvanceBytesUtf8() const;

  //! @return the advance in bytes to store current symbol in UTF-16.
  //! 0 means an invalid symbol;
  //! 2 bytes is a general case;
  //! 4 bytes for surrogate pair.
  Standard_Integer AdvanceBytesUtf16() const;

  //! @return the advance in bytes to store current symbol in UTF-16.
  //! 0 means an invalid symbol;
  //! 1 16-bit code unit is a general case;
  //! 2 16-bit code units for surrogate pair.
  Standard_Integer AdvanceCodeUnitsUtf16() const;

  //! @return the advance in bytes to store current symbol in UTF-32.
  //! Always 4 bytes (method for consistency).
  Standard_Integer AdvanceBytesUtf32() const
  {
    return Standard_Integer(sizeof(Standard_Utf32Char));
  }

  //! Fill the UTF-8 buffer within current Unicode symbol.
  //! Use method AdvanceUtf8() to allocate buffer with enough size.
  //! @param theBuffer buffer to fill
  //! @return new buffer position (for next char)
  Standard_Utf8Char*  GetUtf8(Standard_Utf8Char* theBuffer) const;
  Standard_Utf8UChar* GetUtf8(Standard_Utf8UChar* theBuffer) const;

  //! Fill the UTF-16 buffer within current Unicode symbol.
  //! Use method AdvanceUtf16() to allocate buffer with enough size.
  //! @param theBuffer buffer to fill
  //! @return new buffer position (for next char)
  Standard_Utf16Char* GetUtf16(Standard_Utf16Char* theBuffer) const;

  //! Fill the UTF-32 buffer within current Unicode symbol.
  //! Use method AdvanceUtf32() to allocate buffer with enough size.
  //! @param theBuffer buffer to fill
  //! @return new buffer position (for next char)
  Standard_Utf32Char* GetUtf32(Standard_Utf32Char* theBuffer) const;

  //! @return the advance in TypeWrite chars needed to store current symbol
  template <typename TypeWrite>
  inline Standard_Integer AdvanceBytesUtf() const
  {
    return advanceBytes(static_cast<const typename CharTypeChooser<TypeWrite>::type*>(0));
  }

  //! Fill the UTF-** buffer within current Unicode symbol.
  //! Use method AdvanceUtf**() to allocate buffer with enough size.
  //! @param theBuffer buffer to fill
  //! @return new buffer position (for next char)
  template <typename TypeWrite>
  inline TypeWrite* GetUtf(TypeWrite* theBuffer) const
  {
    return (
      TypeWrite*)(getUtf(reinterpret_cast<typename CharTypeChooser<TypeWrite>::type*>(theBuffer)));
  }

private:
  //! Helper template class dispatching its argument class
  //! to the equivalent (by size) character (Unicode code unit) type.
  //! The code unit type is defined as nested typedef "type".
  //!
  //! In practice this is relevant for wchar_t type:
  //! typename CharTypeChooser<wchar_t>::type resolves to
  //! Standard_Utf16Char on Windows and to Standard_Utf32Char on Linux.
  template <typename TypeChar>
  class CharTypeChooser
      : public std::conditional<
          sizeof(TypeChar) == 1,
          Standard_Utf8Char,
          typename std::conditional<
            sizeof(TypeChar) == 2,
            Standard_Utf16Char,
            typename std::conditional<sizeof(TypeChar) == 4, Standard_Utf32Char, void>::type>::type>
  {
  };

  //! Helper function for reading a single Unicode symbol from the UTF-8 string.
  //! Updates internal state appropriately.
  void readUTF8();

  //! Helper function for reading a single Unicode symbol from the UTF-16 string.
  //! Updates internal state appropriately.
  void readUTF16();

  //! Helper overload methods to dispatch reading function depending on code unit size
  void readNext(const Standard_Utf8Char*) { readUTF8(); }

  void readNext(const Standard_Utf16Char*) { readUTF16(); }

  void readNext(const Standard_Utf32Char*) { myCharUtf32 = *myPosNext++; }

  //! Helper overload methods to dispatch advance function depending on code unit size
  Standard_Integer advanceBytes(const Standard_Utf8Char*) const { return AdvanceBytesUtf8(); }

  Standard_Integer advanceBytes(const Standard_Utf16Char*) const { return AdvanceBytesUtf16(); }

  Standard_Integer advanceBytes(const Standard_Utf32Char*) const { return AdvanceBytesUtf32(); }

  //! Helper overload methods to dispatch getter function depending on code unit size
  Standard_Utf8Char* getUtf(Standard_Utf8Char* theBuffer) const { return GetUtf8(theBuffer); }

  Standard_Utf16Char* getUtf(Standard_Utf16Char* theBuffer) const { return GetUtf16(theBuffer); }

  Standard_Utf32Char* getUtf(Standard_Utf32Char* theBuffer) const { return GetUtf32(theBuffer); }

private: //! @name unicode magic numbers
  static const unsigned char      UTF8_BYTES_MINUS_ONE[256];
  static const Standard_Utf32Char offsetsFromUTF8[6];
  static const unsigned char      UTF8_FIRST_BYTE_MARK[7];
  static const Standard_Utf32Char UTF8_BYTE_MASK;
  static const Standard_Utf32Char UTF8_BYTE_MARK;
  static const Standard_Utf32Char UTF16_SURROGATE_HIGH_START;
  static const Standard_Utf32Char UTF16_SURROGATE_HIGH_END;
  static const Standard_Utf32Char UTF16_SURROGATE_LOW_START;
  static const Standard_Utf32Char UTF16_SURROGATE_LOW_END;
  static const Standard_Utf32Char UTF16_SURROGATE_HIGH_SHIFT;
  static const Standard_Utf32Char UTF16_SURROGATE_LOW_BASE;
  static const Standard_Utf32Char UTF16_SURROGATE_LOW_MASK;
  static const Standard_Utf32Char UTF32_MAX_BMP;
  static const Standard_Utf32Char UTF32_MAX_LEGAL;

private:                          //! @name private fields
  const Type*        myPosition;  //!< buffer position of the first element in the current symbol
  const Type*        myPosNext;   //!< buffer position of the first element in the next symbol
  Standard_Integer   myCharIndex; //!< index displacement from iterator initialization
  Standard_Utf32Char myCharUtf32; //!< Unicode symbol stored at the current buffer position
};

typedef NCollection_UtfIterator<Standard_Utf8Char>  NCollection_Utf8Iter;
typedef NCollection_UtfIterator<Standard_Utf16Char> NCollection_Utf16Iter;
typedef NCollection_UtfIterator<Standard_Utf32Char> NCollection_Utf32Iter;
typedef NCollection_UtfIterator<Standard_WideChar>  NCollection_UtfWideIter;

// template implementation
#include "NCollection_UtfIterator.lxx"

#endif // _NCollection_UtfIterator_H__
