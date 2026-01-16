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

namespace
{
static char16_t THE_DEFAULT_EXT_CHAR_STRING[1] = {0};

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
  char16_t*    aString = static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
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
  char16_t*    aString = static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
  const int    aSize   = theLength * sizeof(char16_t);
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

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString() noexcept
{
  allocate(0);
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char* theString,
                                                       const bool  isMultiByte)
{
  if (theString == nullptr)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  if (isMultiByte)
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

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char16_t* theString)
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

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const wchar_t* theStringUtf)
{
  if (theStringUtf == nullptr)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  myString = fromWideString<sizeof(wchar_t)>(theStringUtf, myLength);
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char aChar)
{
  if (aChar != '\0')
  {
    allocate(1);
    myString[0] = ToExtCharacter(aChar);
  }
  else
  {
    allocate(0);
  }
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char16_t aChar)
{
  if (aChar != u'\0')
  {
    allocate(1);
    myString[0] = aChar;
  }
  else
  {
    allocate(0);
  }
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const int length, const char16_t filler)
{
  allocate(length);
  for (int i = 0; i < length; i++)
    myString[i] = filler;
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const int theValue)
{
  const FormattedInteger aFormatted(theValue);
  allocate(aFormatted.Length);
  for (int i = 0; i < myLength; i++)
  {
    myString[i] = ToExtCharacter(aFormatted.Buffer[i]);
  }
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const double theValue)
{
  const FormattedReal aFormatted(theValue);
  allocate(aFormatted.Length);
  for (int i = 0; i < myLength; i++)
  {
    myString[i] = ToExtCharacter(aFormatted.Buffer[i]);
  }
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const TCollection_ExtendedString& theOther)
{
  allocate(theOther.myLength);
  memcpy(myString, theOther.myString, myLength * sizeof(char16_t));
}

//=================================================================================================

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

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const TCollection_AsciiString& theString,
                                                       const bool                     isMultiByte)
{
  allocate(nbSymbols(theString.ToCString()));
  if (isMultiByte && ConvertToUnicode(theString.ToCString()))
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

//=================================================================================================

void TCollection_ExtendedString::AssignCat(const TCollection_ExtendedString& theOther)
{
  if (theOther.myLength == 0)
  {
    return;
  }

  const int anOtherLength = theOther.myLength;
  const int anOldLength   = myLength;
  reallocate(myLength + anOtherLength);
  memcpy(myString + anOldLength, theOther.myString, anOtherLength * sizeof(char16_t));
}

//=================================================================================================

void TCollection_ExtendedString::AssignCat(const char16_t theChar)
{
  if (theChar != '\0')
  {
    reallocate(myLength + 1);
    myString[myLength - 1] = theChar;
  }
}

//==================================================================================================

void TCollection_ExtendedString::AssignCat(const char16_t* theString)
{
  if (theString == nullptr || theString[0] == '\0')
  {
    return;
  }

  int aOtherLength = 0;
  for (; theString[aOtherLength] != 0; ++aOtherLength)
  {
  }

  const int anOldLength = myLength;
  reallocate(myLength + aOtherLength);
  memcpy(myString + anOldLength, theString, aOtherLength * sizeof(char16_t));
}

//=================================================================================================

TCollection_ExtendedString TCollection_ExtendedString::Cat(
  const TCollection_ExtendedString& other) const
{
  TCollection_ExtendedString res(myLength + other.myLength, 0);
  if (myLength > 0)
    memcpy(res.myString, myString, myLength * sizeof(char16_t));
  if (other.myLength > 0)
    memcpy(res.myString + myLength, other.myString, other.myLength * sizeof(char16_t));
  return res;
}

//=================================================================================================

void TCollection_ExtendedString::ChangeAll(const char16_t aChar, const char16_t NewChar)
{
  for (int i = 0; i < myLength; i++)
    if (myString[i] == aChar)
      myString[i] = NewChar;
}

//=================================================================================================

void TCollection_ExtendedString::Clear()
{
  deallocate();
}

//=================================================================================================

void TCollection_ExtendedString::Copy(const TCollection_ExtendedString& fromwhere)
{
  if (&fromwhere == this)
  {
    return;
  }
  if (fromwhere.myString && fromwhere.myLength > 0)
  {
    reallocate(fromwhere.myLength);
    memcpy(myString, fromwhere.myString, myLength * sizeof(char16_t));
  }
  else if (myString != THE_DEFAULT_EXT_CHAR_STRING)
  {
    myLength           = 0;
    myString[myLength] = 0;
  }
}

//==================================================================================================

void TCollection_ExtendedString::Copy(const char16_t* theString)
{
  if (theString == nullptr)
  {
    if (myString != THE_DEFAULT_EXT_CHAR_STRING)
    {
      myLength    = 0;
      myString[0] = 0;
    }
    return;
  }

  int aNewLength = 0;
  for (; theString[aNewLength] != 0; ++aNewLength)
  {
  }

  if (aNewLength == 0)
  {
    if (myString != THE_DEFAULT_EXT_CHAR_STRING)
    {
      myLength    = 0;
      myString[0] = 0;
    }
  }
  else
  {
    reallocate(aNewLength);
    memcpy(myString, theString, aNewLength * sizeof(char16_t));
  }
}

//=================================================================================================

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

//=================================================================================================

void TCollection_ExtendedString::Swap(TCollection_ExtendedString& theOther)
{
  if (&theOther == this)
  {
    return;
  }
  std::swap(myString, theOther.myString);
  std::swap(myLength, theOther.myLength);
}

//=================================================================================================

TCollection_ExtendedString::~TCollection_ExtendedString()
{
  deallocate();
}

//=================================================================================================

void TCollection_ExtendedString::Insert(const int where, const char16_t what)
{
  if (where > myLength + 1)
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter where is too big");
  if (where < 1)
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter where is out of range");
  reallocate(myLength + 1);
  if (where != myLength)
  {
    for (int i = myLength - 2; i >= where - 1; i--)
      myString[i + 1] = myString[i];
  }
  myString[where - 1] = what;
}

//=================================================================================================

void TCollection_ExtendedString::Insert(const int where, const TCollection_ExtendedString& what)
{
  const char16_t* swhat = what.myString;
  if (where <= myLength + 1)
  {
    int whatlength = what.myLength;
    if (whatlength)
    {
      const int anOldLength = myLength;
      reallocate(myLength + whatlength);
      if (where != anOldLength + 1)
      {
        for (int i = anOldLength - 1; i >= where - 1; i--)
          myString[i + whatlength] = myString[i];
      }
      for (int i = 0; i < whatlength; i++)
        myString[where - 1 + i] = swhat[i];
    }
  }
  else
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter where is too big");
  }
}

//=================================================================================================

static int ExtStrCmp(const char16_t* theStr1, const char16_t* theStr2)
{
  const char16_t* aStr1 = theStr1;
  const char16_t* aStr2 = theStr2;
  while (*aStr1 != 0 && *aStr1 == *aStr2)
  {
    aStr1++;
    aStr2++;
  }
  // char16_t is char16_t which is 2-byte unsigned int, it will be promoted
  // to plain 4-byte int for calculation
  return *aStr1 - *aStr2;
}

//=================================================================================================

bool TCollection_ExtendedString::IsEqual(const char16_t* other) const
{
  return ExtStrCmp(myString, other) == 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsEqual(const TCollection_ExtendedString& other) const
{
  return myLength == other.myLength
         && memcmp(myString, other.myString, (myLength + 1) * sizeof(char16_t)) == 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsDifferent(const char16_t* other) const
{
  return ExtStrCmp(myString, other) != 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsDifferent(const TCollection_ExtendedString& other) const
{
  return myLength != other.myLength
         || memcmp(myString, other.myString, (myLength + 1) * sizeof(char16_t)) != 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsLess(const char16_t* other) const
{
  return ExtStrCmp(myString, other) < 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsLess(const TCollection_ExtendedString& other) const
{
  return ExtStrCmp(myString, other.myString) < 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsGreater(const char16_t* other) const
{
  return ExtStrCmp(myString, other) > 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsGreater(const TCollection_ExtendedString& other) const
{
  return ExtStrCmp(myString, other.myString) > 0;
}

//=================================================================================================

bool TCollection_ExtendedString::StartsWith(const TCollection_ExtendedString& theStartString) const
{
  if (this == &theStartString)
  {
    return true;
  }

  return myLength >= theStartString.myLength
         && memcmp(theStartString.myString, myString, theStartString.myLength * sizeof(char16_t))
              == 0;
}

//=================================================================================================

bool TCollection_ExtendedString::EndsWith(const TCollection_ExtendedString& theEndString) const
{
  if (this == &theEndString)
  {
    return true;
  }

  return myLength >= theEndString.myLength
         && memcmp(theEndString.myString,
                   myString + myLength - theEndString.myLength,
                   theEndString.myLength * sizeof(char16_t))
              == 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsAscii() const
{
  for (int i = 0; i < myLength; i++)
    if (!IsAnAscii(myString[i]))
      return false;
  return true;
}

//=================================================================================================

int TCollection_ExtendedString::Length() const
{
  return myLength;
}

//=================================================================================================

void TCollection_ExtendedString::Print(Standard_OStream& theStream) const
{
  if (myLength > 0)
  {
    const TCollection_AsciiString aUtf8(myString);
    theStream << aUtf8;
  }
}

// ----------------------------------------------------------------------------
Standard_OStream& operator<<(Standard_OStream& astream, const TCollection_ExtendedString& astring)
{
  astring.Print(astream);
  return astream;
}

//=================================================================================================

void TCollection_ExtendedString::RemoveAll(const char16_t what)

{
  if (myLength == 0)
    return;
  int c = 0;
  for (int i = 0; i < myLength; i++)
    if (myString[i] != what)
      myString[c++] = myString[i];
  myLength           = c;
  myString[myLength] = '\0';
}

//=================================================================================================

void TCollection_ExtendedString::Remove(const int where, const int ahowmany)
{
  if (where < 1 || where > myLength || ahowmany < 0 || where + ahowmany > myLength + 1)
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::Remove: "
                              "Invalid parameters");
  }

  if (ahowmany == 0)
  {
    return;
  }

  for (int i = where + ahowmany - 1, j = where - 1; i < myLength; i++, j++)
  {
    myString[j] = myString[i];
  }
  myLength -= ahowmany;
  myString[myLength] = '\0';
}

//=================================================================================================

int TCollection_ExtendedString::Search(const TCollection_ExtendedString& what) const
{
  int             size  = what.myLength;
  const char16_t* swhat = what.myString;
  if (size)
  {
    int  k, j;
    int  i    = 0;
    bool find = false;
    while (i < myLength - size + 1 && !find)
    {
      k = i++;
      j = 0;
      while (j < size && myString[k++] == swhat[j++])
        if (j == size)
          find = true;
    }
    if (find)
      return i;
  }
  return -1;
}

//=================================================================================================

int TCollection_ExtendedString::SearchFromEnd(const TCollection_ExtendedString& what) const
{
  int size = what.myLength;
  if (size)
  {
    const char16_t* swhat = what.myString;
    int             k, j;
    int             i    = myLength - 1;
    bool            find = false;
    while (i >= size - 1 && !find)
    {
      k = i--;
      j = size - 1;
      while (j >= 0 && myString[k--] == swhat[j--])
        if (j == -1)
          find = true;
    }
    if (find)
      return i - size + 3;
  }
  return -1;
}

//=================================================================================================

void TCollection_ExtendedString::SetValue(const int where, const char16_t what)
{
  if (where > 0 && where <= myLength)
  {
    myString[where - 1] = what;
  }
  else
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::SetValue : parameter where");
  }
}

//=================================================================================================

void TCollection_ExtendedString::SetValue(const int where, const TCollection_ExtendedString& what)
{
  if (where > 0 && where <= myLength + 1)
  {
    int             size  = what.myLength;
    const char16_t* swhat = what.myString;
    size += (where - 1);
    if (size >= myLength)
    {
      reallocate(size);
    }
    for (int i = where - 1; i < size; i++)
      myString[i] = swhat[i - (where - 1)];
  }
  else
    throw Standard_OutOfRange("TCollection_ExtendedString::SetValue : "
                              "parameter where");
}

//=================================================================================================

TCollection_ExtendedString TCollection_ExtendedString::Split(const int where)
{
  if (where >= 0 && where < myLength)
  {
    TCollection_ExtendedString res(&myString[where]);
    Trunc(where);
    return res;
  }
  throw Standard_OutOfRange("TCollection_ExtendedString::Split index");
}

//=================================================================================================

TCollection_ExtendedString TCollection_ExtendedString::Token(const char16_t* separators,
                                                             const int       whichone) const
{
  if (myLength == 0)
  {
    return TCollection_ExtendedString();
  }
  TCollection_ExtendedString res(myLength, 0);
  if (!separators)
    throw Standard_NullObject("TCollection_ExtendedString::Token : "
                              "parameter 'separators'");

  int                    i, j, k, l;
  Standard_PExtCharacter buftmp = res.myString;
  char16_t               aSep;

  bool isSepFound = false, otherSepFound;

  j = 0;

  for (i = 0; i < whichone && j < myLength; i++)
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
      aSep          = separators[l];
      while (aSep != 0)
      {
        if (aSep == myString[j])
        {
          aSep          = 0;
          otherSepFound = true;
        }
        else
        {
          aSep = separators[l++];
        }
      }
      if (otherSepFound)
        j++;
    }

    while (!isSepFound && k < myLength && j < myLength)
    {
      l    = 0;
      aSep = separators[l];

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
        aSep = separators[l];
      }
      j++;
      k++;
      if (j == myLength)
        buftmp[k] = 0;
    }
  }

  if (i < whichone)
  {
    res.myLength    = 0;
    res.myString[0] = 0;
  }
  else
  {
    for (res.myLength = 0; buftmp[res.myLength]; ++res.myLength)
      ;
    res.myString[res.myLength] = '\0';
  }
  return res;
}

//=================================================================================================

const char16_t* TCollection_ExtendedString::ToExtString() const
{
  return myString;
}

//=================================================================================================

void TCollection_ExtendedString::Trunc(const int ahowmany)
{
  if (ahowmany < 0 || ahowmany > myLength)
    throw Standard_OutOfRange("TCollection_ExtendedString::Trunc : "
                              "parameter 'ahowmany'");
  myLength           = ahowmany;
  myString[myLength] = '\0';
}

//=================================================================================================

char16_t TCollection_ExtendedString::Value(const int where) const
{
  if (where > 0 && where <= myLength)
  {
    if (myString)
      return myString[where - 1];
    else
      return 0;
  }
  throw Standard_OutOfRange("TCollection_ExtendedString::Value : "
                            "parameter where");
}

//=================================================================================================

bool TCollection_ExtendedString::ConvertToUnicode(const char* theStringUtf)
{
  NCollection_UtfIterator<char> anIterRead(theStringUtf);
  char16_t*                     anIterWrite = myString;
  if (*anIterRead == 0)
  {
    *anIterWrite = '\0';
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

//=================================================================================================

int TCollection_ExtendedString::LengthOfCString() const
{
  int aSizeBytes = 0;
  for (NCollection_UtfIterator<char16_t> anIter(myString); *anIter != 0; ++anIter)
  {
    aSizeBytes += anIter.AdvanceBytesUtf8();
  }
  return aSizeBytes;
}

//----------------------------------------------------------------------------
// Converts the internal <myString> to UTF8 coding and returns length of the
// out CString.
//----------------------------------------------------------------------------
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

//=================================================================================================

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
    myString           = static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
    myString[myLength] = '\0';
  }
}

//=================================================================================================

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
    myString = static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
  }
  else
  {
    const size_t aRoundSize = calculatePaddedSize(theLength);
    myString = static_cast<Standard_PExtCharacter>(Standard::Reallocate(myString, aRoundSize));
  }
  myString[theLength] = 0;
  myLength            = theLength;
}

//=================================================================================================

void TCollection_ExtendedString::deallocate()
{
  if (myString != THE_DEFAULT_EXT_CHAR_STRING)
  {
    Standard::Free(myString);
  }
  myLength = 0;
  myString = THE_DEFAULT_EXT_CHAR_STRING;
}

//=================================================================================================

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
  while (anIndex < myLength && myString[anIndex] == ' ')
  {
    ++anIndex;
  }

  if (anIndex > 0)
  {
    myLength -= anIndex;
    memmove(myString, myString + anIndex, myLength * sizeof(char16_t));
    myString[myLength] = '\0';
  }
}

//==================================================================================================

void TCollection_ExtendedString::RightAdjust()
{
  if (myLength == 0)
  {
    return;
  }

  while (myLength > 0 && myString[myLength - 1] == ' ')
  {
    --myLength;
  }
  myString[myLength] = '\0';
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

  // Convert first character to uppercase
  if (myString[0] >= 'a' && myString[0] <= 'z')
  {
    myString[0] = static_cast<char16_t>(myString[0] - 'a' + 'A');
  }

  // Convert remaining characters to lowercase
  for (int i = 1; i < myLength; ++i)
  {
    if (myString[i] >= 'A' && myString[i] <= 'Z')
    {
      myString[i] = static_cast<char16_t>(myString[i] - 'A' + 'a');
    }
  }
}

//==================================================================================================

void TCollection_ExtendedString::Prepend(const TCollection_ExtendedString& theOther)
{
  if (theOther.myLength == 0)
  {
    return;
  }

  const int anOtherLength = theOther.myLength;
  const int anOldLength   = myLength;
  reallocate(myLength + anOtherLength);

  // Move existing characters to the end
  memmove(myString + anOtherLength, myString, anOldLength * sizeof(char16_t));
  // Copy theOther to the beginning
  memcpy(myString, theOther.myString, anOtherLength * sizeof(char16_t));
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

  // Case-insensitive comparison
  for (int i = 0; i < myLength; ++i)
  {
    char16_t c1 = myString[i];
    char16_t c2 = theOther.myString[i];

    // Convert to lowercase for comparison (ASCII only)
    if (c1 >= 'A' && c1 <= 'Z')
    {
      c1 = static_cast<char16_t>(c1 - 'A' + 'a');
    }
    if (c2 >= 'A' && c2 <= 'Z')
    {
      c2 = static_cast<char16_t>(c2 - 'A' + 'a');
    }

    if (c1 != c2)
    {
      return false;
    }
  }
  return true;
}
