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

#include <TCollection_AsciiString.hxx>

#include <NCollection_UtfIterator.hxx>
#include <Standard.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <functional>

namespace
{
static char THE_DEFAULT_CHAR_STRING[1] = {'\0'};

//! Calculate padded allocation size: minimum +1 byte guaranteed, up to +4 bytes
//! This provides automatic space for null terminator and some extra buffer, aligned to 4-byte
//! boundary
inline Standard_Size calculatePaddedSize(const int theLength)
{
  return (theLength + 4) & ~0x3; // Always guarantees at least +1 byte, up to +4 bytes
}

//! Helper structure to hold formatted integer string with its length
struct FormattedInteger
{
  char             Buffer[16]; // Enough for 32-bit int + sign + null terminator
  Standard_Integer Length;

  FormattedInteger(const Standard_Integer theValue) { Length = Sprintf(Buffer, "%d", theValue); }
};

//! Helper structure to hold formatted real number string with its length
struct FormattedReal
{
  char             Buffer[64]; // Enough for double in %g format + null terminator
  Standard_Integer Length;

  FormattedReal(const Standard_Real theValue) { Length = Sprintf(Buffer, "%g", theValue); }
};
} // namespace

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString() noexcept
{
  allocate(0);
}

//=================================================================================================

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(const Standard_CString theString,
                                                 const Standard_Integer theLen)
{
  if (theString == NULL)
  {
    throw Standard_NullObject("TCollection_AsciiString(): NULL pointer passed to constructor");
  }
  int aLength = 0;
  for (; aLength < theLen && theString[aLength] != '\0'; ++aLength)
  {
  }
  allocate(aLength);
  memcpy(myString, theString, myLength);
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(const Standard_Character theChar)
{
  if (theChar != '\0')
  {
    allocate(1);
    myString[0] = theChar;
  }
  else
  {
    allocate(0);
  }
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(const Standard_Integer   theLength,
                                                 const Standard_Character theFiller)
{
  allocate(theLength);
  memset(myString, theFiller, theLength);
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(const Standard_Integer theValue)
{
  const FormattedInteger aFormatted(theValue);
  allocate(aFormatted.Length);
  memcpy(myString, aFormatted.Buffer, myLength);
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(const Standard_Real theValue)
{
  const FormattedReal aFormatted(theValue);
  allocate(aFormatted.Length);
  memcpy(myString, aFormatted.Buffer, myLength);
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(const TCollection_AsciiString& theString)
{
  allocate(theString.myLength);
  if (myLength != 0)
  {
    memcpy(myString, theString.myString, myLength);
  }
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(TCollection_AsciiString&& theOther)
  Standard_Noexcept
{
  if (theOther.myString == THE_DEFAULT_CHAR_STRING)
  {
    allocate(0);
  }
  else
  {
    myString = theOther.myString;
    myLength = theOther.myLength;
  }
  theOther.myLength = 0;
  theOther.myString = THE_DEFAULT_CHAR_STRING;
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(const TCollection_AsciiString& theString,
                                                 const Standard_Character       theChar)
{
  allocate(theString.myLength + 1);
  if (theString.myLength != 0)
  {
    memcpy(myString, theString.myString, theString.myLength);
  }
  myString[myLength - 1] = theChar;
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(const TCollection_AsciiString& theString1,
                                                 const Standard_CString         theString2)
{
  const Standard_Integer aStr2Len = Standard_Integer(theString2 ? strlen(theString2) : 0);
  allocate(theString1.myLength + aStr2Len);
  if (theString1.myLength != 0)
  {
    memcpy(myString, theString1.myString, theString1.myLength);
  }
  if (aStr2Len != 0)
  {
    memcpy(myString + theString1.myLength, theString2, aStr2Len);
  }
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(const TCollection_AsciiString& theString1,
                                                 const TCollection_AsciiString& theString2)
{
  allocate(theString1.myLength + theString2.myLength);
  if (theString1.myLength)
  {
    memcpy(myString, theString1.myString, theString1.myLength);
  }
  if (theString2.myLength != 0)
  {
    memcpy(myString + theString1.myLength, theString2.myString, theString2.myLength);
  }
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(
  const TCollection_ExtendedString& theExtendedString,
  const Standard_Character          theReplaceNonAscii)
{
  if (theReplaceNonAscii)
  {
    allocate(theExtendedString.Length());
    for (int i = 0; i < myLength; i++)
    {
      Standard_ExtCharacter c = theExtendedString.Value(i + 1);
      myString[i]             = (IsAnAscii(c) ? ToCharacter(c) : theReplaceNonAscii);
    }
  }
  else
  {
    // create UTF-8 string
    // Note: allocate() adds padding (theLength + 4) & ~0x3, so no need for +1 for null terminator
    allocate(theExtendedString.LengthOfCString());
    theExtendedString.ToUTF8CString(myString);
  }
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(const Standard_WideChar* theStringUtf)
{
  int aLength = 0;
  for (NCollection_UtfWideIter anIter(theStringUtf); *anIter != 0; ++anIter)
  {
    aLength += anIter.AdvanceBytesUtf8();
  }
  allocate(aLength);
  NCollection_UtfWideIter anIterRead(theStringUtf);
  for (Standard_Utf8Char* anIterWrite = myString; *anIterRead != 0; ++anIterRead)
  {
    anIterWrite = anIterRead.GetUtf(anIterWrite);
  }
}

//=================================================================================================

void TCollection_AsciiString::AssignCat(const Standard_Integer theOther)
{
  const FormattedInteger aFormatted(theOther);
  AssignCat(aFormatted.Buffer, aFormatted.Length);
}

//=================================================================================================

void TCollection_AsciiString::AssignCat(const Standard_Real theOther)
{
  const FormattedReal aFormatted(theOther);
  AssignCat(aFormatted.Buffer, aFormatted.Length);
}

//=================================================================================================

void TCollection_AsciiString::AssignCat(const Standard_Character theOther)
{
  if (theOther != '\0')
  {
    reallocate(myLength + 1);
    myString[myLength - 1] = theOther;
  }
}

//=================================================================================================

void TCollection_AsciiString::AssignCat(const Standard_CString theString,
                                        const Standard_Integer theLength)
{
  if (theLength == 0 || theString == nullptr)
    return;

  // Check if theString points into our own buffer (self-assignment protection)
  // Use std::less for well-defined pointer comparison (avoids undefined behavior)
  const bool isSelfReference =
    !std::less<const Standard_CString>()(theString, myString)
    && std::less<const Standard_CString>()(theString, myString + myLength);

  if (isSelfReference)
  {
    // Self-reference: need to copy the data before reallocation
    const Standard_Integer anOffset    = static_cast<Standard_Integer>(theString - myString);
    const Standard_Integer anOldLength = myLength;

    reallocate(myLength + theLength);

    // After reallocation, copy from the updated position in the same buffer
    memmove(myString + anOldLength, myString + anOffset, theLength);
  }
  else
  {
    // Normal case: no overlap with our buffer
    const Standard_Integer anOldLength = myLength;
    reallocate(myLength + theLength);
    memcpy(myString + anOldLength, theString, theLength);
  }

  myString[myLength] = '\0'; // Ensure null termination
}

//=================================================================================================

void TCollection_AsciiString::Capitalize()
{
  if (myLength)
    myString[0] = ::UpperCase(myString[0]);
  for (int i = 1; i < myLength; i++)
    myString[i] = ::LowerCase(myString[i]);
}

//=================================================================================================

TCollection_AsciiString TCollection_AsciiString::Cat(const Standard_CString theString,
                                                     const Standard_Integer theLength) const
{
  TCollection_AsciiString aResult(*this);
  aResult.AssignCat(theString, theLength);
  return aResult;
}

//=================================================================================================

TCollection_AsciiString TCollection_AsciiString::Cat(const Standard_Integer theOther) const
{
  const FormattedInteger aFormatted(theOther);
  return Cat(aFormatted.Buffer, aFormatted.Length);
}

//=================================================================================================

TCollection_AsciiString TCollection_AsciiString::Cat(const Standard_Real theOther) const
{
  const FormattedReal aFormatted(theOther);
  return Cat(aFormatted.Buffer, aFormatted.Length);
}

//=================================================================================================

void TCollection_AsciiString::Center(const Standard_Integer   theWidth,
                                     const Standard_Character theFiller)
{
  if (theWidth > myLength)
  {
    Standard_Integer aNewLength = myLength + ((theWidth - myLength) / 2);
    LeftJustify(aNewLength, theFiller);
    RightJustify(theWidth, theFiller);
  }
  else if (theWidth < 0)
  {
    throw Standard_NegativeValue();
  }
}

//=================================================================================================

void TCollection_AsciiString::ChangeAll(const Standard_Character theChar,
                                        const Standard_Character theNewChar,
                                        const Standard_Boolean   theCaseSensitive)
{
  if (theCaseSensitive)
  {
    for (int i = 0; i < myLength; i++)
      if (myString[i] == theChar)
        myString[i] = theNewChar;
  }
  else
  {
    Standard_Character anUpperChar = ::UpperCase(theChar);
    for (int i = 0; i < myLength; i++)
      if (::UpperCase(myString[i]) == anUpperChar)
        myString[i] = theNewChar;
  }
}

//=================================================================================================

void TCollection_AsciiString::Clear()
{
  deallocate();
}

//=================================================================================================

void TCollection_AsciiString::Copy(const Standard_CString theString,
                                   const Standard_Integer theLength)
{
  if (myString == theString)
  {
    return;
  }
  if (theLength == 0 || theString == nullptr)
  {
    deallocate();
    allocate(0);
    return;
  }

  if (theLength <= myLength)
  {
    // Reuse existing buffer - just update length and null terminator
    myLength = theLength;
  }
  else
  {
    // Need bigger buffer - deallocate and allocate fresh
    deallocate();
    allocate(theLength);
  }
  memcpy(myString, theString, myLength);
  myString[myLength] = '\0';
}

//=================================================================================================

void TCollection_AsciiString::Move(TCollection_AsciiString&& theOther)
{
  if (&theOther == this)
  {
    return;
  }
  if (myString != THE_DEFAULT_CHAR_STRING)
  {
    Standard::Free(myString);
  }
  myString          = theOther.myString;
  myLength          = theOther.myLength;
  theOther.myString = THE_DEFAULT_CHAR_STRING;
  theOther.myLength = 0;
}

//=================================================================================================

void TCollection_AsciiString::Swap(TCollection_AsciiString& theOther)
{
  if (&theOther == this)
  {
    return;
  }
  std::swap(myString, theOther.myString);
  std::swap(myLength, theOther.myLength);
}

//=================================================================================================

TCollection_AsciiString::~TCollection_AsciiString()
{
  deallocate();
}

//=================================================================================================

Standard_Integer TCollection_AsciiString::FirstLocationInSet(
  const Standard_CString theSet,
  const Standard_Integer theSetLength,
  const Standard_Integer theFromIndex,
  const Standard_Integer theToIndex) const
{
  if (myLength == 0 || theSetLength == 0)
    return 0;
  if (theFromIndex > 0 && theToIndex <= myLength && theFromIndex <= theToIndex)
  {
    for (int i = theFromIndex - 1; i < theToIndex; i++)
      for (int j = 0; j < theSetLength; j++)
        if (myString[i] == theSet[j])
          return i + 1;
    return 0;
  }
  throw Standard_OutOfRange();
}

//=================================================================================================

Standard_Integer TCollection_AsciiString::FirstLocationNotInSet(
  const Standard_CString theSet,
  const Standard_Integer theSetLength,
  const Standard_Integer theFromIndex,
  const Standard_Integer theToIndex) const
{
  if (myLength == 0 || theSetLength == 0)
    return 0;
  if (theFromIndex > 0 && theToIndex <= myLength && theFromIndex <= theToIndex)
  {
    Standard_Boolean aFind;
    for (int i = theFromIndex - 1; i < theToIndex; i++)
    {
      aFind = Standard_False;
      for (int j = 0; j < theSetLength; j++)
        if (myString[i] == theSet[j])
          aFind = Standard_True;
      if (!aFind)
        return i + 1;
    }
    return 0;
  }
  throw Standard_OutOfRange();
}

//=================================================================================================

void TCollection_AsciiString::Insert(const Standard_Integer   theWhere,
                                     const Standard_Character theWhat)
{
  if (theWhere > myLength + 1)
    throw Standard_OutOfRange("TCollection_AsciiString::Insert : Parameter theWhere is too big");
  if (theWhere < 1)
    throw Standard_OutOfRange("TCollection_AsciiString::Insert : Parameter theWhere is too small");

  const int anOldLength = myLength;
  reallocate(myLength + 1);
  if (theWhere != anOldLength + 1)
  {
    for (int i = anOldLength - 1; i >= theWhere - 1; i--)
      myString[i + 1] = myString[i];
  }
  myString[theWhere - 1] = theWhat;
}

//=================================================================================================

void TCollection_AsciiString::Insert(const Standard_Integer theWhere,
                                     const Standard_CString theString,
                                     const Standard_Integer theLength)
{
  if (theWhere < 1 || theWhere > myLength + 1)
    throw Standard_OutOfRange("TCollection_AsciiString::Insert : parameter theWhere");

  if (theLength == 0 || theString == nullptr)
    return;

  const int anInsertSize  = theLength;
  const int anOldLength   = myLength;
  const int aNewLength    = anOldLength + anInsertSize;
  const int anInsertIndex = theWhere - 1;

  // Check if theString points into our own buffer (self-reference protection)
  // Use std::less for well-defined pointer comparison (avoids undefined behavior)
  const bool isSelfReference =
    !std::less<const Standard_CString>()(theString, myString)
    && std::less<const Standard_CString>()(theString, myString + myLength);

  if (isSelfReference)
  {
    // Self-reference: save offset, reallocate, then handle the shifted source
    const Standard_Integer anOffset = static_cast<Standard_Integer>(theString - myString);

    reallocate(aNewLength);

    // Shift existing characters to the right
    if (anInsertIndex < anOldLength)
    {
      memmove(myString + anInsertIndex + anInsertSize,
              myString + anInsertIndex,
              anOldLength - anInsertIndex);
    }

    // After the shift, if the source was at or after the insertion point,
    // it has been shifted by anInsertSize positions to the right
    const Standard_Integer aSourceOffset =
      (anOffset >= anInsertIndex) ? (anOffset + anInsertSize) : anOffset;

    // Insert from the recalculated position in the same buffer using memmove
    memmove(myString + anInsertIndex, myString + aSourceOffset, anInsertSize);
  }
  else
  {
    // Normal case: no overlap with our buffer
    reallocate(aNewLength);

    // Shift existing characters to the right
    if (anInsertIndex < anOldLength)
    {
      memmove(myString + anInsertIndex + anInsertSize,
              myString + anInsertIndex,
              anOldLength - anInsertIndex);
    }

    // Insert the new content
    memcpy(myString + anInsertIndex, theString, anInsertSize);
  }
}

//=================================================================================================

void TCollection_AsciiString::InsertAfter(const Standard_Integer theIndex,
                                          const Standard_CString theString,
                                          const Standard_Integer theLength)
{
  if (theIndex < 0 || theIndex > myLength)
    throw Standard_OutOfRange();
  Insert(theIndex + 1, theString, theLength);
}

//=================================================================================================

void TCollection_AsciiString::InsertBefore(const Standard_Integer theIndex,
                                           const Standard_CString theString,
                                           const Standard_Integer theLength)
{
  if (theIndex < 1 || theIndex > myLength)
    throw Standard_OutOfRange();
  Insert(theIndex, theString, theLength);
}

//=================================================================================================

Standard_Boolean TCollection_AsciiString::IsEqual(const Standard_CString theString,
                                                  const Standard_Integer theLength) const
{
  if (myLength != theLength)
    return Standard_False;

  if (myLength == 0)
    return Standard_True;

  return memcmp(myString, theString, myLength) == 0;
}

//=================================================================================================

Standard_Boolean TCollection_AsciiString::IsLess(const Standard_CString theString,
                                                 const Standard_Integer theLength) const
{
  const int aMinLength = std::min(myLength, theLength);
  const int aResult    = memcmp(myString, theString, aMinLength);

  if (aResult < 0)
    return Standard_True;
  if (aResult > 0)
    return Standard_False;

  return myLength < theLength;
}

//=================================================================================================

Standard_Boolean TCollection_AsciiString::IsGreater(const Standard_CString theString,
                                                    const Standard_Integer theLength) const
{
  const int aMinLength = std::min(myLength, theLength);
  const int aResult    = memcmp(myString, theString, aMinLength);

  if (aResult > 0)
    return Standard_True;
  if (aResult < 0)
    return Standard_False;

  return myLength > theLength;
}

//=================================================================================================

Standard_Boolean TCollection_AsciiString::StartsWith(const Standard_CString theStartString,
                                                     const Standard_Integer theStartLength) const
{
  if (theStartLength > myLength)
    return Standard_False;

  if (theStartLength == 0)
    return Standard_True;

  return memcmp(myString, theStartString, theStartLength) == 0;
}

//=================================================================================================

Standard_Boolean TCollection_AsciiString::EndsWith(const Standard_CString theEndString,
                                                   const Standard_Integer theEndLength) const
{
  if (theEndLength > myLength)
    return Standard_False;

  if (theEndLength == 0)
    return Standard_True;

  return memcmp(myString + myLength - theEndLength, theEndString, theEndLength) == 0;
}

//=================================================================================================

Standard_Integer TCollection_AsciiString::IntegerValue() const
{
  char*            ptr;
  Standard_Integer value = (Standard_Integer)strtol(myString, &ptr, 10);
  if (ptr != myString)
    return value;

  throw Standard_NumericError("TCollection_AsciiString::IntegerValue");
}

//=================================================================================================

Standard_Boolean TCollection_AsciiString::IsIntegerValue() const
{
  char* ptr;
  strtol(myString, &ptr, 10);

  if (ptr != myString)
  {
    for (int i = int(ptr - myString); i < myLength; i++)
    {
      if (myString[i] == '.')
        return Standard_False; // what about 'e','x',etc ???
    }
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean TCollection_AsciiString::IsRealValue(Standard_Boolean theToCheckFull) const
{
  char* ptr;
  Strtod(myString, &ptr);
  if (theToCheckFull)
  {
    return (ptr[0] == '\0');
  }
  else
  {
    return (ptr != myString);
  }
}

//=================================================================================================

Standard_Boolean TCollection_AsciiString::IsAscii() const
{
  // LD : Debuggee le 26/11/98
  //      Cette fonction retournait TOUJOURS Standard_True !
  for (int i = 0; i < myLength; i++)
    if (myString[i] >= 127 || myString[i] < ' ')
      return Standard_False;
  return Standard_True;
}

//=================================================================================================

void TCollection_AsciiString::LeftAdjust()
{
  Standard_Integer i;
  for (i = 0; i < myLength; i++)
    if (!IsSpace(myString[i]))
      break;
  if (i > 0)
    Remove(1, i);
}

//=================================================================================================

void TCollection_AsciiString::LeftJustify(const Standard_Integer   theWidth,
                                          const Standard_Character theFiller)
{
  if (theWidth > myLength)
  {
    const int anOldLength = myLength;
    reallocate(theWidth);
    for (int i = anOldLength; i < theWidth; i++)
      myString[i] = theFiller;
  }
  else if (theWidth < 0)
  {
    throw Standard_NegativeValue();
  }
}

//=================================================================================================

Standard_Integer TCollection_AsciiString::Location(const Standard_Integer   theN,
                                                   const Standard_Character theC,
                                                   const Standard_Integer   theFromIndex,
                                                   const Standard_Integer   theToIndex) const
{
  if (theFromIndex > 0 && theToIndex <= myLength && theFromIndex <= theToIndex)
  {
    for (int i = theFromIndex - 1, aCount = 0; i <= theToIndex - 1; i++)
    {
      if (myString[i] == theC)
      {
        aCount++;
        if (aCount == theN)
          return i + 1;
      }
    }
    return 0;
  }
  throw Standard_OutOfRange();
}

//=================================================================================================

Standard_Integer TCollection_AsciiString::Location(const TCollection_AsciiString& theWhat,
                                                   const Standard_Integer         theFromIndex,
                                                   const Standard_Integer         theToIndex) const
{
  if (myLength == 0 || theWhat.myLength == 0)
    return 0;
  if (theToIndex <= myLength && theFromIndex > 0 && theFromIndex <= theToIndex)
  {
    Standard_Integer i     = theFromIndex - 1;
    Standard_Integer k     = 1;
    Standard_Integer l     = theFromIndex - 2;
    Standard_Boolean aFind = Standard_False;
    while (!aFind && i < theToIndex)
    {
      if (myString[i] == theWhat.Value(k))
      {
        k++;
        if (k > theWhat.myLength)
          aFind = Standard_True;
      }
      else
      {
        if (k > 1)
          i--; // si on est en cours de recherche
        k = 1;
        l = i;
      }
      i++;
    }
    if (aFind)
      return l + 2;
    else
      return 0;
  }
  throw Standard_OutOfRange();
}

//=================================================================================================

void TCollection_AsciiString::LowerCase()
{
  for (int i = 0; i < myLength; i++)
    myString[i] = ::LowerCase(myString[i]);
}

//=================================================================================================

void TCollection_AsciiString::Prepend(const TCollection_AsciiString& theWhat)
{
  Insert(1, theWhat);
}

//=================================================================================================

Standard_Real TCollection_AsciiString::RealValue() const
{
  char*         ptr;
  Standard_Real value = Strtod(myString, &ptr);
  if (ptr != myString)
    return value;

  throw Standard_NumericError("TCollection_AsciiString::RealValue");
}

//=================================================================================================

void TCollection_AsciiString::Read(Standard_IStream& theStream)
{
  // get characters from theStream
  const Standard_Integer bufSize = 8190;
  Standard_Character     buffer[bufSize];
  std::streamsize        oldWidth = theStream.width(bufSize);
  theStream >> buffer;
  theStream.width(oldWidth);

  // put to string
  reallocate(Standard_Integer(strlen(buffer)));
  memcpy(myString, buffer, myLength);
}

//---------------------------------------------------------------------------
Standard_IStream& operator>>(Standard_IStream& astream, TCollection_AsciiString& astring)
{
  astring.Read(astream);
  return astream;
}

//=================================================================================================

void TCollection_AsciiString::Print(Standard_OStream& theStream) const
{
  if (myString)
    theStream << myString;
}

// ----------------------------------------------------------------------------
Standard_OStream& operator<<(Standard_OStream& astream, const TCollection_AsciiString& astring)
{
  astring.Print(astream);
  return astream;
}

//=================================================================================================

void TCollection_AsciiString::RemoveAll(const Standard_Character theWhat,
                                        const Standard_Boolean   theCaseSensitive)
{
  if (myLength == 0)
  {
    return;
  }
  const Standard_Character aTargetChar = theCaseSensitive ? theWhat : ::UpperCase(theWhat);
  int                      aNewLength  = 0;
  for (int i = 0; i < myLength; ++i)
  {
    const Standard_Character aCurrentChar =
      theCaseSensitive ? myString[i] : ::UpperCase(myString[i]);
    if (aCurrentChar != aTargetChar)
    {
      myString[aNewLength++] = myString[i];
    }
  }
  myLength           = aNewLength;
  myString[myLength] = '\0';
}

//=================================================================================================

void TCollection_AsciiString::RemoveAll(const Standard_Character theWhat)
{
  RemoveAll(theWhat, Standard_True);
}

//=================================================================================================

void TCollection_AsciiString::Remove(const Standard_Integer theWhere,
                                     const Standard_Integer theHowMany)
{
  if (theWhere + theHowMany <= myLength + 1)
  {
    int i, j;
    for (i = theWhere + theHowMany - 1, j = theWhere - 1; i < myLength; i++, j++)
      myString[j] = myString[i];
    myLength -= theHowMany;
    myString[myLength] = '\0';
  }
  else
  {
    throw Standard_OutOfRange("TCollection_AsciiString::Remove: "
                              "Too many characters to erase or invalid "
                              "starting value.");
  }
}

//=================================================================================================

void TCollection_AsciiString::RightAdjust()
{
  Standard_Integer i;
  for (i = myLength - 1; i >= 0; i--)
    if (!IsSpace(myString[i]))
      break;
  if (i < myLength - 1)
    Remove(i + 2, myLength - (i + 2) + 1);
}

//=================================================================================================

void TCollection_AsciiString::RightJustify(const Standard_Integer   theWidth,
                                           const Standard_Character theFiller)
{
  if (theWidth > myLength)
  {
    const int anOldLength = myLength;
    reallocate(theWidth);
    int i, k;
    for (i = anOldLength - 1, k = theWidth - 1; i >= 0; i--, k--)
      myString[k] = myString[i];
    for (; k >= 0; k--)
      myString[k] = theFiller;
  }
  else if (theWidth < 0)
  {
    throw Standard_NegativeValue();
  }
}

//=================================================================================================

Standard_Integer TCollection_AsciiString::Search(const Standard_CString theWhat,
                                                 const Standard_Integer theWhatLength) const
{
  if (theWhatLength == 0)
    return -1;

  for (int i = 0; i <= myLength - theWhatLength; ++i)
  {
    if (memcmp(myString + i, theWhat, theWhatLength) == 0)
    {
      return i + 1; // Return 1-based position
    }
  }
  return -1;
}

//=================================================================================================

Standard_Integer TCollection_AsciiString::SearchFromEnd(const Standard_CString theWhat,
                                                        const Standard_Integer theWhatLength) const
{
  if (theWhatLength == 0)
    return -1;

  for (int i = myLength - theWhatLength; i >= 0; --i)
  {
    if (memcmp(myString + i, theWhat, theWhatLength) == 0)
    {
      return i + 1; // Return 1-based position
    }
  }
  return -1;
}

//=================================================================================================

void TCollection_AsciiString::SetValue(const Standard_Integer   theWhere,
                                       const Standard_Character theWhat)
{
  if (theWhere <= 0 || theWhere > myLength)
  {
    throw Standard_OutOfRange("TCollection_AsciiString::SetValue(): out of range location");
  }
  else if (theWhat == '\0')
  {
    throw Standard_OutOfRange("TCollection_AsciiString::SetValue(): NULL terminator is passed");
  }
  myString[theWhere - 1] = theWhat;
}

//=================================================================================================

void TCollection_AsciiString::SetValue(const Standard_Integer theWhere,
                                       const Standard_CString theString,
                                       const Standard_Integer theLength)
{
  if (theWhere <= 0)
    throw Standard_OutOfRange("TCollection_AsciiString::SetValue");

  const int anInsertSize = theLength;
  const int aNewLength   = theWhere - 1 + anInsertSize;

  if (aNewLength > myLength)
  {
    reallocate(aNewLength);
  }

  memcpy(myString + theWhere - 1, theString, anInsertSize);
}

//=================================================================================================

TCollection_AsciiString TCollection_AsciiString::Split(const Standard_Integer theWhere)
{
  if (theWhere >= 0 && theWhere <= myLength)
  {
    TCollection_AsciiString res(&myString[theWhere], myLength - theWhere);
    Trunc(theWhere);
    return res;
  }
  throw Standard_OutOfRange("TCollection_AsciiString::Split index");
}

//=================================================================================================

TCollection_AsciiString TCollection_AsciiString::SubString(const Standard_Integer theFromIndex,
                                                           const Standard_Integer theToIndex) const
{
  // note the we are doing here weird casts just to suppress annoying and meaningless warning
  // -Wstrict-overflow
  Standard_OutOfRange_Always_Raise_if(theFromIndex <= 0 || theToIndex <= 0
                                        || (unsigned int)theToIndex > (unsigned int)myLength
                                        || (unsigned int)theFromIndex > (unsigned int)theToIndex,
                                      "TCollection_AsciiString::SubString() out of range");
  return TCollection_AsciiString(&myString[theFromIndex - 1], theToIndex - theFromIndex + 1);
}

//=================================================================================================

TCollection_AsciiString TCollection_AsciiString::Token(const Standard_CString theSeparators,
                                                       const Standard_Integer theWhichOne) const
{
  if (!theSeparators)
    throw Standard_NullObject("TCollection_AsciiString::Token : "
                              "parameter 'theSeparators'");

  Standard_Integer anIndex;
  Standard_Integer aStringIndex = 0;
  Standard_Integer aSeparatorIndex;
  Standard_Integer aBeginIndex = 0;
  Standard_Integer anEndIndex  = 0;

  //  std::cout << "'" << myString <<  "'" << std::endl ;
  for (anIndex = 0; anIndex < theWhichOne; anIndex++)
  {
    aBeginIndex = 0;
    anEndIndex  = 0;
    //     std::cout << "anIndex " << anIndex << std::endl ;
    if (aStringIndex == myLength)
      break;
    for (; aStringIndex < myLength && anEndIndex == 0; aStringIndex++)
    {
      aSeparatorIndex = 0;
      //        std::cout << "aStringIndex " << aStringIndex << std::endl ;
      while (theSeparators[aSeparatorIndex])
      {
        if (myString[aStringIndex] == theSeparators[aSeparatorIndex])
        {
          break;
        }
        aSeparatorIndex += 1;
      }
      if (theSeparators[aSeparatorIndex] != '\0')
      { // We have a Separator
        if (aBeginIndex && anEndIndex == 0)
        {
          anEndIndex = aStringIndex;
          //            std::cout << "anEndIndex " << anEndIndex << " '" << SubString( aBeginIndex ,
          //            anEndIndex ).ToCString() << "'" << std::endl ;
          break;
        }
      }
      else if (aBeginIndex == 0)
      { // We have not a Separator
        aBeginIndex = aStringIndex + 1;
        //          std::cout << "aBeginIndex " << aBeginIndex << std::endl ;
      }
    }
    //     std::cout << "aBeginIndex " << aBeginIndex << " anEndIndex " << anEndIndex << std::endl ;
  }
  if (aBeginIndex == 0)
    return TCollection_AsciiString("", 0);
  if (anEndIndex == 0)
    anEndIndex = myLength;
  //    std::cout << "'" << SubString( aBeginIndex , anEndIndex ).ToCString() << "'" << std::endl ;
  return TCollection_AsciiString(&myString[aBeginIndex - 1], anEndIndex - aBeginIndex + 1);
}

//=================================================================================================

void TCollection_AsciiString::Trunc(const Standard_Integer theHowMany)
{
  if (theHowMany < 0 || theHowMany > myLength)
    throw Standard_OutOfRange("TCollection_AsciiString::Trunc : "
                              "parameter 'theHowMany'");
  myLength           = theHowMany;
  myString[myLength] = '\0';
}

//=================================================================================================

void TCollection_AsciiString::UpperCase()
{
  for (int i = 0; i < myLength; i++)
    myString[i] = ::UpperCase(myString[i]);
}

//=================================================================================================

Standard_Integer TCollection_AsciiString::UsefullLength() const
{
  Standard_Integer i;
  for (i = myLength - 1; i >= 0; i--)
    if (IsGraphic(myString[i]))
      break;
  return i + 1;
}

//=================================================================================================

Standard_Character TCollection_AsciiString::Value(const Standard_Integer theWhere) const
{
  if (theWhere > 0 && theWhere <= myLength)
  {
    return myString[theWhere - 1];
  }
  throw Standard_OutOfRange("TCollection_AsciiString::Value : parameter theWhere");
}

//=================================================================================================

void TCollection_AsciiString::allocate(const int theLength)
{
  myLength = theLength;
  if (theLength == 0)
  {
    myString = THE_DEFAULT_CHAR_STRING;
  }
  else
  {
    const Standard_Size aRoundSize = calculatePaddedSize(theLength);
    myString           = static_cast<Standard_PCharacter>(Standard::AllocateOptimal(aRoundSize));
    myString[myLength] = '\0';
  }
}

//=================================================================================================

void TCollection_AsciiString::reallocate(const int theLength)
{
  if (theLength != 0)
  {
    if (myString == THE_DEFAULT_CHAR_STRING)
    {
      // Use same padding strategy as allocate() for consistency
      const Standard_Size aRoundSize = calculatePaddedSize(theLength);
      myString = static_cast<Standard_PCharacter>(Standard::AllocateOptimal(aRoundSize));
    }
    else
    {
      // For existing allocations, use padding size with Standard::Reallocate
      const Standard_Size aRoundSize = calculatePaddedSize(theLength);
      myString = static_cast<Standard_PCharacter>(Standard::Reallocate(myString, aRoundSize));
    }
  }
  // Ensure null termination at new length
  if (myString != THE_DEFAULT_CHAR_STRING)
  {
    myString[theLength] = '\0';
  }
  myLength = theLength;
}

//=================================================================================================

void TCollection_AsciiString::deallocate()
{
  if (myString != THE_DEFAULT_CHAR_STRING)
  {
    Standard::Free(myString);
  }
  myLength = 0;
  myString = THE_DEFAULT_CHAR_STRING;
}

//=================================================================================================

Standard_Boolean TCollection_AsciiString::IsSameString(const Standard_CString theString1,
                                                       const Standard_Integer theLength1,
                                                       const Standard_CString theString2,
                                                       const Standard_Integer theLength2,
                                                       const Standard_Boolean theIsCaseSensitive)
{
  if (theLength1 != theLength2)
    return Standard_False;

  if (theLength1 == 0)
    return Standard_True;

  if (theIsCaseSensitive)
  {
    return memcmp(theString1, theString2, theLength1) == 0;
  }
  else
  {
    for (Standard_Integer anIndex = 0; anIndex < theLength1; ++anIndex)
    {
      if (::UpperCase(theString1[anIndex]) != ::UpperCase(theString2[anIndex]))
        return Standard_False;
    }
    return Standard_True;
  }
}

//=================================================================================================

const TCollection_AsciiString& TCollection_AsciiString::EmptyString() noexcept
{
  static const TCollection_AsciiString THE_EMPTY_STRING;
  return THE_EMPTY_STRING;
}
