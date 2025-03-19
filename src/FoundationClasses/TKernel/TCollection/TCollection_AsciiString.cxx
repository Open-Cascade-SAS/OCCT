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

namespace
{
static char THE_DEFAULT_CHAR_STRING[1] = {'\0'};
}

// ----------------------------------------------------------------------------
// Create an empty AsciiString
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString()
{
  allocate(0);
}

// ----------------------------------------------------------------------------
// Create an asciistring from a Standard_CString
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_CString theString)
{
  if (theString == NULL)
  {
    throw Standard_NullObject("TCollection_AsciiString(): NULL pointer passed to constructor");
  }
  allocate(static_cast<int>(strlen(theString)));
  memcpy(mystring, theString, mylength);
}

// ----------------------------------------------------------------------------
// Create an asciistring from a Standard_CString
// ----------------------------------------------------------------------------
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
  memcpy(mystring, theString, mylength);
}

// ----------------------------------------------------------------------------
// Create an asciistring from a Standard_Character
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_Character aChar)
{
  if (aChar != '\0')
  {
    allocate(1);
    mystring[0] = aChar;
  }
  else
  {
    allocate(0);
  }
}

// ----------------------------------------------------------------------------
// Create an AsciiString from a filler
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_Integer   length,
                                                 const Standard_Character filler)
{
  allocate(length);
  memset(mystring, filler, length);
}

// ----------------------------------------------------------------------------
// Create an AsciiString from an Integer
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_Integer aValue)
{
  char t[13];
  allocate(Sprintf(t, "%d", aValue));
  memcpy(mystring, t, mylength);
}

// ----------------------------------------------------------------------------
// Create an asciistring from a real
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_Real aValue)
{
  char t[50];
  allocate(Sprintf(t, "%g", aValue));
  memcpy(mystring, t, mylength);
}

// ----------------------------------------------------------------------------
// Create an asciistring from an asciistring
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const TCollection_AsciiString& theString)
{
  allocate(theString.mylength);
  if (mylength != 0)
  {
    memcpy(mystring, theString.mystring, mylength);
  }
}

//=================================================================================================

TCollection_AsciiString::TCollection_AsciiString(TCollection_AsciiString&& theOther)
  Standard_Noexcept
{
  if (theOther.mystring == THE_DEFAULT_CHAR_STRING)
  {
    allocate(0);
  }
  else
  {
    mystring = theOther.mystring;
    mylength = theOther.mylength;
  }
  theOther.mylength = 0;
  theOther.mystring = THE_DEFAULT_CHAR_STRING;
}

// ----------------------------------------------------------------------------
// Create an asciistring from a character
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const TCollection_AsciiString& theString,
                                                 const Standard_Character       theChar)
{
  allocate(theString.mylength + 1);
  if (theString.mylength != 0)
  {
    memcpy(mystring, theString.mystring, theString.mylength);
  }
  mystring[mylength - 1] = theChar;
}

// ----------------------------------------------------------------------------
// Create an asciistring from an asciistring
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const TCollection_AsciiString& theString1,
                                                 const Standard_CString         theString2)
{
  const Standard_Integer aStr2Len = Standard_Integer(theString2 ? strlen(theString2) : 0);
  allocate(theString1.mylength + aStr2Len);
  if (theString1.mylength != 0)
  {
    memcpy(mystring, theString1.mystring, theString1.mylength);
  }
  if (aStr2Len != 0)
  {
    memcpy(mystring + theString1.mylength, theString2, aStr2Len);
  }
}

// ----------------------------------------------------------------------------
// Create an asciistring from an asciistring
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const TCollection_AsciiString& theString1,
                                                 const TCollection_AsciiString& theString2)
{
  allocate(theString1.mylength + theString2.mylength);
  if (theString1.mylength)
  {
    memcpy(mystring, theString1.mystring, theString1.mylength);
  }
  if (theString2.mylength != 0)
  {
    memcpy(mystring + theString1.mylength, theString2.mystring, theString2.mylength);
  }
}

//---------------------------------------------------------------------------
//  Create an asciistring from an ExtendedString
//---------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const TCollection_ExtendedString& astring,
                                                 const Standard_Character          replaceNonAscii)
{
  if (replaceNonAscii)
  {
    allocate(astring.Length());
    for (int i = 0; i < mylength; i++)
    {
      Standard_ExtCharacter c = astring.Value(i + 1);
      mystring[i]             = (IsAnAscii(c) ? ToCharacter(c) : replaceNonAscii);
    }
  }
  else
  {
    // create UTF-8 string
    allocate(astring.LengthOfCString());
    astring.ToUTF8CString(mystring);
  }
}

//---------------------------------------------------------------------------
//  Create an TCollection_AsciiString from a Standard_WideChar
//---------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_WideChar* theStringUtf)
{
  int aLength = 0;
  for (NCollection_UtfWideIter anIter(theStringUtf); *anIter != 0; ++anIter)
  {
    aLength += anIter.AdvanceBytesUtf8();
  }
  allocate(aLength);
  NCollection_UtfWideIter anIterRead(theStringUtf);
  for (Standard_Utf8Char* anIterWrite = mystring; *anIterRead != 0; ++anIterRead)
  {
    anIterWrite = anIterRead.GetUtf(anIterWrite);
  }
}

// ----------------------------------------------------------------------------
// AssignCat
// ----------------------------------------------------------------------------
void TCollection_AsciiString::AssignCat(const Standard_Integer other)
{

  AssignCat(TCollection_AsciiString(other));
}

// ----------------------------------------------------------------------------
// AssignCat
// ----------------------------------------------------------------------------
void TCollection_AsciiString::AssignCat(const Standard_Real other)
{

  AssignCat(TCollection_AsciiString(other));
}

// ----------------------------------------------------------------------------
// AssignCat
// ----------------------------------------------------------------------------
void TCollection_AsciiString::AssignCat(const Standard_Character other)
{
  if (other != '\0')
  {
    reallocate(mylength + 1);
    mystring[mylength - 1] = other;
  }
}

// ----------------------------------------------------------------------------
// AssignCat
// ----------------------------------------------------------------------------
void TCollection_AsciiString::AssignCat(const Standard_CString theOther)
{
  if (theOther == NULL)
  {
    throw Standard_NullObject("TCollection_AsciiString::Operator += parameter other");
  }

  Standard_Integer anOtherLen = Standard_Integer(strlen(theOther));
  if (anOtherLen != 0)
  {
    const Standard_Integer anOldLength = mylength;
    reallocate(mylength + anOtherLen);
    memcpy(mystring + anOldLength, theOther, anOtherLen + 1);
  }
}

// ----------------------------------------------------------------------------
// AssignCat
// ----------------------------------------------------------------------------
void TCollection_AsciiString::AssignCat(const TCollection_AsciiString& theOther)
{
  if (theOther.mylength != 0)
  {
    const Standard_Integer anOldLength = mylength;
    reallocate(mylength + theOther.mylength);
    memcpy(mystring + anOldLength, theOther.mystring, theOther.mylength + 1);
  }
}

// ---------------------------------------------------------------------------
// Capitalize
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Capitalize()
{
  if (mylength)
    mystring[0] = ::UpperCase(mystring[0]);
  for (int i = 1; i < mylength; i++)
    mystring[i] = ::LowerCase(mystring[i]);
}

// ---------------------------------------------------------------------------
// Center
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Center(const Standard_Integer Width, const Standard_Character Filler)
{
  if (Width > mylength)
  {
    Standard_Integer newlength = mylength + ((Width - mylength) / 2);
    LeftJustify(newlength, Filler);
    RightJustify(Width, Filler);
  }
  else if (Width < 0)
  {
    throw Standard_NegativeValue();
  }
}

// ----------------------------------------------------------------------------
// ChangeAll
// ----------------------------------------------------------------------------
void TCollection_AsciiString::ChangeAll(const Standard_Character aChar,
                                        const Standard_Character NewChar,
                                        const Standard_Boolean   CaseSensitive)
{
  if (CaseSensitive)
  {
    for (int i = 0; i < mylength; i++)
      if (mystring[i] == aChar)
        mystring[i] = NewChar;
  }
  else
  {
    Standard_Character anUpperChar = ::UpperCase(aChar);
    for (int i = 0; i < mylength; i++)
      if (::UpperCase(mystring[i]) == anUpperChar)
        mystring[i] = NewChar;
  }
}

// ----------------------------------------------------------------------------
// Clear
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Clear()
{
  deallocate();
}

// ----------------------------------------------------------------------------
// Copy
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Copy(const Standard_CString fromwhere)
{
  if (fromwhere == mystring)
  {
    return;
  }
  if (fromwhere && fromwhere[0] != '\0')
  {
    reallocate(static_cast<int>(strlen(fromwhere)));
    memcpy(mystring, fromwhere, mylength);
  }
  else
  {
    mylength = 0;
    mystring = THE_DEFAULT_CHAR_STRING;
  }
}

// ----------------------------------------------------------------------------
// Copy
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Copy(const TCollection_AsciiString& fromwhere)
{
  if (&fromwhere == this)
  {
    return;
  }
  if (fromwhere.mystring && fromwhere.mylength > 0)
  {
    reallocate(fromwhere.mylength);
    memcpy(mystring, fromwhere.mystring, mylength);
  }
  else if (mystring != THE_DEFAULT_CHAR_STRING)
  {
    mylength           = 0;
    mystring[mylength] = '\0';
  }
}

//=================================================================================================

void TCollection_AsciiString::Move(TCollection_AsciiString&& theOther)
{
  if (&theOther == this)
  {
    return;
  }
  if (mystring != THE_DEFAULT_CHAR_STRING)
  {
    Standard::Free(mystring);
  }
  mystring          = theOther.mystring;
  mylength          = theOther.mylength;
  theOther.mystring = THE_DEFAULT_CHAR_STRING;
  theOther.mylength = 0;
}

// ----------------------------------------------------------------------------
// Swap
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Swap(TCollection_AsciiString& theOther)
{
  if (&theOther == this)
  {
    return;
  }
  std::swap(mystring, theOther.mystring);
  std::swap(mylength, theOther.mylength);
}

// ----------------------------------------------------------------------------
// Destroy
// ----------------------------------------------------------------------------
TCollection_AsciiString::~TCollection_AsciiString()
{
  deallocate();
}

// ----------------------------------------------------------------------------
// FirstLocationInSet
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::FirstLocationInSet(const TCollection_AsciiString& Set,
                                                             const Standard_Integer FromIndex,
                                                             const Standard_Integer ToIndex) const
{
  if (mylength == 0 || Set.mylength == 0)
    return 0;
  if (FromIndex > 0 && ToIndex <= mylength && FromIndex <= ToIndex)
  {
    for (int i = FromIndex - 1; i < ToIndex; i++)
      for (int j = 0; j < Set.mylength; j++)
        if (mystring[i] == Set.mystring[j])
          return i + 1;
    return 0;
  }
  throw Standard_OutOfRange();
}

// ----------------------------------------------------------------------------
// FirstLocationNotInSet
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::FirstLocationNotInSet(
  const TCollection_AsciiString& Set,
  const Standard_Integer         FromIndex,
  const Standard_Integer         ToIndex) const
{
  if (mylength == 0 || Set.mylength == 0)
    return 0;
  if (FromIndex > 0 && ToIndex <= mylength && FromIndex <= ToIndex)
  {
    Standard_Boolean find;
    for (int i = FromIndex - 1; i < ToIndex; i++)
    {
      find = Standard_False;
      for (int j = 0; j < Set.mylength; j++)
        if (mystring[i] == Set.mystring[j])
          find = Standard_True;
      if (!find)
        return i + 1;
    }
    return 0;
  }
  throw Standard_OutOfRange();
}

//----------------------------------------------------------------------------
// Insert a character before 'where'th character
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Insert(const Standard_Integer where, const Standard_Character what)
{
  if (where > mylength + 1)
    throw Standard_OutOfRange("TCollection_AsciiString::Insert : Parameter where is too big");
  if (where < 1)
    throw Standard_OutOfRange("TCollection_AsciiString::Insert : Parameter where is too small");

  const int anOldLength = mylength;
  reallocate(mylength + 1);
  if (where != anOldLength + 1)
  {
    for (int i = anOldLength - 1; i >= where - 1; i--)
      mystring[i + 1] = mystring[i];
  }
  mystring[where - 1] = what;
}

// ----------------------------------------------------------------------------
// Insert
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Insert(const Standard_Integer where, const Standard_CString what)
{
  if (where <= mylength + 1 && where > 0)
  {
    if (what)
    {
      const Standard_Integer whatlength  = Standard_Integer(strlen(what));
      const int              anOldLength = mylength;
      reallocate(mylength + whatlength);
      if (where != anOldLength + 1)
      {
        for (int i = anOldLength - 1; i >= where - 1; i--)
          mystring[i + whatlength] = mystring[i];
      }
      for (int i = 0; i < whatlength; i++)
        mystring[where - 1 + i] = what[i];
    }
  }
  else
  {
    throw Standard_OutOfRange("TCollection_AsciiString::Insert : "
                              "Parameter where is invalid");
  }
}

// ----------------------------------------------------------------------------
// Insert
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Insert(const Standard_Integer         where,
                                     const TCollection_AsciiString& what)
{
  Standard_CString swhat = what.mystring;
  if (where <= mylength + 1)
  {
    const Standard_Integer whatlength = what.mylength;
    if (whatlength)
    {
      const int anOldLength = mylength;
      reallocate(mylength + whatlength);

      if (where != anOldLength + 1)
      {
        for (int i = anOldLength - 1; i >= where - 1; i--)
          mystring[i + whatlength] = mystring[i];
      }
      for (int i = 0; i < whatlength; i++)
        mystring[where - 1 + i] = swhat[i];
    }
  }
  else
  {
    throw Standard_OutOfRange("TCollection_AsciiString::Insert : "
                              "Parameter where is too big");
  }
}

//------------------------------------------------------------------------
//  InsertAfter
//------------------------------------------------------------------------
void TCollection_AsciiString::InsertAfter(const Standard_Integer         Index,
                                          const TCollection_AsciiString& what)
{
  if (Index < 0 || Index > mylength)
    throw Standard_OutOfRange();
  Insert(Index + 1, what);
}

//------------------------------------------------------------------------
//  InsertBefore
//------------------------------------------------------------------------
void TCollection_AsciiString::InsertBefore(const Standard_Integer         Index,
                                           const TCollection_AsciiString& what)
{
  if (Index < 1 || Index > mylength)
    throw Standard_OutOfRange();
  Insert(Index, what);
}

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsEqual(const Standard_CString other) const
{
  if (other)
  {
    return (strncmp(other, mystring, mylength + 1) == 0);
  }
  throw Standard_NullObject("TCollection_AsciiString::Operator == "
                            "Parameter 'other'");
}

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsEqual(const TCollection_AsciiString& other) const
{
  if (mylength != other.mylength)
    return Standard_False;
  return (strncmp(other.mystring, mystring, mylength) == 0);
}

// ----------------------------------------------------------------------------
// IsSameString
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsSameString(const TCollection_AsciiString& theString1,
                                                       const TCollection_AsciiString& theString2,
                                                       const Standard_Boolean theIsCaseSensitive)
{
  const Standard_Integer aSize1 = theString1.Length();
  if (aSize1 != theString2.Length())
  {
    return Standard_False;
  }

  if (theIsCaseSensitive)
  {
    return (strncmp(theString1.ToCString(), theString2.ToCString(), aSize1) == 0);
  }

  for (Standard_Integer aCharIter = 1; aCharIter <= aSize1; ++aCharIter)
  {
    if (toupper(theString1.Value(aCharIter)) != toupper(theString2.Value(aCharIter)))
    {
      return Standard_False;
    }
  }
  return Standard_True;
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsDifferent(const Standard_CString other) const
{
  if (other)
  {
    return (strncmp(other, mystring, mylength + 1) != 0);
  }
  throw Standard_NullObject("TCollection_AsciiString::Operator != "
                            "Parameter 'other'");
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsDifferent(const TCollection_AsciiString& other) const
{

  if (mylength != other.mylength)
    return Standard_True;
  return (strncmp(other.mystring, mystring, mylength) != 0);
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsLess(const Standard_CString other) const
{
  if (other)
  {
    return (strncmp(mystring, other, mylength + 1) < 0);
  }
  throw Standard_NullObject("TCollection_AsciiString::Operator < "
                            "Parameter 'other'");
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsLess(const TCollection_AsciiString& other) const
{
  return (strncmp(mystring, other.mystring, mylength + 1) < 0);
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsGreater(const Standard_CString other) const
{
  if (other)
  {
    return (strncmp(mystring, other, mylength + 1) > 0);
  }
  throw Standard_NullObject("TCollection_AsciiString::Operator > "
                            "Parameter 'other'");
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsGreater(const TCollection_AsciiString& other) const
{
  return (strncmp(mystring, other.mystring, mylength + 1) > 0);
}

// ----------------------------------------------------------------------------
// StartsWith
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::StartsWith(
  const TCollection_AsciiString& theStartString) const
{
  if (this == &theStartString)
  {
    return true;
  }

  return mylength >= theStartString.mylength
         && strncmp(theStartString.mystring, mystring, theStartString.mylength) == 0;
}

// ----------------------------------------------------------------------------
// EndsWith
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::EndsWith(
  const TCollection_AsciiString& theEndString) const
{
  if (this == &theEndString)
  {
    return true;
  }

  return mylength >= theEndString.mylength
         && strncmp(theEndString.mystring,
                    mystring + mylength - theEndString.mylength,
                    theEndString.mylength)
              == 0;
}

// ----------------------------------------------------------------------------
// IntegerValue
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::IntegerValue() const
{
  char*            ptr;
  Standard_Integer value = (Standard_Integer)strtol(mystring, &ptr, 10);
  if (ptr != mystring)
    return value;

  throw Standard_NumericError("TCollection_AsciiString::IntegerValue");
}

// ----------------------------------------------------------------------------
// IsIntegerValue
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsIntegerValue() const
{
  char* ptr;
  strtol(mystring, &ptr, 10);

  if (ptr != mystring)
  {
    for (int i = int(ptr - mystring); i < mylength; i++)
    {
      if (mystring[i] == '.')
        return Standard_False; // what about 'e','x',etc ???
    }
    return Standard_True;
  }
  return Standard_False;
}

// ----------------------------------------------------------------------------
// IsRealValue
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsRealValue(Standard_Boolean theToCheckFull) const
{
  char* ptr;
  Strtod(mystring, &ptr);
  if (theToCheckFull)
  {
    return (ptr[0] == '\0');
  }
  else
  {
    return (ptr != mystring);
  }
}

// ----------------------------------------------------------------------------
// IsAscii
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsAscii() const
{
  // LD : Debuggee le 26/11/98
  //      Cette fonction retournait TOUJOURS Standard_True !
  for (int i = 0; i < mylength; i++)
    if (mystring[i] >= 127 || mystring[i] < ' ')
      return Standard_False;
  return Standard_True;
}

//------------------------------------------------------------------------
//  LeftAdjust
//------------------------------------------------------------------------
void TCollection_AsciiString::LeftAdjust()
{
  Standard_Integer i;
  for (i = 0; i < mylength; i++)
    if (!IsSpace(mystring[i]))
      break;
  if (i > 0)
    Remove(1, i);
}

//------------------------------------------------------------------------
//  LeftJustify
//------------------------------------------------------------------------
void TCollection_AsciiString::LeftJustify(const Standard_Integer   Width,
                                          const Standard_Character Filler)
{
  if (Width > mylength)
  {
    const int anOldLength = mylength;
    reallocate(Width);
    for (int i = anOldLength; i < Width; i++)
      mystring[i] = Filler;
  }
  else if (Width < 0)
  {
    throw Standard_NegativeValue();
  }
}

//------------------------------------------------------------------------
//  Location
//------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::Location(const Standard_Integer   N,
                                                   const Standard_Character C,
                                                   const Standard_Integer   FromIndex,
                                                   const Standard_Integer   ToIndex) const
{
  if (FromIndex > 0 && ToIndex <= mylength && FromIndex <= ToIndex)
  {
    for (int i = FromIndex - 1, count = 0; i <= ToIndex - 1; i++)
    {
      if (mystring[i] == C)
      {
        count++;
        if (count == N)
          return i + 1;
      }
    }
    return 0;
  }
  throw Standard_OutOfRange();
}

//------------------------------------------------------------------------
//  Location
//------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::Location(const TCollection_AsciiString& what,
                                                   const Standard_Integer         FromIndex,
                                                   const Standard_Integer         ToIndex) const
{
  if (mylength == 0 || what.mylength == 0)
    return 0;
  if (ToIndex <= mylength && FromIndex > 0 && FromIndex <= ToIndex)
  {
    Standard_Integer i    = FromIndex - 1;
    Standard_Integer k    = 1;
    Standard_Integer l    = FromIndex - 2;
    Standard_Boolean Find = Standard_False;
    while (!Find && i < ToIndex)
    {
      if (mystring[i] == what.Value(k))
      {
        k++;
        if (k > what.mylength)
          Find = Standard_True;
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
    if (Find)
      return l + 2;
    else
      return 0;
  }
  throw Standard_OutOfRange();
}

// ----------------------------------------------------------------------------
// LowerCase
// ----------------------------------------------------------------------------
void TCollection_AsciiString::LowerCase()
{
  for (int i = 0; i < mylength; i++)
    mystring[i] = ::LowerCase(mystring[i]);
}

//------------------------------------------------------------------------
//  Prepend
//------------------------------------------------------------------------
void TCollection_AsciiString::Prepend(const TCollection_AsciiString& what)
{
  Insert(1, what);
}

// ----------------------------------------------------------------------------
// RealValue
// ----------------------------------------------------------------------------
Standard_Real TCollection_AsciiString::RealValue() const
{
  char*         ptr;
  Standard_Real value = Strtod(mystring, &ptr);
  if (ptr != mystring)
    return value;

  throw Standard_NumericError("TCollection_AsciiString::RealValue");
}

// ----------------------------------------------------------------------------
// Read
//--------------------------------------------------------------------------
void TCollection_AsciiString::Read(Standard_IStream& astream)
{
  // get characters from astream
  const Standard_Integer bufSize = 8190;
  Standard_Character     buffer[bufSize];
  std::streamsize        oldWidth = astream.width(bufSize);
  astream >> buffer;
  astream.width(oldWidth);

  // put to string
  reallocate(Standard_Integer(strlen(buffer)));
  memcpy(mystring, buffer, mylength);
}

//---------------------------------------------------------------------------
Standard_IStream& operator>>(Standard_IStream& astream, TCollection_AsciiString& astring)
{
  astring.Read(astream);
  return astream;
}

// ----------------------------------------------------------------------------
// Print
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Print(Standard_OStream& astream) const
{
  if (mystring)
    astream << mystring;
}

// ----------------------------------------------------------------------------
Standard_OStream& operator<<(Standard_OStream& astream, const TCollection_AsciiString& astring)
{
  astring.Print(astream);
  return astream;
}

// ----------------------------------------------------------------------------
// RemoveAll
// ----------------------------------------------------------------------------
void TCollection_AsciiString::RemoveAll(const Standard_Character what,
                                        const Standard_Boolean   CaseSensitive)
{
  if (mylength == 0)
  {
    return;
  }
  const Standard_Character aTargetChar = CaseSensitive ? what : ::UpperCase(what);
  int                      aNewLength  = 0;
  for (int i = 0; i < mylength; ++i)
  {
    const Standard_Character aCurrentChar = CaseSensitive ? mystring[i] : ::UpperCase(mystring[i]);
    if (aCurrentChar != aTargetChar)
    {
      mystring[aNewLength++] = mystring[i];
    }
  }
  mylength           = aNewLength;
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// RemoveAll
// ----------------------------------------------------------------------------
void TCollection_AsciiString::RemoveAll(const Standard_Character what)
{
  RemoveAll(what, Standard_True);
}

// ----------------------------------------------------------------------------
// Remove
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Remove(const Standard_Integer where, const Standard_Integer ahowmany)
{
  if (where + ahowmany <= mylength + 1)
  {
    int i, j;
    for (i = where + ahowmany - 1, j = where - 1; i < mylength; i++, j++)
      mystring[j] = mystring[i];
    mylength -= ahowmany;
    mystring[mylength] = '\0';
  }
  else
  {
    throw Standard_OutOfRange("TCollection_AsciiString::Remove: "
                              "Too many characters to erase or invalid "
                              "starting value.");
  }
}

//------------------------------------------------------------------------
//  RightAdjust
//------------------------------------------------------------------------
void TCollection_AsciiString::RightAdjust()
{
  Standard_Integer i;
  for (i = mylength - 1; i >= 0; i--)
    if (!IsSpace(mystring[i]))
      break;
  if (i < mylength - 1)
    Remove(i + 2, mylength - (i + 2) + 1);
}

//------------------------------------------------------------------------
//  RightJustify
//------------------------------------------------------------------------
void TCollection_AsciiString::RightJustify(const Standard_Integer   Width,
                                           const Standard_Character Filler)
{
  if (Width > mylength)
  {
    const int anOldLength = mylength;
    reallocate(Width);
    int i, k;
    for (i = anOldLength - 1, k = Width - 1; i >= 0; i--, k--)
      mystring[k] = mystring[i];
    for (; k >= 0; k--)
      mystring[k] = Filler;
  }
  else if (Width < 0)
  {
    throw Standard_NegativeValue();
  }
}

// ----------------------------------------------------------------------------
// Search
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::Search(const Standard_CString what) const
{
  Standard_Integer size = Standard_Integer(what ? strlen(what) : 0);
  if (size)
  {
    int k, j;
    int i = 0;
    while (i < mylength - size + 1)
    {
      k = i++;
      j = 0;
      while (j < size && mystring[k++] == what[j++])
        if (j == size)
          return i;
    }
  }
  return -1;
}

// ----------------------------------------------------------------------------
// Search
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::Search(const TCollection_AsciiString& what) const
{
  Standard_Integer size  = what.mylength;
  Standard_CString swhat = what.mystring;
  if (size)
  {
    int k, j;
    int i = 0;
    while (i < mylength - size + 1)
    {
      k = i++;
      j = 0;
      while (j < size && mystring[k++] == swhat[j++])
        if (j == size)
          return i;
    }
  }
  return -1;
}

// ----------------------------------------------------------------------------
// SearchFromEnd
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::SearchFromEnd(const Standard_CString what) const
{
  Standard_Integer size = Standard_Integer(what ? strlen(what) : 0);
  if (size)
  {
    int k, j;
    int i = mylength - 1;
    while (i >= size - 1)
    {
      k = i--;
      j = size - 1;
      while (j >= 0 && mystring[k--] == what[j--])
        if (j == -1)
          return i - size + 3;
    }
  }
  return -1;
}

// ----------------------------------------------------------------------------
// SearchFromEnd
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::SearchFromEnd(const TCollection_AsciiString& what) const
{
  int size = what.mylength;
  if (size)
  {
    Standard_CString swhat = what.mystring;
    int              k, j;
    int              i = mylength - 1;
    while (i >= size - 1)
    {
      k = i--;
      j = size - 1;
      while (j >= 0 && mystring[k--] == swhat[j--])
        if (j == -1)
          return i - size + 3;
    }
  }
  return -1;
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_AsciiString::SetValue(const Standard_Integer   theWhere,
                                       const Standard_Character theWhat)
{
  if (theWhere <= 0 || theWhere > mylength)
  {
    throw Standard_OutOfRange("TCollection_AsciiString::SetValue(): out of range location");
  }
  else if (theWhat == '\0')
  {
    throw Standard_OutOfRange("TCollection_AsciiString::SetValue(): NULL terminator is passed");
  }
  mystring[theWhere - 1] = theWhat;
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_AsciiString::SetValue(const Standard_Integer where, const Standard_CString what)
{
  if (where > 0 && where <= mylength + 1)
  {
    Standard_Integer size = Standard_Integer(what ? strlen(what) : 0);
    size += (where - 1);
    if (size >= mylength)
    {
      reallocate(size);
    }
    for (int i = where - 1; i < size; i++)
      mystring[i] = what[i - (where - 1)];
  }
  else
  {
    throw Standard_OutOfRange("TCollection_AsciiString::SetValue : "
                              "parameter where");
  }
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_AsciiString::SetValue(const Standard_Integer         where,
                                       const TCollection_AsciiString& what)
{
  if (where > 0 && where <= mylength + 1)
  {
    Standard_Integer size  = what.mylength;
    Standard_CString swhat = what.mystring;
    size += (where - 1);
    if (size >= mylength)
    {
      reallocate(size);
    }
    for (int i = where - 1; i < size; i++)
      mystring[i] = swhat[i - (where - 1)];
  }
  else
  {
    throw Standard_OutOfRange("TCollection_AsciiString::SetValue : "
                              "parameter where");
  }
}

// ----------------------------------------------------------------------------
// Split
// ----------------------------------------------------------------------------
TCollection_AsciiString TCollection_AsciiString::Split(const Standard_Integer where)
{
  if (where >= 0 && where <= mylength)
  {
    TCollection_AsciiString res(&mystring[where], mylength - where);
    Trunc(where);
    return res;
  }
  throw Standard_OutOfRange("TCollection_AsciiString::Split index");
}

// ----------------------------------------------------------------------------
// Token
// ----------------------------------------------------------------------------
TCollection_AsciiString TCollection_AsciiString::Token(const Standard_CString separators,
                                                       const Standard_Integer whichone) const
{
  if (!separators)
    throw Standard_NullObject("TCollection_AsciiString::Token : "
                              "parameter 'separators'");

  Standard_Integer theOne;
  Standard_Integer StringIndex = 0;
  Standard_Integer SeparatorIndex;
  Standard_Integer BeginIndex = 0;
  Standard_Integer EndIndex   = 0;

  //  std::cout << "'" << mystring <<  "'" << std::endl ;
  for (theOne = 0; theOne < whichone; theOne++)
  {
    BeginIndex = 0;
    EndIndex   = 0;
    //     std::cout << "theOne " << theOne << std::endl ;
    if (StringIndex == mylength)
      break;
    for (; StringIndex < mylength && EndIndex == 0; StringIndex++)
    {
      SeparatorIndex = 0;
      //        std::cout << "StringIndex " << StringIndex << std::endl ;
      while (separators[SeparatorIndex])
      {
        if (mystring[StringIndex] == separators[SeparatorIndex])
        {
          break;
        }
        SeparatorIndex += 1;
      }
      if (separators[SeparatorIndex] != '\0')
      { // We have a Separator
        if (BeginIndex && EndIndex == 0)
        {
          EndIndex = StringIndex;
          //            std::cout << "EndIndex " << EndIndex << " '" << SubString( BeginIndex ,
          //            EndIndex ).ToCString() << "'" << std::endl ;
          break;
        }
      }
      else if (BeginIndex == 0)
      { // We have not a Separator
        BeginIndex = StringIndex + 1;
        //          std::cout << "BeginIndex " << BeginIndex << std::endl ;
      }
    }
    //     std::cout << "BeginIndex " << BeginIndex << " EndIndex " << EndIndex << std::endl ;
  }
  if (BeginIndex == 0)
    return TCollection_AsciiString("", 0);
  if (EndIndex == 0)
    EndIndex = mylength;
  //    std::cout << "'" << SubString( BeginIndex , EndIndex ).ToCString() << "'" << std::endl ;
  return TCollection_AsciiString(&mystring[BeginIndex - 1], EndIndex - BeginIndex + 1);
}

// ----------------------------------------------------------------------------
// Trunc
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Trunc(const Standard_Integer ahowmany)
{
  if (ahowmany < 0 || ahowmany > mylength)
    throw Standard_OutOfRange("TCollection_AsciiString::Trunc : "
                              "parameter 'ahowmany'");
  mylength           = ahowmany;
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// UpperCase
// ----------------------------------------------------------------------------
void TCollection_AsciiString::UpperCase()
{
  for (int i = 0; i < mylength; i++)
    mystring[i] = ::UpperCase(mystring[i]);
}

//------------------------------------------------------------------------
//  UsefullLength
//------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::UsefullLength() const
{
  Standard_Integer i;
  for (i = mylength - 1; i >= 0; i--)
    if (IsGraphic(mystring[i]))
      break;
  return i + 1;
}

// ----------------------------------------------------------------------------
// Value
// ----------------------------------------------------------------------------
Standard_Character TCollection_AsciiString::Value(const Standard_Integer where) const
{
  if (where > 0 && where <= mylength)
  {
    return mystring[where - 1];
  }
  throw Standard_OutOfRange("TCollection_AsciiString::Value : parameter where");
}

//=================================================================================================

void TCollection_AsciiString::allocate(const int theLength)
{
  mylength = theLength;
  if (theLength == 0)
  {
    mystring = THE_DEFAULT_CHAR_STRING;
  }
  else
  {
    const Standard_Size aRoundSize = (theLength + 4) & ~0x3;
    mystring           = static_cast<Standard_PCharacter>(Standard::AllocateOptimal(aRoundSize));
    mystring[mylength] = '\0';
  }
}

//=================================================================================================

void TCollection_AsciiString::reallocate(const int theLength)
{
  if (theLength != 0)
  {
    if (mystring == THE_DEFAULT_CHAR_STRING)
    {
      const Standard_Size aRoundSize = (theLength + 4) & ~0x3;
      mystring = static_cast<Standard_PCharacter>(Standard::AllocateOptimal(aRoundSize));
    }
    else
    {
      mystring = static_cast<Standard_PCharacter>(Standard::Reallocate(mystring, theLength + 1));
    }
    mystring[theLength] = '\0';
  }
  if (mystring != THE_DEFAULT_CHAR_STRING)
  {
    mystring[theLength] = '\0';
  }
  mylength = theLength;
}

//=================================================================================================

void TCollection_AsciiString::deallocate()
{
  if (mystring != THE_DEFAULT_CHAR_STRING)
  {
    Standard::Free(mystring);
  }
  mylength = 0;
  mystring = THE_DEFAULT_CHAR_STRING;
}
