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
inline char16_t* Standard_UNUSED fromWideString(const wchar_t* theUtfString,
                                                             int&        theLength)
{
  theLength = nbSymbols(theUtfString);
  if (theLength == 0)
  {
    return THE_DEFAULT_EXT_CHAR_STRING;
  }
  const size_t    aRoundSize = calculatePaddedSize(theLength);
  char16_t* aString =
    static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
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
inline char16_t* Standard_UNUSED
  fromWideString<sizeof(char16_t)>(const wchar_t* theUtfString,
                                                int&        theLength)
{
  for (theLength = 0; theUtfString[theLength] != 0; ++theLength)
  {
  }
  if (theLength == 0)
  {
    return THE_DEFAULT_EXT_CHAR_STRING;
  }
  const size_t    aRoundSize = calculatePaddedSize(theLength);
  char16_t* aString =
    static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
  const int aSize = theLength * sizeof(char16_t);
  memcpy(aString, theUtfString, aSize);
  aString[theLength] = 0;
  return aString;
}

} // namespace

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString() noexcept
{
  allocate(0);
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char* theString,
                                                       const bool isMultiByte)
{
  if (theString == NULL)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  if (isMultiByte)
  {
    allocate(nbSymbols(theString));
    mystring[mylength] = 0;
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
  for (int aCharIter = 0; aCharIter < mylength; ++aCharIter)
  {
    mystring[aCharIter] = ToExtCharacter(theString[aCharIter]);
  }
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char16_t* theString)
{
  if (theString == NULL)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  for (mylength = 0; theString[mylength] != 0; ++mylength)
  {
  }
  allocate(mylength);
  memcpy(mystring, theString, mylength * sizeof(char16_t));
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const wchar_t* theStringUtf)
{
  if (theStringUtf == NULL)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  mystring = fromWideString<sizeof(wchar_t)>(theStringUtf, mylength);
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char aChar)
{
  if (aChar != '\0')
  {
    allocate(1);
    mystring[0] = ToExtCharacter(aChar);
  }
  else
  {
    allocate(0);
  }
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const char16_t aChar)
{
  allocate(1);
  mystring[0] = aChar;
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const int      length,
                                                       const char16_t filler)
{
  allocate(length);
  for (int i = 0; i < length; i++)
    mystring[i] = filler;
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const int aValue)
{
  union {
    int  bid;
    char t[13];
  } CHN{};

  Sprintf(&CHN.t[0], "%d", aValue);
  allocate((int)strlen(CHN.t));
  for (int i = 0; i < mylength; i++)
    mystring[i] = ToExtCharacter(CHN.t[i]);
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const double aValue)
{
  union {
    int  bid;
    char t[50];
  } CHN{};

  Sprintf(&CHN.t[0], "%g", aValue);
  allocate((int)strlen(CHN.t));
  for (int i = 0; i < mylength; i++)
    mystring[i] = ToExtCharacter(CHN.t[i]);
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const TCollection_ExtendedString& theOther)
{
  allocate(theOther.mylength);
  memcpy(mystring, theOther.mystring, mylength * sizeof(char16_t));
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(
  TCollection_ExtendedString&& theOther) noexcept
{
  if (theOther.mystring == THE_DEFAULT_EXT_CHAR_STRING)
  {
    allocate(0);
  }
  else
  {
    mystring = theOther.mystring;
    mylength = theOther.mylength;
  }
  theOther.mystring = THE_DEFAULT_EXT_CHAR_STRING;
  theOther.mylength = 0;
}

//=================================================================================================

TCollection_ExtendedString::TCollection_ExtendedString(const TCollection_AsciiString& theString,
                                                       const bool         isMultiByte)
{
  allocate(nbSymbols(theString.ToCString()));
  if (isMultiByte && ConvertToUnicode(theString.ToCString()))
  {
    return;
  }
  reallocate(theString.Length());
  const char* aCString = theString.ToCString();
  for (int aCharIter = 0; aCharIter <= mylength; ++aCharIter)
  {
    mystring[aCharIter] = ToExtCharacter(aCString[aCharIter]);
  }
}

//=================================================================================================

void TCollection_ExtendedString::AssignCat(const TCollection_ExtendedString& theOther)
{
  if (theOther.mylength == 0)
  {
    return;
  }

  const int anOtherLength = theOther.mylength;
  const int anOldLength   = mylength;
  reallocate(mylength + anOtherLength);
  memcpy(mystring + anOldLength, theOther.mystring, anOtherLength * sizeof(char16_t));
}

//=================================================================================================

void TCollection_ExtendedString::AssignCat(const char16_t theChar)
{
  if (theChar != '\0')
  {
    reallocate(mylength + 1);
    mystring[mylength - 1] = theChar;
  }
}

//=================================================================================================

TCollection_ExtendedString TCollection_ExtendedString::Cat(
  const TCollection_ExtendedString& other) const
{
  TCollection_ExtendedString res(mylength + other.mylength, 0);
  if (mylength > 0)
    memcpy(res.mystring, mystring, mylength * sizeof(char16_t));
  if (other.mylength > 0)
    memcpy(res.mystring + mylength, other.mystring, other.mylength * sizeof(char16_t));
  return res;
}

//=================================================================================================

void TCollection_ExtendedString::ChangeAll(const char16_t aChar,
                                           const char16_t NewChar)
{
  for (int i = 0; i < mylength; i++)
    if (mystring[i] == aChar)
      mystring[i] = NewChar;
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
  if (fromwhere.mystring && fromwhere.mylength > 0)
  {
    reallocate(fromwhere.mylength);
    memcpy(mystring, fromwhere.mystring, mylength * sizeof(char16_t));
  }
  else if (mystring != THE_DEFAULT_EXT_CHAR_STRING)
  {
    mylength           = 0;
    mystring[mylength] = 0;
  }
}

//=================================================================================================

void TCollection_ExtendedString::Move(TCollection_ExtendedString&& theOther)
{
  if (&theOther == this)
  {
    return;
  }
  if (theOther.mystring == THE_DEFAULT_EXT_CHAR_STRING)
  {
    reallocate(0);
  }
  else
  {
    if (mystring != THE_DEFAULT_EXT_CHAR_STRING)
    {
      Standard::Free(mystring);
    }
    mystring = theOther.mystring;
    mylength = theOther.mylength;
  }
  theOther.mystring = THE_DEFAULT_EXT_CHAR_STRING;
  theOther.mylength = 0;
}

//=================================================================================================

void TCollection_ExtendedString::Swap(TCollection_ExtendedString& theOther)
{
  if (&theOther == this)
  {
    return;
  }
  std::swap(mystring, theOther.mystring);
  std::swap(mylength, theOther.mylength);
}

//=================================================================================================

TCollection_ExtendedString::~TCollection_ExtendedString()
{
  deallocate();
}

//=================================================================================================

void TCollection_ExtendedString::Insert(const int      where,
                                        const char16_t what)
{
  if (where > mylength + 1)
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter where is too big");
  if (where < 0)
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter where is negative");
  reallocate(mylength + 1);
  if (where != mylength)
  {
    for (int i = mylength - 2; i >= where - 1; i--)
      mystring[i + 1] = mystring[i];
  }
  mystring[where - 1] = what;
}

//=================================================================================================

void TCollection_ExtendedString::Insert(const int            where,
                                        const TCollection_ExtendedString& what)
{
  const char16_t* swhat = what.mystring;
  if (where <= mylength + 1)
  {
    int whatlength = what.mylength;
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
  return ExtStrCmp(mystring, other) == 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsEqual(const TCollection_ExtendedString& other) const
{
  return mylength == other.mylength
         && memcmp(mystring, other.mystring, (mylength + 1) * sizeof(char16_t)) == 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsDifferent(const char16_t* other) const
{
  return ExtStrCmp(mystring, other) != 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsDifferent(
  const TCollection_ExtendedString& other) const
{
  return mylength != other.mylength
         || memcmp(mystring, other.mystring, (mylength + 1) * sizeof(char16_t)) != 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsLess(const char16_t* other) const
{
  return ExtStrCmp(mystring, other) < 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsLess(const TCollection_ExtendedString& other) const
{
  return ExtStrCmp(mystring, other.mystring) < 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsGreater(const char16_t* other) const
{
  return ExtStrCmp(mystring, other) > 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsGreater(
  const TCollection_ExtendedString& other) const
{
  return ExtStrCmp(mystring, other.mystring) > 0;
}

//=================================================================================================

bool TCollection_ExtendedString::StartsWith(
  const TCollection_ExtendedString& theStartString) const
{
  if (this == &theStartString)
  {
    return true;
  }

  return mylength >= theStartString.mylength
         && memcmp(theStartString.mystring,
                   mystring,
                   theStartString.mylength * sizeof(char16_t))
              == 0;
}

//=================================================================================================

bool TCollection_ExtendedString::EndsWith(
  const TCollection_ExtendedString& theEndString) const
{
  if (this == &theEndString)
  {
    return true;
  }

  return mylength >= theEndString.mylength
         && memcmp(theEndString.mystring,
                   mystring + mylength - theEndString.mylength,
                   theEndString.mylength * sizeof(char16_t))
              == 0;
}

//=================================================================================================

bool TCollection_ExtendedString::IsAscii() const
{
  for (int i = 0; i < mylength; i++)
    if (!IsAnAscii(mystring[i]))
      return false;
  return true;
}

//=================================================================================================

int TCollection_ExtendedString::Length() const
{
  return mylength;
}

//=================================================================================================

void TCollection_ExtendedString::Print(Standard_OStream& theStream) const
{
  if (mylength > 0)
  {
    const TCollection_AsciiString aUtf8(mystring);
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
  if (mylength == 0)
    return;
  int c = 0;
  for (int i = 0; i < mylength; i++)
    if (mystring[i] != what)
      mystring[c++] = mystring[i];
  mylength           = c;
  mystring[mylength] = '\0';
}

//=================================================================================================

void TCollection_ExtendedString::Remove(const int where,
                                        const int ahowmany)
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
    throw Standard_OutOfRange("TCollection_ExtendedString::Remove: "
                              "Too many characters to erase or "
                              "invalid starting value.");
}

//=================================================================================================

int TCollection_ExtendedString::Search(const TCollection_ExtendedString& what) const
{
  int   size  = what.mylength;
  const char16_t* swhat = what.mystring;
  if (size)
  {
    int              k, j;
    int              i    = 0;
    bool find = false;
    while (i < mylength - size + 1 && !find)
    {
      k = i++;
      j = 0;
      while (j < size && mystring[k++] == swhat[j++])
        if (j == size)
          find = true;
    }
    if (find)
      return i;
  }
  return -1;
}

//=================================================================================================

int TCollection_ExtendedString::SearchFromEnd(
  const TCollection_ExtendedString& what) const
{
  int size = what.mylength;
  if (size)
  {
    const char16_t* swhat = what.mystring;
    int                k, j;
    int                i    = mylength - 1;
    bool   find = false;
    while (i >= size - 1 && !find)
    {
      k = i--;
      j = size - 1;
      while (j >= 0 && mystring[k--] == swhat[j--])
        if (j == -1)
          find = true;
    }
    if (find)
      return i - size + 3;
  }
  return -1;
}

//=================================================================================================

void TCollection_ExtendedString::SetValue(const int      where,
                                          const char16_t what)
{
  if (where > 0 && where <= mylength)
  {
    mystring[where - 1] = what;
  }
  else
  {
    throw Standard_OutOfRange("TCollection_ExtendedString::SetValue : parameter where");
  }
}

//=================================================================================================

void TCollection_ExtendedString::SetValue(const int            where,
                                          const TCollection_ExtendedString& what)
{
  if (where > 0 && where <= mylength + 1)
  {
    int   size  = what.mylength;
    const char16_t* swhat = what.mystring;
    size += (where - 1);
    if (size >= mylength)
    {
      reallocate(size);
    }
    for (int i = where - 1; i < size; i++)
      mystring[i] = swhat[i - (where - 1)];
  }
  else
    throw Standard_OutOfRange("TCollection_ExtendedString::SetValue : "
                              "parameter where");
}

//=================================================================================================

TCollection_ExtendedString TCollection_ExtendedString::Split(const int where)
{
  if (where >= 0 && where < mylength)
  {
    TCollection_ExtendedString res(&mystring[where]);
    Trunc(where);
    return res;
  }
  throw Standard_OutOfRange("TCollection_ExtendedString::Split index");
}

//=================================================================================================

TCollection_ExtendedString TCollection_ExtendedString::Token(const char16_t* separators,
                                                             const int whichone) const
{
  if (mylength == 0)
  {
    return TCollection_ExtendedString();
  }
  TCollection_ExtendedString res(mylength, 0);
  if (!separators)
    throw Standard_NullObject("TCollection_ExtendedString::Token : "
                              "parameter 'separators'");

  int                    i, j, k, l;
  Standard_PExtCharacter buftmp = res.mystring;
  char16_t  aSep;

  bool isSepFound = false, otherSepFound;

  j = 0;

  for (i = 0; i < whichone && j < mylength; i++)
  {
    isSepFound = false;
    k          = 0;
    buftmp[0]  = 0;

    // Avant de commencer il faut virer les saloperies devant
    //
    otherSepFound = true;
    while (j < mylength && otherSepFound)
    {
      l             = 0;
      otherSepFound = false;
      aSep          = separators[l];
      while (aSep != 0)
      {
        if (aSep == mystring[j])
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

    while (!isSepFound && k < mylength && j < mylength)
    {
      l    = 0;
      aSep = separators[l];

      while (aSep != 0 && !isSepFound)
      {
        if (aSep == mystring[j])
        {
          buftmp[k]  = 0;
          isSepFound = true;
        }
        else
        {
          buftmp[k] = mystring[j];
        }
        l++;
        aSep = separators[l];
      }
      j++;
      k++;
      if (j == mylength)
        buftmp[k] = 0;
    }
  }

  if (i < whichone)
  {
    res.mylength    = 0;
    res.mystring[0] = 0;
  }
  else
  {
    for (res.mylength = 0; buftmp[res.mylength]; ++res.mylength)
      ;
    res.mystring[res.mylength] = '\0';
  }
  return res;
}

//=================================================================================================

const char16_t* TCollection_ExtendedString::ToExtString() const
{
  return mystring;
}

//=================================================================================================

void TCollection_ExtendedString::Trunc(const int ahowmany)
{
  if (ahowmany < 0 || ahowmany > mylength)
    throw Standard_OutOfRange("TCollection_ExtendedString::Trunc : "
                              "parameter 'ahowmany'");
  mylength           = ahowmany;
  mystring[mylength] = '\0';
}

//=================================================================================================

char16_t TCollection_ExtendedString::Value(const int where) const
{
  if (where > 0 && where <= mylength)
  {
    if (mystring)
      return mystring[where - 1];
    else
      return 0;
  }
  throw Standard_OutOfRange("TCollection_ExtendedString::Value : "
                            "parameter where");
}

//=================================================================================================

bool TCollection_ExtendedString::ConvertToUnicode(const char* theStringUtf)
{
  NCollection_UtfIterator<char>   anIterRead(theStringUtf);
  char16_t* anIterWrite = mystring;
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
  for (NCollection_UtfIterator<char16_t> anIter(mystring); *anIter != 0; ++anIter)
  {
    aSizeBytes += anIter.AdvanceBytesUtf8();
  }
  return aSizeBytes;
}

//----------------------------------------------------------------------------
// Converts the internal <mystring> to UTF8 coding and returns length of the
// out CString.
//----------------------------------------------------------------------------
int TCollection_ExtendedString::ToUTF8CString(Standard_PCharacter& theCString) const
{
  NCollection_UtfIterator<char16_t> anIterRead(mystring);
  char*    anIterWrite = theCString;
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
  mylength = theLength;
  if (theLength == 0)
  {
    mystring = THE_DEFAULT_EXT_CHAR_STRING;
  }
  else
  {
    const size_t aRoundSize = calculatePaddedSize(theLength);
    mystring           = static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
    mystring[mylength] = '\0';
  }
}

//=================================================================================================

void TCollection_ExtendedString::reallocate(const int theLength)
{
  if (theLength != 0)
  {
    if (mystring == THE_DEFAULT_EXT_CHAR_STRING)
    {
      const size_t aRoundSize = calculatePaddedSize(theLength);
      mystring = static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
    }
    else
    {
      // For reallocate, use padded size for consistency
      const size_t aRoundSize = calculatePaddedSize(theLength);
      mystring = static_cast<Standard_PExtCharacter>(Standard::Reallocate(mystring, aRoundSize));
    }
    mystring[theLength] = 0;
  }
  if (mystring != THE_DEFAULT_EXT_CHAR_STRING)
  {
    mystring[theLength] = 0;
  }
  mylength = theLength;
}

//=================================================================================================

void TCollection_ExtendedString::deallocate()
{
  if (mystring != THE_DEFAULT_EXT_CHAR_STRING)
  {
    Standard::Free(mystring);
  }
  mylength = 0;
  mystring = THE_DEFAULT_EXT_CHAR_STRING;
}

//=================================================================================================

const TCollection_ExtendedString& TCollection_ExtendedString::EmptyString() noexcept
{
  static const TCollection_ExtendedString THE_EMPTY_STRING;
  return THE_EMPTY_STRING;
}
