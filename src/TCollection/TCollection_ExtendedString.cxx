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
#include <Standard_ExtString.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_AsciiString.hxx>

#include <algorithm>
#include <cctype>
#include <cstdio>

namespace
{
  //! Allocate string buffer (automatically adding extra symbol for NULL-termination).
  static Standard_ExtCharacter* allocateExtChars (const Standard_Size theLength)
  {
    return (Standard_ExtCharacter* )Standard::Allocate ((theLength + 1) * sizeof(Standard_ExtCharacter));
  }

  //! Re-allocate string buffer (automatically adding extra symbol for NULL-termination).
  static Standard_ExtCharacter* reallocateExtChars (Standard_Address theAddr,
                                                    const Standard_Size theLength)
  {
    return (Standard_ExtCharacter* )Standard::Reallocate (theAddr, (theLength + 1) * sizeof(Standard_ExtCharacter));
  }

  static const Standard_ExtCharacter NULL_EXTSTRING[1] = {0};

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
    Standard_ExtCharacter* aString = allocateExtChars (theLength);
    NCollection_UtfWideIter anIterRead (theUtfString);
    for (Standard_ExtCharacter* anIterWrite = aString; *anIterRead != 0; ++anIterRead)
    {
      anIterWrite = anIterRead.GetUtf (anIterWrite);
    }
    aString[theLength] = '\0';
    return aString;
  }

  //! Use memcpy() conversion when size is the same (e.g. on Windows).
  template<>
  inline Standard_ExtCharacter* Standard_UNUSED fromWideString<sizeof(Standard_ExtCharacter)> (const Standard_WideChar* theUtfString,
                                                                                               Standard_Integer& theLength)
  {
    for (theLength = 0; theUtfString[theLength] != L'\0'; ++theLength) {}
    Standard_ExtCharacter* aString = allocateExtChars (theLength);
    const Standard_Integer aSize = theLength * sizeof(Standard_ExtCharacter);
    memcpy (aString, theUtfString, aSize);
    aString[theLength] = '\0';
    return aString;
  }

}

//-----------------------------------------------------------------------------
// Create an empty ExtendedString
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString()
: mystring (allocateExtChars (0)),
  mylength (0)
{
  mystring[0] = '\0';
}

//----------------------------------------------------------------------------
// Create an ExtendedString from a Standard_CString
//----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                          (const Standard_CString theString,
                                           const Standard_Boolean isMultiByte) 
: mystring (NULL),
  mylength (0)
{
  if (theString == NULL)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  if (isMultiByte)
  {
    mylength = nbSymbols (theString);
    mystring = allocateExtChars (mylength);
    mystring[mylength] = '\0';
    if (ConvertToUnicode (theString))
    {
      return;
    }
  }

  mylength = (int)strlen(theString);
  mystring = reallocateExtChars (mystring, mylength);
  for (int aCharIter = 0; aCharIter < mylength; ++aCharIter)
  {
    mystring[aCharIter] = ToExtCharacter (theString[aCharIter]);
  }
  mystring[mylength] = '\0';
}

//---------------------------------------------------------------------------
// Create an ExtendedString from an ExtString
//--------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString (const Standard_ExtString theString)
: mystring (NULL),
  mylength (0)
{
  if (theString == NULL)
  {
    throw Standard_NullObject("TCollection_ExtendedString : null parameter ");
  }

  for (mylength = 0; theString[mylength] != '\0'; ++mylength) {}
  mystring = allocateExtChars (mylength);
  const Standard_Integer aSizeBytes = mylength * sizeof(Standard_ExtCharacter);
  memcpy (mystring, theString, aSizeBytes);
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// TCollection_ExtendedString
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString (const Standard_WideChar* theStringUtf)
: mystring (NULL),
  mylength (0)
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
  if ( aChar != '\0' ) {
    mylength    = 1;
    mystring    = allocateExtChars (1);
    mystring[0] = ToExtCharacter(aChar);
    mystring[1] = '\0';
  }
  else {
    //    mystring = 0L;
    mylength = 0;
    mystring = allocateExtChars (0);
    mystring[0] = '\0';
  }
}

//--------------------------------------------------------------------------
// Create a string from a ExtCharacter
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_ExtCharacter aChar)
{
  mylength    = 1;
  mystring    = allocateExtChars (1);
  mystring[0] = aChar;
  mystring[1] = '\0';
}

// ----------------------------------------------------------------------------
// Create an AsciiString from a filler
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_Integer      length,
                                         const Standard_ExtCharacter filler )
{
  mystring = allocateExtChars (length);
  mylength = length;
  for (int i = 0 ; i < length ; i++) mystring[i] = filler;
  mystring[mylength] = '\0';
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
  mylength = (int)strlen(CHN.t);
  mystring = allocateExtChars (mylength);
  for (int i = 0 ; i < mylength ; i++) mystring[i] = ToExtCharacter(CHN.t[i]);
  mystring[mylength] = '\0';
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
  mylength = (int)strlen( CHN.t );
  mystring = allocateExtChars (mylength);
  for (int i = 0 ; i < mylength ; i++) mystring[i] = ToExtCharacter(CHN.t[i]);
  mystring[mylength] = '\0';
}

//-----------------------------------------------------------------------------
// create an extendedstring from an extendedstring
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                (const TCollection_ExtendedString& astring)
{
  const Standard_Integer aSizeBytes = astring.mylength * sizeof(Standard_ExtCharacter);
  mylength = astring.mylength;
  mystring = allocateExtChars (astring.mylength);
  memcpy (mystring, astring.mystring, aSizeBytes);
  mystring[mylength] = '\0';
}

//---------------------------------------------------------------------------
//  Create an extendedstring from an AsciiString 
//---------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                (const TCollection_AsciiString& theString)
{
  mylength = nbSymbols (theString.ToCString());
  mystring = allocateExtChars (mylength);
  mystring[mylength] = '\0';
  if (ConvertToUnicode (theString.ToCString()))
  {
    return;
  }

  mylength = theString.Length();
  mystring = reallocateExtChars (mystring, mylength);
  Standard_CString aCString = theString.ToCString();
  for (Standard_Integer aCharIter = 0; aCharIter <= mylength; ++aCharIter)
  {
    mystring[aCharIter] = ToExtCharacter (aCString[aCharIter]);
  }
  mystring[mylength] = '\0';
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
  const Standard_Integer aNewlength    = mylength + anOtherLength;
  if (mystring != NULL)
  {
    mystring = reallocateExtChars (mystring, aNewlength);
    memcpy (mystring + mylength, theOther.mystring, anOtherLength * sizeof(Standard_ExtCharacter));
  }
  else
  {
    mystring = allocateExtChars (aNewlength);
    memcpy (mystring, theOther.mystring, anOtherLength * sizeof(Standard_ExtCharacter));
  }
  mylength = aNewlength;
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// Cat
// ----------------------------------------------------------------------------
TCollection_ExtendedString TCollection_ExtendedString::Cat
                                (const TCollection_ExtendedString& other) const 
{
  TCollection_ExtendedString res( mylength + other.mylength, 0 );
  if ( mylength > 0 )
    memcpy( res.mystring, mystring, mylength*2 );
  if ( other.mylength > 0 )
    memcpy( res.mystring + mylength, other.mystring, other.mylength*2 );
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
  if (mylength == 0)
  {
    return;
  }

  Standard::Free (mystring);
  mylength = 0;
  mystring = allocateExtChars (mylength);
  mystring[0] = '\0';
}

// ----------------------------------------------------------------------------
// Copy
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Copy (const TCollection_ExtendedString& fromwhere)
{

  if (fromwhere.mystring) {
    const Standard_Integer newlength  = fromwhere.mylength;
    const Standard_Integer aSizeBytes = newlength * sizeof(Standard_ExtCharacter);
    if (mystring != NULL)
    {
      mystring = reallocateExtChars (mystring, newlength);
    }
    else {
      mystring = allocateExtChars (newlength);
    }
    mylength = newlength;
    memcpy (mystring, fromwhere.mystring, aSizeBytes);
    mystring[mylength] = '\0';
  }
  else
  {
    if (mystring != 0)
    {
      mylength = 0;
      mystring[0] = '\0';
    }
  }
}

// ----------------------------------------------------------------------------
// Swap
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Swap (TCollection_ExtendedString& theOther)
{
  std::swap (mystring, theOther.mystring);
  std::swap (mylength, theOther.mylength);
}

// ----------------------------------------------------------------------------
// Destroy
// ----------------------------------------------------------------------------
TCollection_ExtendedString::~TCollection_ExtendedString()
{
  if (mystring) Standard::Free(mystring);
  mystring = 0L;
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

  if (mystring != NULL)
  {
    mystring = reallocateExtChars (mystring, mylength + 1);
  }
  else {
    mystring = allocateExtChars (mylength + 1);
  }
  if (where != mylength +1) {
    for (int i=mylength-1; i >= where-1; i--)
      mystring[i+1] = mystring[i];
  }
  mystring[where-1] = what;
  mylength++;
  mystring[mylength] = '\0';
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
      Standard_Integer newlength = mylength + whatlength;
      
      if (mystring) {
          mystring = reallocateExtChars (mystring, newlength);
      }
      else {
        mystring = allocateExtChars (newlength);
      }
      if (where != mylength +1) {
        for (int i=mylength-1; i >= where-1; i--)
          mystring[i+whatlength] = mystring[i];
      }
      for (int i=0; i < whatlength; i++)
        mystring[where-1+i] = swhat[i];
      
      mylength = newlength;
      mystring[mylength] = '\0';
    }
  }
  else {
    throw Standard_OutOfRange("TCollection_ExtendedString::Insert : "
                              "Parameter where is too big");
  }
}

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsEqual
                                (const Standard_ExtString other) const
{
  return ( memcmp( mystring, other, (mylength+1)*2 ) == 0 );
}

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsEqual
                                (const TCollection_ExtendedString& other) const
{
  return ( memcmp( mystring, other.mystring, (mylength+1)*2 ) == 0 );
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsDifferent
                                (const Standard_ExtString other ) const
{
  return ( memcmp( mystring, other, (mylength+1)*2 ) != 0 );
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsDifferent
                                (const TCollection_ExtendedString& other) const
{
  return ( memcmp( mystring, other.mystring, (mylength+1)*2 ) != 0 );
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsLess
                                (const Standard_ExtString other) const
{
  return ( memcmp( mystring, other, (mylength+1)*2 ) < 0 );
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsLess
                                (const TCollection_ExtendedString& other) const
{
  return ( memcmp( mystring, other.mystring, (mylength+1)*2 ) < 0 );
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsGreater
                                (const Standard_ExtString other) const
{
  return ( memcmp( mystring, other, (mylength+1)*2 ) > 0 );
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsGreater
                                (const TCollection_ExtendedString& other) const
{
  return ( memcmp( mystring, other.mystring, (mylength+1)*2 ) > 0 );
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
      && memcmp (theStartString.mystring, mystring, theStartString.mylength) == 0;
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
      && memcmp (theEndString.mystring, mystring + mylength - theEndString.mylength, theEndString.mylength) == 0;
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
void TCollection_ExtendedString::Print(Standard_OStream& astream) const 
{ 
  // ASCII symbols (including extended Ascii) are printed as is;
  // other Unicode characters are encoded as SGML numeric character references
  for (Standard_Integer i = 0 ; i < mylength ; i++) {
    Standard_ExtCharacter c = mystring[i];
    if ( IsAnAscii(c) )
      astream << ToCharacter(c);
    else 
      astream << "&#" << c << ";";
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
      if (mystring) {
        mystring = reallocateExtChars (mystring, size);
      }
      else {
        mystring = allocateExtChars (size);
      }
      mylength = size;
    } 
    for (int i = where-1; i < size; i++)
      mystring[i] = swhat[i-(where-1)];
    mystring[mylength] =  '\0';
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
  TCollection_ExtendedString res;
  if (!separators)
    throw Standard_NullObject("TCollection_ExtendedString::Token : "
                              "parameter 'separators'");
  
  int                   i,j,k,l;
  Standard_PExtCharacter  buftmp = allocateExtChars (mylength);
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
  
  if (i < whichone) {
    buftmp[0] = 0;
    Standard::Free(buftmp);
  }
  else {
    Standard::Free(res.mystring);
    res.mystring = buftmp;
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
  if(mystring) return mystring;
  return NULL_EXTSTRING;
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
