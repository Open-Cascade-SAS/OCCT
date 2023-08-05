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
#include <TCollection_AsciiString.hxx>

#include <algorithm>

namespace
{
  static Standard_ExtCharacter THE_DEFAULT_EXT_CHAR_STRING[1] = {0};

  //! Returns the number of 16-bit code units in Unicode string
  template<typename T>
  static Standard_Integer nbSymbols (const T* theUtfString)
  {
    Standard_Integer aNbCodeUnits = 0;
    for (NCollection_UtfIterator<T> anIter (theUtfString); *anIter != 0; ++anIter)
    {
      aNbCodeUnits += anIter.AdvanceCodeUnitsUtf16();
    }
    return aNbCodeUnits;
  }

  //! Convert from wchar_t* to extended string.
  //! Default implementation when size of wchar_t and extended char is different (e.g. Linux / UNIX).
  template<size_t CharSize>
  inline Standard_ExtCharacter* Standard_UNUSED fromWideString (const Standard_WideChar* theUtfString,
                                                                Standard_Integer& theLength)
  {
    theLength = nbSymbols (theUtfString);
    if (theLength == 0)
    {
      return THE_DEFAULT_EXT_CHAR_STRING;
    }
    const Standard_Size aRoundSize = (((theLength + 1) * sizeof(Standard_ExtCharacter)) + 3) & ~0x3;
    Standard_ExtCharacter* aString = static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
    NCollection_UtfWideIter anIterRead (theUtfString);
    for (Standard_ExtCharacter* anIterWrite = aString; *anIterRead != 0; ++anIterRead)
    {
      anIterWrite = anIterRead.GetUtf (anIterWrite);
    }
    aString[theLength] = 0;
    return aString;
  }

  //! Use memcpy() conversion when size is the same (e.g. on Windows).
  template<>
  inline Standard_ExtCharacter* Standard_UNUSED fromWideString<sizeof(Standard_ExtCharacter)> (const Standard_WideChar* theUtfString,
                                                                                               Standard_Integer& theLength)
  {
    for (theLength = 0; theUtfString[theLength] != 0; ++theLength) {}
    if (theLength == 0)
    {
      return THE_DEFAULT_EXT_CHAR_STRING;
    } 
    const Standard_Size aRoundSize = (((theLength + 1) * sizeof(Standard_ExtCharacter)) + 3) & ~0x3;
    Standard_ExtCharacter* aString = static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
    const Standard_Integer aSize = theLength * sizeof(Standard_ExtCharacter);
    memcpy (aString, theUtfString, aSize);
    aString[theLength] = 0;
    return aString;
  }

}

//-----------------------------------------------------------------------------
// Create an empty ExtendedString
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString()
{
  allocate(0);
}

//----------------------------------------------------------------------------
// Create an ExtendedString from a Standard_CString
//----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                          (const Standard_CString theString,
                                           const Standard_Boolean isMultiByte)
{
  if (theString == NULL)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  if (isMultiByte)
  {
    allocate(nbSymbols(theString));
    mystring[mylength] = 0;
    if (ConvertToUnicode (theString))
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
    mystring[aCharIter] = ToExtCharacter (theString[aCharIter]);
  }
}

//---------------------------------------------------------------------------
// Create an ExtendedString from an ExtString
//--------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString (const Standard_ExtString theString)
{
  if (theString == NULL)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  for (mylength = 0; theString[mylength] != 0; ++mylength) {}
  allocate (mylength);
  memcpy (mystring, theString, mylength * sizeof(Standard_ExtCharacter));
}

// ----------------------------------------------------------------------------
// TCollection_ExtendedString
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString (const Standard_WideChar* theStringUtf)
{
  if (theStringUtf == NULL)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  mystring = fromWideString<sizeof(Standard_WideChar)> (theStringUtf, mylength);
}

// ----------------------------------------------------------------------------
// Create an asciistring from a Standard_Character
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_Character aChar)
{
  if ( aChar != '\0' )
  {
    allocate(1);
    mystring[0] = ToExtCharacter(aChar);
  }
  else
  {
    allocate(0);
  }
}

//--------------------------------------------------------------------------
// Create a string from a ExtCharacter
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_ExtCharacter aChar)
{
  allocate(1);
  mystring[0] = aChar;
}

// ----------------------------------------------------------------------------
// Create an AsciiString from a filler
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_Integer      length,
                                         const Standard_ExtCharacter filler )
{
  allocate(length);
  for (int i = 0 ; i < length ; i++) mystring[i] = filler;
}

// ----------------------------------------------------------------------------
// Create a String from an Integer
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_Integer aValue)
{
  union {int bid ;
         char t [13];} CHN ;
  Sprintf(&CHN.t[0],"%d",aValue);
  allocate((int)strlen(CHN.t));
  for (int i = 0 ; i < mylength ; i++) mystring[i] = ToExtCharacter(CHN.t[i]);
}

// ----------------------------------------------------------------------------
// Create a String from a real
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_Real aValue)
{
  union {int bid ;
         char t [50];} CHN ;
  Sprintf(&CHN.t[0],"%g",aValue);
  allocate((int)strlen(CHN.t));
  for (int i = 0 ; i < mylength ; i++) mystring[i] = ToExtCharacter(CHN.t[i]);
}

//-----------------------------------------------------------------------------
// create an extendedstring from an extendedstring
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                (const TCollection_ExtendedString& theOther)
{
  allocate(theOther.mylength);
  memcpy(mystring, theOther.mystring, mylength * sizeof(Standard_ExtCharacter));
}

//=======================================================================
//function : TCollection_ExtendedString
//purpose  :
//=======================================================================
TCollection_ExtendedString::TCollection_ExtendedString (TCollection_ExtendedString&& theOther) noexcept
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

//---------------------------------------------------------------------------
//  Create an extendedstring from an AsciiString 
//---------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                (const TCollection_AsciiString& theString,
                                 const Standard_Boolean isMultiByte)
{
  allocate(nbSymbols (theString.ToCString()));
  if (isMultiByte && ConvertToUnicode (theString.ToCString()))
  {
    return;
  }
  reallocate(theString.Length());
  Standard_CString aCString = theString.ToCString();
  for (Standard_Integer aCharIter = 0; aCharIter <= mylength; ++aCharIter)
  {
    mystring[aCharIter] = ToExtCharacter (aCString[aCharIter]);
  }
}

// ----------------------------------------------------------------------------
//  AssignCat
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::AssignCat (const TCollection_ExtendedString& theOther)
{
  if (theOther.mylength == 0)
  {
    return;
  }

  const Standard_Integer anOtherLength = theOther.mylength;
  const Standard_Integer anOldLength   = mylength;
  reallocate(mylength + anOtherLength);
  memcpy (mystring + anOldLength, theOther.mystring, anOtherLength * sizeof(Standard_ExtCharacter));
}

// ----------------------------------------------------------------------------
//  AssignCat
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::AssignCat(const Standard_Utf16Char theChar)
{
  if (theChar != '\0')
  {
    reallocate(mylength + 1);
    mystring[mylength - 1] = theChar;
  }  
}

// ----------------------------------------------------------------------------
// Cat
// ----------------------------------------------------------------------------
TCollection_ExtendedString TCollection_ExtendedString::Cat
                                (const TCollection_ExtendedString& other) const 
{
  TCollection_ExtendedString res( mylength + other.mylength, 0 );
  if ( mylength > 0 )
    memcpy (res.mystring, mystring, mylength * sizeof(Standard_ExtCharacter));
  if ( other.mylength > 0 )
    memcpy (res.mystring + mylength, other.mystring, other.mylength * sizeof(Standard_ExtCharacter));
  return res;
}

// ----------------------------------------------------------------------------
// ChangeAll
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::ChangeAll(const Standard_ExtCharacter aChar,
                                           const Standard_ExtCharacter NewChar)
{
  for (int i = 0 ; i < mylength; i++)
    if (mystring[i] == aChar) mystring[i] = NewChar;
}

// ----------------------------------------------------------------------------
// Clear
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Clear()
{
  deallocate();
}

// ----------------------------------------------------------------------------
// Copy
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Copy(const TCollection_ExtendedString& fromwhere)
{
  if (&fromwhere == this)
  {
    return;
  }
  if (fromwhere.mystring && fromwhere.mylength > 0)
  {
    reallocate (fromwhere.mylength);
    memcpy (mystring, fromwhere.mystring, mylength * sizeof(Standard_ExtCharacter));
  }
  else if (mystring != THE_DEFAULT_EXT_CHAR_STRING)
  {
    mylength = 0;
    mystring[mylength] = 0;
  }
}

//=======================================================================
//function : Move
//purpose  :
//=======================================================================
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

// ----------------------------------------------------------------------------
// Swap
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Swap (TCollection_ExtendedString& theOther)
{
  if (&theOther == this)
  {
    return;
  }
  std::swap(mystring, theOther.mystring);
  std::swap (mylength, theOther.mylength);
}

// ----------------------------------------------------------------------------
// Destroy
// ----------------------------------------------------------------------------
TCollection_ExtendedString::~TCollection_ExtendedString()
{
  deallocate ();
}

//----------------------------------------------------------------------------
// Insert a character before 'where'th character
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Insert(const Standard_Integer where,
                                        const Standard_ExtCharacter what)
{
  if (where > mylength + 1 )
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter where is too big");
  if (where < 0)
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter where is negative");
  reallocate(mylength + 1);
  if (where != mylength) {
    for (int i=mylength-2; i >= where-1; i--)
      mystring[i+1] = mystring[i];
  }
  mystring[where-1] = what;
}

// ----------------------------------------------------------------------------
// Insert
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Insert(const Standard_Integer            where,
                                        const TCollection_ExtendedString& what)
{
  Standard_ExtString swhat = what.mystring;
  if (where <= mylength + 1) {
    Standard_Integer whatlength = what.mylength;
    if(whatlength) {
      const int anOldLength = mylength;
      reallocate(mylength + whatlength);
      if (where != anOldLength +1) {
        for (int i=anOldLength-1; i >= where-1; i--)
          mystring[i+whatlength] = mystring[i];
      }
      for (int i=0; i < whatlength; i++)
        mystring[where-1+i] = swhat[i];
    }
  }
  else {
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter where is too big");
  }
}

// ----------------------------------------------------------------------------
// ExtStrCmp: strcmp for 16-bit chars
// ----------------------------------------------------------------------------
static int ExtStrCmp (const Standard_ExtString theStr1, const Standard_ExtString theStr2)
{
  const Standard_ExtCharacter* aStr1 = theStr1;
  const Standard_ExtCharacter* aStr2 = theStr2;
  while (*aStr1 != 0 && *aStr1 == *aStr2)
  {
    aStr1++;
    aStr2++;
  }
  // Standard_ExtCharacter is char16_t which is 2-byte unsigned int, it will be promoted 
  // to plain 4-byte int for calculation
  return *aStr1 - *aStr2;
}

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsEqual
                                (const Standard_ExtString other) const
{
  return ExtStrCmp (mystring, other) == 0;
}

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsEqual
                                (const TCollection_ExtendedString& other) const
{
  return mylength == other.mylength &&
         memcmp (mystring, other.mystring, (mylength + 1) * sizeof(Standard_ExtCharacter)) == 0;
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsDifferent
                                (const Standard_ExtString other ) const
{
  return ExtStrCmp (mystring, other) != 0;
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsDifferent
                                (const TCollection_ExtendedString& other) const
{
  return mylength != other.mylength ||
         memcmp (mystring, other.mystring, (mylength + 1) * sizeof(Standard_ExtCharacter)) != 0;
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsLess
                                (const Standard_ExtString other) const
{
  return ExtStrCmp (mystring, other) < 0;
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsLess
                                (const TCollection_ExtendedString& other) const
{
  return ExtStrCmp (mystring, other.mystring) < 0;
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsGreater
                                (const Standard_ExtString other) const
{
  return ExtStrCmp (mystring, other) > 0;
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsGreater
                                (const TCollection_ExtendedString& other) const
{
  return ExtStrCmp (mystring, other.mystring) > 0;
}

// ----------------------------------------------------------------------------
// StartsWith
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::StartsWith (const TCollection_ExtendedString& theStartString) const
{
  if (this == &theStartString)
  {
    return true;
  }

  return mylength >= theStartString.mylength
      && memcmp (theStartString.mystring, mystring, theStartString.mylength * sizeof(Standard_ExtCharacter)) == 0;
}

// ----------------------------------------------------------------------------
// EndsWith
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::EndsWith (const TCollection_ExtendedString& theEndString) const
{
  if (this == &theEndString)
  {
    return true;
  }

  return mylength >= theEndString.mylength
      && memcmp (theEndString.mystring, mystring + mylength - theEndString.mylength, theEndString.mylength * sizeof(Standard_ExtCharacter)) == 0;
}

// ----------------------------------------------------------------------------
// IsAscii
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsAscii()  const 
{
  for( Standard_Integer i = 0 ; i < mylength ; i++)
    if (!IsAnAscii(mystring[i])) return Standard_False;
  return Standard_True;
}

//------------------------------------------------------------------------
//  Length
// ----------------------------------------------------------------------------
Standard_Integer TCollection_ExtendedString::Length()  const
{
  return mylength;
}

// ----------------------------------------------------------------------------
// Print
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Print (Standard_OStream& theStream) const
{
  if (mylength > 0)
  {
    const TCollection_AsciiString aUtf8 (mystring);
    theStream << aUtf8;
  }
}


// ----------------------------------------------------------------------------
Standard_OStream& operator << (Standard_OStream& astream,
                               const TCollection_ExtendedString& astring)
{
  astring.Print(astream);
  return astream;
}

// ----------------------------------------------------------------------------
// RemoveAll
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::RemoveAll(const Standard_ExtCharacter what)

{
  if (mylength == 0) return;
  int c = 0;
    for (int i=0; i < mylength; i++)
      if (mystring[i] != what) mystring[c++] = mystring[i];
  mylength = c;
  mystring[mylength] =  '\0';
}

// ----------------------------------------------------------------------------
// Remove
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Remove (const Standard_Integer where,
                                         const Standard_Integer ahowmany)
{
  if (where+ahowmany <= mylength+1) {
    int i,j;
    for (i = where+ahowmany-1, j = where-1; i < mylength; i++, j++)
      mystring[j] = mystring[i];
    mylength -= ahowmany;
    mystring[mylength] =  '\0';
  }
  else
    throw Standard_OutOfRange("TCollection_ExtendedString::Remove: "
                              "Too many characters to erase or "
                              "invalid starting value.");
}

// ----------------------------------------------------------------------------
// Search
// ----------------------------------------------------------------------------
Standard_Integer TCollection_ExtendedString::Search
                                (const TCollection_ExtendedString& what) const 
{
  Standard_Integer size = what.mylength;
  Standard_ExtString swhat = what.mystring;  
  if (size) {
    int k,j;
    int i = 0;
    Standard_Boolean find = Standard_False; 
    while ( i < mylength-size+1 && !find) {
      k = i++;
      j = 0;
      while (j < size && mystring[k++] == swhat[j++])
        if (j == size) find = Standard_True;
    }
    if (find)  return i;
  }
  return -1;
}

// ----------------------------------------------------------------------------
// SearchFromEnd
// ----------------------------------------------------------------------------
Standard_Integer TCollection_ExtendedString::SearchFromEnd
                                (const TCollection_ExtendedString& what) const
{
  Standard_Integer size = what.mylength;
  if (size) {
    Standard_ExtString swhat = what.mystring;  
    int k,j;
    int i = mylength-1;
    Standard_Boolean find = Standard_False; 
    while ( i >= size-1 && !find) {
      k = i--;
      j = size-1;
      while (j >= 0 && mystring[k--] == swhat[j--])
        if (j == -1) find = Standard_True;
    }
    if (find)  return i-size+3;
  }
  return -1;
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::SetValue(const Standard_Integer      where,
                                          const Standard_ExtCharacter what)
{
  if (where > 0 && where <= mylength) {
    mystring[where-1] = what;
  }
  else {
    throw Standard_OutOfRange("TCollection_ExtendedString::SetValue : parameter where");
  }
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::SetValue
                                (const Standard_Integer            where,
                                 const TCollection_ExtendedString& what)
{
  if (where > 0 && where <= mylength+1) {
    Standard_Integer size = what.mylength;
    Standard_ExtString swhat = what.mystring;  
    size += (where - 1);  
    if (size >= mylength){
      reallocate(size);
    } 
    for (int i = where-1; i < size; i++)
      mystring[i] = swhat[i-(where-1)];
  }
  else
    throw Standard_OutOfRange("TCollection_ExtendedString::SetValue : "
                              "parameter where");
}

// ----------------------------------------------------------------------------
// Split
// ----------------------------------------------------------------------------
TCollection_ExtendedString TCollection_ExtendedString::Split
                                        (const Standard_Integer where)
{
  if (where >= 0 && where < mylength) {
    TCollection_ExtendedString res(&mystring[where]);
    Trunc(where);
    return res;
  }
  throw Standard_OutOfRange("TCollection_ExtendedString::Split index");
}

// ----------------------------------------------------------------------------
// Token
// ----------------------------------------------------------------------------
TCollection_ExtendedString TCollection_ExtendedString::Token
                                      (const Standard_ExtString separators,
                                       const Standard_Integer   whichone) const
{
  if (mylength == 0)
  {
    return TCollection_ExtendedString();
  }
  TCollection_ExtendedString res(mylength, 0);
  if (!separators)
    throw Standard_NullObject("TCollection_ExtendedString::Token : "
                              "parameter 'separators'");
  
  int                   i,j,k,l;
  Standard_PExtCharacter  buftmp = res.mystring;
  Standard_ExtCharacter aSep;
  
  Standard_Boolean isSepFound   = Standard_False, otherSepFound;
  
  j = 0;
  
  for (i = 0; i < whichone && j < mylength; i++) {
    isSepFound   = Standard_False;
    k = 0;
    buftmp[0] = 0;
    
    // Avant de commencer il faut virer les saloperies devant
    //
    otherSepFound = Standard_True;
    while (j < mylength && otherSepFound) {
      l    = 0;
      otherSepFound = Standard_False;
      aSep = separators[l];
      while(aSep != 0) {
        if (aSep == mystring[j]) {
          aSep = 0;
          otherSepFound = Standard_True;
        }
        else {
          aSep = separators[l++];
        }
      }
      if (otherSepFound) j++;
    }
    
    while (!isSepFound && k < mylength && j<mylength ) {
      l    = 0;
      aSep = separators[l];
      
      while (aSep != 0 && !isSepFound) {
        if (aSep == mystring[j]) {
          buftmp[k] = 0;
          isSepFound = Standard_True;
        }
        else {
          buftmp[k] = mystring[j];
        }
        l++;
        aSep = separators[l];
      }
      j++; k++;
      if(j==mylength) buftmp[k] = 0;
    }
  }
  
  if (i < whichone) 
  {
    res.mylength = 0;
    res.mystring[0] = 0;
  }
  else
  {  
    for ( res.mylength=0; buftmp[res.mylength]; ++res.mylength );
    res.mystring[res.mylength] = '\0';
  }
  return res;
}

// ----------------------------------------------------------------------------
// ToExtString
// ----------------------------------------------------------------------------
Standard_ExtString TCollection_ExtendedString::ToExtString() const
{
  return mystring;
}

// ----------------------------------------------------------------------------
// Trunc
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Trunc(const Standard_Integer ahowmany)
{
  if (ahowmany < 0 || ahowmany > mylength)
    throw Standard_OutOfRange("TCollection_ExtendedString::Trunc : "
                              "parameter 'ahowmany'");
  mylength = ahowmany;
  mystring[mylength] =  '\0';
}

// ----------------------------------------------------------------------------
// Value
// ----------------------------------------------------------------------------
Standard_ExtCharacter TCollection_ExtendedString::Value
                                        (const Standard_Integer where) const
{
 if (where > 0 && where <= mylength) {
   if(mystring) return mystring[where-1];
   else         return 0;
 }
 throw Standard_OutOfRange("TCollection_ExtendedString::Value : "
                           "parameter where");
}


//----------------------------------------------------------------------------
// Convert CString (including multibyte case) to UniCode representation
//----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::ConvertToUnicode (const Standard_CString theStringUtf)
{
  NCollection_Utf8Iter anIterRead (theStringUtf);
  Standard_ExtCharacter* anIterWrite = mystring;
  if (*anIterRead == 0)
  {
    *anIterWrite = '\0';
    return Standard_True;
  }

  for (; *anIterRead != 0; ++anIterRead)
  {
    if (!anIterRead.IsValid())
    {
      return Standard_False;
    }

    anIterWrite = anIterRead.GetUtf (anIterWrite);
  }
  return Standard_True;
}

//----------------------------------------------------------------------------
// Returns expected CString length in UTF8 coding.
//----------------------------------------------------------------------------
Standard_Integer TCollection_ExtendedString::LengthOfCString() const
{
  Standard_Integer aSizeBytes = 0;
  for (NCollection_Utf16Iter anIter (mystring); *anIter != 0; ++anIter)
  {
    aSizeBytes += anIter.AdvanceBytesUtf8();
  }
  return aSizeBytes;
}

//----------------------------------------------------------------------------
// Converts the internal <mystring> to UTF8 coding and returns length of the 
// out CString.
//----------------------------------------------------------------------------
Standard_Integer TCollection_ExtendedString::ToUTF8CString(Standard_PCharacter& theCString) const
{
  NCollection_Utf16Iter anIterRead (mystring);
  Standard_Utf8Char* anIterWrite = theCString;
  if (*anIterRead == 0)
  {
    *anIterWrite = '\0';
    return 0;
  }

  for (; *anIterRead != 0; ++anIterRead)
  {
    anIterWrite = anIterRead.GetUtf (anIterWrite);
  }
  *anIterWrite = '\0';
  return Standard_Integer(anIterWrite - theCString);
}

//=======================================================================
//function : allocate
//purpose  :
//=======================================================================
void TCollection_ExtendedString::allocate(const int theLength)
{
  mylength = theLength;
  if (theLength == 0)
  {
    mystring = THE_DEFAULT_EXT_CHAR_STRING;
  }
  else
  {
    const Standard_Size aRoundSize = (((theLength + 1) * sizeof(Standard_ExtCharacter)) + 3) & ~0x3;
    mystring = static_cast<Standard_PExtCharacter>(
      Standard::AllocateOptimal(aRoundSize));
    mystring[mylength] = '\0';
  }
}

//=======================================================================
//function : reallocate
//purpose  :
//=======================================================================
void TCollection_ExtendedString::reallocate(const int theLength)
{
  if (theLength != 0)
  {
    if (mystring == THE_DEFAULT_EXT_CHAR_STRING)
    {
      const Standard_Size aRoundSize = (((theLength + 1) * sizeof(Standard_ExtCharacter)) + 3) & ~0x3;
      mystring = static_cast<Standard_PExtCharacter>(Standard::AllocateOptimal(aRoundSize));
    }
    else
    {
      mystring = static_cast<Standard_PExtCharacter>(
        Standard::Reallocate(mystring, (theLength + 1) * sizeof(Standard_ExtCharacter)));
    }
    mystring[theLength] = 0;
  }
  if (mystring != THE_DEFAULT_EXT_CHAR_STRING)
  {
    mystring[theLength] = 0;
  }
  mylength = theLength;
}

//=======================================================================
//function : deallocate
//purpose  :
//=======================================================================
void TCollection_ExtendedString::deallocate()
{
  if (mystring != THE_DEFAULT_EXT_CHAR_STRING)
  {
    Standard::Free(mystring);
  }
  mylength = 0;
  mystring = THE_DEFAULT_EXT_CHAR_STRING;
}
