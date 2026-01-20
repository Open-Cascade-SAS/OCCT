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

#include <TCollection_ExtendedString.hxx>

#include <NCollection_UtfIterator.hxx>
#include <Standard.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_AsciiString.hxx>

#include <algorithm>
#include <cstddef>

namespace
{
static char16_t THE_DEFAULT_EXT_CHAR_STRING[1] = {u'\0'};

//! Calculate padded allocation size for ExtendedString (2-byte characters)
//! Guarantees at least +1 character space for null terminator, aligned to 4-byte boundary
inline size_t calculatePaddedSize(const int theLength)
{
  return (((theLength + 1) * sizeof(char16_t)) + 3) & ~0x3;
}

//! Returns the number of 16-bit code units in Unicode string
template <typename T>
static int nbSymbols(const T* theUtfString)
{
  int aNbCodeUnits = 0;
  for (NCollection_UtfIterator<T> anIter(theUtfString); *anIter != 0; ++anIter)
  {
    aNbCodeUnits += anIter.AdvanceCodeUnitsUtf16();
  }
  return aNbCodeUnits;
}

//! Convert from wchar_t* to extended string.
//! Default implementation when size of wchar_t and extended char is different (e.g. Linux / UNIX).
template <size_t CharSize>
inline char16_t* Standard_UNUSED fromWideString(const wchar_t* theUtfString, int& theLength)
{
  theLength = nbSymbols(theUtfString);
  if (theLength == 0)
  {
    return THE_DEFAULT_EXT_CHAR_STRING;
  }
  const size_t aRoundSize = calculatePaddedSize(theLength);
  char16_t*    aString    = static_cast<char16_t*>(Standard::AllocateOptimal(aRoundSize));
  NCollection_UtfIterator<wchar_t> anIterRead(theUtfString);
  for (char16_t* anIterWrite = aString; *anIterRead != 0; ++anIterRead)
  {
    anIterWrite = anIterRead.GetUtf(anIterWrite);
  }
  aString[theLength] = 0;
  return aString;
}

//! Use memcpy() conversion when size is the same (e.g. on Windows).
template <>
inline char16_t* Standard_UNUSED fromWideString<sizeof(char16_t)>(const wchar_t* theUtfString,
                                                                  int&           theLength)
{
  for (theLength = 0; theUtfString[theLength] != 0; ++theLength)
  {
  }
  if (theLength == 0)
  {
    return THE_DEFAULT_EXT_CHAR_STRING;
  }
  const size_t aRoundSize = calculatePaddedSize(theLength);
  char16_t*    aString    = static_cast<char16_t*>(Standard::AllocateOptimal(aRoundSize));
  const int    aSize      = theLength * sizeof(char16_t);
  memcpy(aString, theUtfString, aSize);
  aString[theLength] = 0;
  return aString;
}

//! Helper structure to hold formatted integer string with its length
struct FormattedInteger
{
  char Buffer[16]; // Enough for 32-bit int + sign + null terminator
  int  Length;

  FormattedInteger(const int theValue) { Length = Sprintf(Buffer, "%d", theValue); }
};

//! Helper structure to hold formatted real number string with its length
struct FormattedReal
{
  char Buffer[64]; // Enough for double in %g format + null terminator
  int  Length;

  FormattedReal(const double theValue) { Length = Sprintf(Buffer, "%g", theValue); }
};

} // namespace

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString() noexcept
{
  allocate(0);
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char* const theString,
                                                       const bool        isMultiByte)
{
  if (theString == nullptr)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  if (theIsMultiByte)
  {
    allocate(nbSymbols(theString));
    myString[myLength] = 0;
    if (ConvertToUnicode(theString))
    {
      return;
    }
    reallocate((int)strlen(theString));
  }
  else
  {
    allocate((int)strlen(theString));
  }
  for (int aCharIter = 0; aCharIter < myLength; ++aCharIter)
  {
    myString[aCharIter] = ToExtCharacter(theString[aCharIter]);
  }
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char16_t* const theString)
{
  if (theString == nullptr)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  for (myLength = 0; theString[myLength] != 0; ++myLength)
  {
  }
  allocate(myLength);
  memcpy(myString, theString, myLength * sizeof(char16_t));
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char16_t* theString,
                                                       const int       theLength)
{
  if (theString == nullptr)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }
  if (theLength < 0)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString : negative length");
  }

  allocate(theLength);
  if (theLength > 0)
  {
    memcpy(myString, theString, theLength * sizeof(char16_t));
  }
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const wchar_t* theStringUtf)
{
  if (theStringUtf == nullptr)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  myString = fromWideString<sizeof(wchar_t)>(theStringUtf, myLength);
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char theChar)
{
  if (theChar != '\0')
  {
    allocate(1);
    myString[0] = ToExtCharacter(theChar);
  }
  else
  {
    allocate(0);
  }
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char16_t theChar)
{
  if (theChar != u'\0')
  {
    allocate(1);
    myString[0] = theChar;
  }
  else
  {
    allocate(0);
  }
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const int      theLength,
                                                       const char16_t theFiller)
{
  if (theLength < 0)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString : negative length");
  }
  allocate(theLength);
  for (int i = 0; i < theLength; i++)
  {
    myString[i] = theFiller;
  }
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const int theValue)
{
  const FormattedInteger aFormatted(theValue);
  allocate(aFormatted.Length);
  for (int i = 0; i < myLength; i++)
  {
    myString[i] = ToExtCharacter(aFormatted.Buffer[i]);
  }
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const double theValue)
{
  const FormattedReal aFormatted(theValue);
  allocate(aFormatted.Length);
  for (int i = 0; i < myLength; i++)
  {
    myString[i] = ToExtCharacter(aFormatted.Buffer[i]);
  }
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const TCollection_ExtendedString& theString)
{
  allocate(theString.myLength);
  memcpy(myString, theString.myString, myLength * sizeof(char16_t));
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(
  TCollection_ExtendedString&& theOther) noexcept
{
  if (theOther.myString == THE_DEFAULT_EXT_CHAR_STRING)
  {
    allocate(0);
  }
  else
  {
    myString = theOther.myString;
    myLength = theOther.myLength;
  }
  theOther.myString = THE_DEFAULT_EXT_CHAR_STRING;
  theOther.myLength = 0;
}

//==================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const TCollection_AsciiString& theString,
                                                       const bool theIsMultiByte)
{
  allocate(nbSymbols(theString.ToCString()));
  if (theIsMultiByte && ConvertToUnicode(theString.ToCString()))
  {
    return;
  }
  reallocate(theString.Length());
  const char* aCString = theString.ToCString();
  for (int aCharIter = 0; aCharIter <= myLength; ++aCharIter)
  {
    myString[aCharIter] = ToExtCharacter(aCString[aCharIter]);
  }
}

//==================================================================================================

void TCollection_ExtendedString::AssignCat(const TCollection_ExtendedString& theOther)
{
  if (theOther.myLength == 0)
  {
    return;
  }

  // Handle self-append case: theOther.myString will be invalidated by reallocate
  if (&theOther == this)
  {
    const int anOldLength = myLength;
    reallocate(myLength + myLength);
    memcpy(myString + anOldLength, myString, anOldLength * sizeof(char16_t));
    return;
  }

  const int anOtherLength = theOther.myLength;
  const int anOldLength   = myLength;
  reallocate(myLength + anOtherLength);
  memcpy(myString + anOldLength, theOther.myString, anOtherLength * sizeof(char16_t));
}

//==================================================================================================

void TCollection_ExtendedString::AssignCat(const char16_t theChar)
{
  if (theChar != u'\0')
  {
    reallocate(myLength + 1);
    myString[myLength - 1] = theChar;
  }
}

//==================================================================================================

void TCollection_ExtendedString::AssignCat(const char16_t* theString, const int theLength)
{
  if (theString == nullptr || theLength <= 0)
  {
    return;
  }

  // Check if theString points into current buffer (aliasing case)
  const bool isAliased = (theString >= myString && theString < myString + myLength);

  if (isAliased)
  {
    // Save offset before reallocate invalidates the pointer
    const std::ptrdiff_t anOffset    = theString - myString;
    const int            anOldLength = myLength;
    reallocate(myLength + theLength);
    // Use memmove since source and destination may overlap
    memmove(myString + anOldLength, myString + anOffset, theLength * sizeof(char16_t));
  }
  else
  {
    const int anOldLength = myLength;
    reallocate(myLength + theLength);
    memcpy(myString + anOldLength, theString, theLength * sizeof(char16_t));
  }
}

//==================================================================================================

TCollection_ExtendedString TCollection_ExtendedString::Cat(const char16_t* theOther,
                                                           const int       theLength) const
{
  TCollection_ExtendedString aResult(myLength + theLength, 0);
  if (myLength > 0)
  {
    memcpy(aResult.myString, myString, myLength * sizeof(char16_t));
  }
  if (theLength > 0)
  {
    memcpy(aResult.myString + myLength, theOther, theLength * sizeof(char16_t));
  }
  return aResult;
}

//==================================================================================================

void TCollection_ExtendedString::ChangeAll(const char16_t theChar, const char16_t theNewChar)
{
  for (int i = 0; i < myLength; i++)
    if (myString[i] == theChar)
      myString[i] = theNewChar;
}

//==================================================================================================

void TCollection_ExtendedString::Clear()
{
  deallocate();
}

//==================================================================================================

void TCollection_ExtendedString::Copy(const char16_t* theString, const int theLength)
{
  if (theString == nullptr || theLength <= 0)
  {
    if (myString != THE_DEFAULT_EXT_CHAR_STRING)
    {
      myLength    = 0;
      myString[0] = 0;
    }
    return;
  }

  reallocate(theLength);
  memcpy(myString, theString, theLength * sizeof(char16_t));
}

//==================================================================================================

void TCollection_ExtendedString::Move(TCollection_ExtendedString&& theOther)
{
  if (&theOther == this)
  {
    return;
  }
  if (theOther.myString == THE_DEFAULT_EXT_CHAR_STRING)
  {
    reallocate(0);
  }
  else
  {
    if (myString != THE_DEFAULT_EXT_CHAR_STRING)
    {
      Standard::Free(myString);
    }
    myString = theOther.myString;
    myLength = theOther.myLength;
  }
  theOther.myString = THE_DEFAULT_EXT_CHAR_STRING;
  theOther.myLength = 0;
}

//==================================================================================================

void TCollection_ExtendedString::Swap(TCollection_ExtendedString& theOther)
{
  if (&theOther == this)
  {
    return;
  }
  std::swap(myString, theOther.myString);
  std::swap(myLength, theOther.myLength);
}

//==================================================================================================

TCollection_ExtendedString::~TCollection_ExtendedString()
{
  deallocate();
}

//==================================================================================================

void TCollection_ExtendedString::Insert(const int theWhere, const char16_t theWhat)
{
  if (theWhere > myLength + 1)
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter theWhere is too big");
  if (theWhere < 1)
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter theWhere is out of range");
  reallocate(myLength + 1);
  if (theWhere != myLength)
  {
    for (int i = myLength - 2; i >= theWhere - 1; i--)
      myString[i + 1] = myString[i];
  }
  myString[theWhere - 1] = theWhat;
}

//==================================================================================================

void TCollection_ExtendedString::Insert(const int       theWhere,
                                        const char16_t* theWhat,
                                        const int       theLength)
{
  if (theWhere > myLength + 1)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter theWhere is too big");
  }
  if (theWhere < 1)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter theWhere is out of range");
  }
  if (theWhat == nullptr || theLength <= 0)
  {
    return;
  }

  const int anOldLength = myLength;
  reallocate(myLength + theLength);
  if (theWhere != anOldLength + 1)
  {
    for (int i = anOldLength - 1; i >= theWhere - 1; i--)
    {
      myString[i + theLength] = myString[i];
    }
  }
  for (int i = 0; i < theLength; i++)
  {
    myString[theWhere - 1 + i] = theWhat[i];
  }
}

//==================================================================================================

static int ExtStrCmp(const char16_t* const theStr1, const char16_t* const theStr2)
{
  if (theLength < 0)
  {
    return false;
  }
  if (myLength != theLength)
  {
    return false;
  }
  if (theLength == 0)
  {
    return true;
  }
  if (theOther == nullptr)
  {
    return false;
  }
  return memcmp(myString, theOther, theLength * sizeof(char16_t)) == 0;
}

//==================================================================================================

bool TCollection_ExtendedString::IsDifferent(const char16_t* const theOther,
                                             const int             theLength) const
{
  if (theLength < 0)
  {
    return true;
  }
  if (myLength != theLength)
  {
    return true;
  }
  if (theLength == 0)
  {
    return false;
  }
  if (theOther == nullptr)
  {
    return true;
  }
  return memcmp(myString, theOther, theLength * sizeof(char16_t)) != 0;
}

//==================================================================================================

bool TCollection_ExtendedString::IsLess(const char16_t* const theOther, const int theLength) const
{
  if (theLength < 0)
  {
    return false; // this >= invalid
  }
  if (theOther == nullptr)
  {
    return theLength > 0 ? false : myLength < 0; // this >= empty/invalid
  }
  const int aMinLen = (myLength < theLength) ? myLength : theLength;
  for (int i = 0; i < aMinLen; ++i)
  {
    if (myString[i] < theOther[i])
    {
      return true;
    }
    if (myString[i] > theOther[i])
    {
      return false;
    }
  }
  return myLength < theLength;
}

//==================================================================================================

bool TCollection_ExtendedString::IsGreater(const char16_t* const theOther,
                                           const int             theLength) const
{
  if (theLength < 0)
  {
    return true; // this > invalid
  }
  if (theOther == nullptr)
  {
    return theLength == 0 ? myLength > 0 : true; // this > empty/invalid
  }
  const int aMinLen = (myLength < theLength) ? myLength : theLength;
  for (int i = 0; i < aMinLen; ++i)
  {
    if (myString[i] > theOther[i])
    {
      return true;
    }
    if (myString[i] < theOther[i])
    {
      return false;
    }
  }
  return myLength > theLength;
}

//==================================================================================================

bool TCollection_ExtendedString::StartsWith(const char16_t* const theStartString,
                                            const int             theLength) const
{
  if (theLength < 0)
  {
    return false;
  }
  if (theLength == 0)
  {
    return true;
  }
  if (theStartString == nullptr)
  {
    return false;
  }
  if (myLength < theLength)
  {
    return false;
  }
  return memcmp(myString, theStartString, theLength * sizeof(char16_t)) == 0;
}

//==================================================================================================

bool TCollection_ExtendedString::EndsWith(const char16_t* const theEndString,
                                          const int             theLength) const
{
  if (theLength < 0)
  {
    return false;
  }
  if (theLength == 0)
  {
    return true;
  }
  if (theEndString == nullptr)
  {
    return false;
  }
  if (myLength < theLength)
  {
    return false;
  }
  return memcmp(myString + myLength - theLength, theEndString, theLength * sizeof(char16_t)) == 0;
}

//==================================================================================================

bool TCollection_ExtendedString::IsAscii() const
{
  for (int i = 0; i < myLength; i++)
    if (!IsAnAscii(myString[i]))
      return false;
  return true;
}

//==================================================================================================

int TCollection_ExtendedString::Length() const
{
  return myLength;
}

//==================================================================================================

void TCollection_ExtendedString::Print(Standard_OStream& theStream) const
{
  if (myLength > 0)
  {
    const TCollection_AsciiString aUtf8(myString);
    theStream << aUtf8;
  }
}

//==================================================================================================

Standard_OStream& operator<<(Standard_OStream&                 theStream,
                             const TCollection_ExtendedString& theString)
{
  theString.Print(theStream);
  return theStream;
}

//==================================================================================================

void TCollection_ExtendedString::RemoveAll(const char16_t theWhat)

{
  if (myLength == 0)
    return;
  int c = 0;
  for (int i = 0; i < myLength; i++)
    if (myString[i] != theWhat)
      myString[c++] = myString[i];
  myLength           = c;
  myString[myLength] = u'\0';
}

//==================================================================================================

void TCollection_ExtendedString::Remove(const int theWhere, const int theHowMany)
{
  if (theWhere < 1 || theWhere > myLength || theHowMany < 0 || theWhere + theHowMany > myLength + 1)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::Remove: "
                              "Invalid parameters");
  }

  if (theHowMany == 0)
  {
    return;
  }

  for (int i = theWhere + theHowMany - 1, j = theWhere - 1; i < myLength; i++, j++)
  {
    myString[j] = myString[i];
  }
  myLength -= theHowMany;
  myString[myLength] = u'\0';
}

//==================================================================================================

int TCollection_ExtendedString::Search(const char16_t* const theWhat, const int theLength) const
{
  if (theWhat == nullptr || theLength <= 0)
  {
    return -1;
  }
  if (theLength > myLength)
  {
    return -1;
  }

  int  k, j;
  int  i    = 0;
  bool find = false;
  while (i < myLength - theLength + 1 && !find)
  {
    k = i++;
    j = 0;
    while (j < theLength && myString[k++] == theWhat[j++])
    {
      if (j == theLength)
      {
        find = true;
      }
    }
  }
  if (find)
  {
    return i;
  }
  return -1;
}

//==================================================================================================

int TCollection_ExtendedString::SearchFromEnd(const char16_t* const theWhat,
                                              const int             theLength) const
{
  if (theWhat == nullptr || theLength <= 0)
  {
    return -1;
  }
  if (theLength > myLength)
  {
    return -1;
  }

  int  k, j;
  int  i    = myLength - 1;
  bool find = false;
  while (i >= theLength - 1 && !find)
  {
    k = i--;
    j = theLength - 1;
    while (j >= 0 && myString[k--] == theWhat[j--])
    {
      if (j == -1)
      {
        find = true;
      }
    }
  }
  if (find)
  {
    return i - theLength + 3;
  }
  return -1;
}

//==================================================================================================

void TCollection_ExtendedString::SetValue(const int theWhere, const char16_t theWhat)
{
  if (theWhere > 0 && theWhere <= myLength)
  {
    myString[theWhere - 1] = theWhat;
  }
  else
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::SetValue : parameter theWhere");
  }
}

//==================================================================================================

void TCollection_ExtendedString::SetValue(const int             theWhere,
                                          const char16_t* const theWhat,
                                          const int             theLength)
{
  if (theWhere < 1 || theWhere > myLength + 1)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::SetValue : parameter theWhere");
  }
  if (theWhat == nullptr || theLength <= 0)
  {
    return;
  }

  const int aNewEnd = theWhere - 1 + theLength;
  if (aNewEnd > myLength)
  {
    reallocate(aNewEnd);
  }
  for (int i = 0; i < theLength; i++)
  {
    myString[theWhere - 1 + i] = theWhat[i];
  }
}

//==================================================================================================

TCollection_ExtendedString TCollection_ExtendedString::SubString(const int theFromIndex,
                                                                 const int theToIndex) const
{
  if (theFromIndex < 1 || theFromIndex > myLength)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::SubString : theFromIndex");
  }
  if (theToIndex < 1 || theToIndex > myLength)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::SubString : theToIndex");
  }
  if (theFromIndex > theToIndex)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::SubString : theFromIndex > theToIndex");
  }

  const int aSubLen = theToIndex - theFromIndex + 1;
  return TCollection_ExtendedString(myString + theFromIndex - 1, aSubLen);
}

//==================================================================================================

TCollection_ExtendedString TCollection_ExtendedString::Split(const int theWhere)
{
  if (theWhere >= 0 && theWhere < myLength)
  {
    TCollection_ExtendedString res(&myString[theWhere]);
    Trunc(theWhere);
    return res;
  }
  throw Standard_OutOfRange("TCollection_ExtendedString::Split index");
}

//==================================================================================================

TCollection_ExtendedString TCollection_ExtendedString::Token(const char16_t* const theSeparators,
                                                             const int theWhichOne) const
{
  if (myLength == 0)
  {
    return TCollection_ExtendedString();
  }
  TCollection_ExtendedString res(myLength, 0);
  if (!theSeparators)
    throw Standard_NullObject("TCollection_ExtendedString::Token : "
                              "parameter 'theSeparators'");

  int       i, j, k, l;
  char16_t* buftmp = res.myString;
  char16_t  aSep;

  bool isSepFound = false, otherSepFound;

  j = 0;

  for (i = 0; i < theWhichOne && j < myLength; i++)
  {
    isSepFound = false;
    k          = 0;
    buftmp[0]  = 0;

    // Avant de commencer il faut virer les saloperies devant
    //
    otherSepFound = true;
    while (j < myLength && otherSepFound)
    {
      l             = 0;
      otherSepFound = false;
      aSep          = theSeparators[l];
      while (aSep != 0)
      {
        if (aSep == myString[j])
        {
          aSep          = 0;
          otherSepFound = true;
        }
        else
        {
          aSep = theSeparators[l++];
        }
      }
      if (otherSepFound)
        j++;
    }

    while (!isSepFound && k < myLength && j < myLength)
    {
      l    = 0;
      aSep = theSeparators[l];

      while (aSep != 0 && !isSepFound)
      {
        if (aSep == myString[j])
        {
          buftmp[k]  = 0;
          isSepFound = true;
        }
        else
        {
          buftmp[k] = myString[j];
        }
        l++;
        aSep = theSeparators[l];
      }
      j++;
      k++;
      if (j == myLength)
        buftmp[k] = 0;
    }
  }

  if (i < theWhichOne)
  {
    res.myLength    = 0;
    res.myString[0] = 0;
  }
  else
  {
    for (res.myLength = 0; buftmp[res.myLength]; ++res.myLength)
      ;
    res.myString[res.myLength] = u'\0';
  }
  return res;
}

//==================================================================================================

const char16_t* TCollection_ExtendedString::ToExtString() const
{
  return myString;
}

//==================================================================================================

void TCollection_ExtendedString::Trunc(const int theHowMany)
{
  if (theHowMany < 0 || theHowMany > myLength)
    throw Standard_OutOfRange("TCollection_ExtendedString::Trunc : "
                              "parameter 'theHowMany'");
  myLength           = theHowMany;
  myString[myLength] = u'\0';
}

//==================================================================================================

char16_t TCollection_ExtendedString::Value(const int theWhere) const
{
  if (theWhere > 0 && theWhere <= myLength)
  {
    if (myString)
      return myString[theWhere - 1];
    else
      return 0;
  }
  throw Standard_OutOfRange("TCollection_ExtendedString::Value : "
                            "parameter theWhere");
}

//==================================================================================================

bool TCollection_ExtendedString::ConvertToUnicode(const char* theString)
{
  NCollection_UtfIterator<char> anIterRead(theString);
  char16_t*                     anIterWrite = myString;
  if (*anIterRead == 0)
  {
    *anIterWrite = u'\0';
    return true;
  }

  for (; *anIterRead != 0; ++anIterRead)
  {
    if (!anIterRead.IsValid())
    {
      return false;
    }

    anIterWrite = anIterRead.GetUtf(anIterWrite);
  }
  return true;
}

//==================================================================================================

int TCollection_ExtendedString::LengthOfCString() const
{
  int aSizeBytes = 0;
  for (NCollection_UtfIterator<char16_t> anIter(myString); *anIter != 0; ++anIter)
  {
    aSizeBytes += anIter.AdvanceBytesUtf8();
  }
  return aSizeBytes;
}

//==================================================================================================

int TCollection_ExtendedString::ToUTF8CString(Standard_PCharacter& theCString) const
{
  NCollection_UtfIterator<char16_t> anIterRead(myString);
  char*                             anIterWrite = theCString;
  if (*anIterRead == 0)
  {
    *anIterWrite = '\0';
    return 0;
  }

  for (; *anIterRead != 0; ++anIterRead)
  {
    anIterWrite = anIterRead.GetUtf(anIterWrite);
  }
  *anIterWrite = '\0';
  return int(anIterWrite - theCString);
}

//==================================================================================================

void TCollection_ExtendedString::allocate(const int theLength)
{
  myLength = theLength;
  if (theLength == 0)
  {
    myString = THE_DEFAULT_EXT_CHAR_STRING;
  }
  else
  {
    const size_t aRoundSize = calculatePaddedSize(theLength);
    myString                = static_cast<char16_t*>(Standard::AllocateOptimal(aRoundSize));
    myString[myLength]      = u'\0';
  }
}

//==================================================================================================

void TCollection_ExtendedString::reallocate(const int theLength)
{
  if (theLength == 0)
  {
    // When shrinking to empty, just set length to 0 and null-terminate
    if (myString != THE_DEFAULT_EXT_CHAR_STRING)
    {
      myString[0] = 0;
    }
    myLength = 0;
    return;
  }

  if (myString == THE_DEFAULT_EXT_CHAR_STRING)
  {
    const size_t aRoundSize = calculatePaddedSize(theLength);
    myString                = static_cast<char16_t*>(Standard::AllocateOptimal(aRoundSize));
  }
  else
  {
    const size_t aRoundSize = calculatePaddedSize(theLength);
    myString                = static_cast<char16_t*>(Standard::Reallocate(myString, aRoundSize));
  }
  myString[theLength] = 0;
  myLength            = theLength;
}

//==================================================================================================

void TCollection_ExtendedString::deallocate()
{
  if (myString != THE_DEFAULT_EXT_CHAR_STRING)
  {
    Standard::Free(myString);
  }
  myLength = 0;
  myString = THE_DEFAULT_EXT_CHAR_STRING;
}

//==================================================================================================

const TCollection_ExtendedString& TCollection_ExtendedString::EmptyString() noexcept
{
  static const TCollection_ExtendedString THE_EMPTY_STRING;
  return THE_EMPTY_STRING;
}

//==================================================================================================

void TCollection_ExtendedString::LeftAdjust()
{
  if (myLength == 0)
  {
    return;
  }

  int anIndex = 0;
  while (anIndex < myLength && myString[anIndex] == u' ')
  {
    ++anIndex;
  }

  if (anIndex > 0)
  {
    myLength -= anIndex;
    memmove(myString, myString + anIndex, myLength * sizeof(char16_t));
    myString[myLength] = u'\0';
  }
}

//==================================================================================================

void TCollection_ExtendedString::RightAdjust()
{
  if (myLength == 0)
  {
    return;
  }

  while (myLength > 0 && myString[myLength - 1] == u' ')
  {
    --myLength;
  }
  myString[myLength] = u'\0';
}

//==================================================================================================

void TCollection_ExtendedString::LeftJustify(const int theWidth, const char16_t theFiller)
{
  if (theWidth < 0)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::LeftJustify : theWidth is negative");
  }

  if (theWidth > myLength)
  {
    const int anOldLength = myLength;
    reallocate(theWidth);
    for (int i = anOldLength; i < theWidth; ++i)
    {
      myString[i] = theFiller;
    }
  }
}

//==================================================================================================

void TCollection_ExtendedString::RightJustify(const int theWidth, const char16_t theFiller)
{
  if (theWidth < 0)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::RightJustify : theWidth is negative");
  }

  if (theWidth > myLength)
  {
    const int aShift   = theWidth - myLength;
    const int anOldLen = myLength;
    reallocate(theWidth);
    // Move existing characters to the end
    memmove(myString + aShift, myString, anOldLen * sizeof(char16_t));
    // Fill the beginning with filler
    for (int i = 0; i < aShift; ++i)
    {
      myString[i] = theFiller;
    }
  }
}

//==================================================================================================

void TCollection_ExtendedString::Center(const int theWidth, const char16_t theFiller)
{
  if (theWidth < 0)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::Center : theWidth is negative");
  }

  if (theWidth > myLength)
  {
    const int aLeftShift  = (theWidth - myLength) / 2;
    const int aRightSpace = theWidth - myLength - aLeftShift;
    const int anOldLen    = myLength;
    reallocate(theWidth);

    // Move existing characters to the center
    memmove(myString + aLeftShift, myString, anOldLen * sizeof(char16_t));
    // Fill the beginning
    for (int i = 0; i < aLeftShift; ++i)
    {
      myString[i] = theFiller;
    }
    // Fill the end
    for (int i = 0; i < aRightSpace; ++i)
    {
      myString[anOldLen + aLeftShift + i] = theFiller;
    }
  }
}

//==================================================================================================

void TCollection_ExtendedString::Capitalize()
{
  if (myLength == 0)
  {
    return;
  }

  // Convert first character to uppercase (ASCII only)
  if (myString[0] >= u'a' && myString[0] <= u'z')
  {
    myString[0] = static_cast<char16_t>(myString[0] - u'a' + u'A');
  }

  // Convert remaining characters to lowercase (ASCII only)
  for (int i = 1; i < myLength; ++i)
  {
    if (myString[i] >= u'A' && myString[i] <= u'Z')
    {
      myString[i] = static_cast<char16_t>(myString[i] - u'A' + u'a');
    }
  }
}

//==================================================================================================

void TCollection_ExtendedString::Prepend(const char16_t* const theOther, const int theLength)
{
  if (theOther == nullptr || theLength <= 0)
  {
    return;
  }

  // Check if theOther points into current buffer (aliasing case)
  const bool isAliased = (theOther >= myString && theOther < myString + myLength);

  if (isAliased)
  {
    // Save offset before reallocate invalidates the pointer
    const std::ptrdiff_t anOffset    = theOther - myString;
    const int            anOldLength = myLength;
    reallocate(myLength + theLength);
    // Move existing characters to the end first
    memmove(myString + theLength, myString, anOldLength * sizeof(char16_t));
    // Copy from the shifted position (offset + theLength because content was moved)
    memmove(myString, myString + theLength + anOffset, theLength * sizeof(char16_t));
  }
  else
  {
    const int anOldLength = myLength;
    reallocate(myLength + theLength);
    // Move existing characters to the end
    memmove(myString + theLength, myString, anOldLength * sizeof(char16_t));
    // Copy theOther to the beginning
    memcpy(myString, theOther, theLength * sizeof(char16_t));
  }
}

//==================================================================================================

int TCollection_ExtendedString::FirstLocationInSet(const TCollection_ExtendedString& theSet,
                                                   const int                         theFromIndex,
                                                   const int theToIndex) const
{
  if (theFromIndex < 1 || theFromIndex > myLength)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::FirstLocationInSet : theFromIndex");
  }
  if (theToIndex < 1 || theToIndex > myLength)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::FirstLocationInSet : theToIndex");
  }
  if (theSet.myLength == 0)
  {
    return 0;
  }

  for (int i = theFromIndex - 1; i < theToIndex; ++i)
  {
    for (int j = 0; j < theSet.myLength; ++j)
    {
      if (myString[i] == theSet.myString[j])
      {
        return i + 1;
      }
    }
  }
  return 0;
}

//==================================================================================================

int TCollection_ExtendedString::FirstLocationNotInSet(const TCollection_ExtendedString& theSet,
                                                      const int theFromIndex,
                                                      const int theToIndex) const
{
  if (theFromIndex < 1 || theFromIndex > myLength)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::FirstLocationNotInSet : theFromIndex");
  }
  if (theToIndex < 1 || theToIndex > myLength)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::FirstLocationNotInSet : theToIndex");
  }
  if (theSet.myLength == 0)
  {
    return (theFromIndex <= theToIndex) ? theFromIndex : 0;
  }

  for (int i = theFromIndex - 1; i < theToIndex; ++i)
  {
    bool isFound = false;
    for (int j = 0; j < theSet.myLength; ++j)
    {
      if (myString[i] == theSet.myString[j])
      {
        isFound = true;
        break;
      }
    }
    if (!isFound)
    {
      return i + 1;
    }
  }
  return 0;
}

//==================================================================================================

int TCollection_ExtendedString::IntegerValue() const
{
  if (myLength == 0)
  {
    return 0;
  }
  // Convert to ASCII and parse
  TCollection_AsciiString anAscii(*this);
  return anAscii.IntegerValue();
}

//==================================================================================================

bool TCollection_ExtendedString::IsIntegerValue() const
{
  if (myLength == 0)
  {
    return false;
  }
  // Convert to ASCII and check
  TCollection_AsciiString anAscii(*this);
  return anAscii.IsIntegerValue();
}

//==================================================================================================

double TCollection_ExtendedString::RealValue() const
{
  if (myLength == 0)
  {
    return 0.0;
  }
  // Convert to ASCII and parse
  TCollection_AsciiString anAscii(*this);
  return anAscii.RealValue();
}

//==================================================================================================

bool TCollection_ExtendedString::IsRealValue(bool theToCheckFull) const
{
  if (myLength == 0)
  {
    return false;
  }
  // Convert to ASCII and check
  TCollection_AsciiString anAscii(*this);
  return anAscii.IsRealValue(theToCheckFull);
}

//==================================================================================================

bool TCollection_ExtendedString::IsSameString(const TCollection_ExtendedString& theOther,
                                              const bool theIsCaseSensitive) const
{
  if (myLength != theOther.myLength)
  {
    return false;
  }

  if (theIsCaseSensitive)
  {
    return memcmp(myString, theOther.myString, myLength * sizeof(char16_t)) == 0;
  }

  // Case-insensitive comparison (ASCII only)
  for (int i = 0; i < myLength; ++i)
  {
    char16_t c1 = myString[i];
    char16_t c2 = theOther.myString[i];

    // Convert to lowercase for comparison
    if (c1 >= u'A' && c1 <= u'Z')
    {
      c1 = static_cast<char16_t>(c1 - u'A' + u'a');
    }
    if (c2 >= u'A' && c2 <= u'Z')
    {
      c2 = static_cast<char16_t>(c2 - u'A' + u'a');
    }

    if (c1 != c2)
    {
      return false;
    }
  }
  return true;
}
