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

#include <cctype>
#include <cstdio>
#include <cstring>
// Shortcuts to standard allocate and reallocate functions
static inline Standard_PCharacter Allocate(const Standard_Size aLength)
{
  return (Standard_PCharacter)Standard::Allocate (aLength);
}
static inline Standard_PCharacter Reallocate (Standard_Address aAddr,
                                              const Standard_Size aLength)
{
  return (Standard_PCharacter)Standard::Reallocate (aAddr, aLength);
}
static inline void Free (Standard_PCharacter aAddr)
{
  Standard_Address aPtr = aAddr;
  Standard::Free (aPtr);
}

// ----------------------------------------------------------------------------
// Create an empty AsciiString
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString()
{
  mylength = 0;
  
  mystring = Allocate(mylength+1);
  mystring[mylength] = '\0';
}


// ----------------------------------------------------------------------------
// Create an asciistring from a Standard_CString
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_CString astring)
  : mystring(0), mylength(0)
{
  if (astring) {
    mylength = Standard_Integer( strlen(astring) );
    mystring = Allocate(mylength+1);
    strncpy(mystring,astring,mylength);
    mystring[mylength] = '\0';
  }
  else {
    throw Standard_NullObject("TCollection_AsciiString : parameter 'astring'");
  }
}

// ----------------------------------------------------------------------------
// Create an asciistring from a Standard_CString
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_CString astring,
                                                 const Standard_Integer aLen )
  : mystring(0), mylength(aLen)
{
  if (astring) {
    mystring = Allocate(mylength+1);
    strncpy( mystring , astring , mylength );
    mystring [ mylength ] = '\0' ;
  }
  else {
    throw Standard_NullObject("TCollection_AsciiString : parameter 'astring'");
  }
}

// ----------------------------------------------------------------------------
// Create an asciistring from a Standard_Character
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_Character aChar)
     : mystring(0)
{
  if ( aChar != '\0' ) {
    mylength    = 1;
    mystring    = Allocate(2);
    mystring[0] = aChar;
    mystring[1] = '\0';
  }
  else {
    mylength = 0;
    mystring = Allocate(mylength+1);
    mystring[mylength] = '\0';
  }
}

// ----------------------------------------------------------------------------
// Create an AsciiString from a filler
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_Integer length,
                                                 const Standard_Character filler )
{
  mystring = Allocate(length+1);
  mylength = length;
  for (int i = 0 ; i < length ; i++) mystring[i] = filler;
  mystring[length] = '\0';
}

// ----------------------------------------------------------------------------
// Create an AsciiString from an Integer
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_Integer aValue)
     : mystring(0)
{
  char t [13];
  mylength = Sprintf( t,"%d",aValue);
  mystring = Allocate(mylength+1);
  strncpy( mystring , t , mylength );
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// Create an asciistring from a real
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_Real aValue)
     : mystring(0)
{
  char t [50];
  mylength = Sprintf( t,"%g",aValue);
  mystring = Allocate(mylength+1);
  strncpy( mystring , t , mylength );
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// Create an asciistring from an asciistring
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const TCollection_AsciiString& astring)
     : mystring(0)
{
  mylength = astring.mylength;
  mystring = Allocate(mylength+1);
  if ( mylength )
    strncpy(mystring,astring.mystring,mylength);
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// Create an asciistring from a character
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(
                         const TCollection_AsciiString& astring ,
                         const Standard_Character other )
     : mystring(0)
{
  mylength = astring.mylength + 1 ;
  mystring = Allocate(mylength+1);
  if ( astring.mylength ) {
    strncpy( mystring , astring.mystring , astring.mylength ) ;
  }
  mystring[mylength-1] = other ;
  mystring[mylength] = '\0' ;
}

// ----------------------------------------------------------------------------
// Create an asciistring from an asciistring
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(
                         const TCollection_AsciiString& astring ,
                         const Standard_CString other )
     : mystring(0)
{
  Standard_Integer otherlength = Standard_Integer( other ? strlen( other ) : 0 );
  mylength = astring.mylength + otherlength ;
  mystring = Allocate(mylength+1);
  if ( astring.mylength ) {
    strncpy( mystring , astring.mystring , astring.mylength ) ;
  }
  if ( otherlength ) {
    strncpy( mystring + astring.mylength, other, otherlength );
  }
  mystring[ mylength ] = '\0';
}

// ----------------------------------------------------------------------------
// Create an asciistring from an asciistring
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(
                         const TCollection_AsciiString& astring ,
                         const TCollection_AsciiString& other )
     : mystring(0)
{
  mylength = astring.mylength + other.mylength ;
  mystring = Allocate(mylength+1);
  if ( astring.mylength ) {
    strncpy( mystring , astring.mystring , astring.mylength ) ;
  }
  if ( other.mylength ) {
    strncpy( mystring + astring.mylength, other.mystring , other.mylength ) ;
  }
  mystring[mylength] = '\0' ;
}

//---------------------------------------------------------------------------
//  Create an asciistring from an ExtendedString 
//---------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const TCollection_ExtendedString& astring,
                                                 const Standard_Character replaceNonAscii) 
: mystring (0)
{
  if (replaceNonAscii)
  {
    mylength = astring.Length(); 
    mystring = Allocate(mylength+1);
    for(int i = 0; i < mylength; i++) {
      Standard_ExtCharacter c = astring.Value(i+1);
      mystring[i] = ( IsAnAscii(c) ? ToCharacter(c) : replaceNonAscii );
    }
    mystring[mylength] = '\0';
  }
  else {
    // create UTF-8 string
    mylength = astring.LengthOfCString();
    mystring = Allocate(mylength+1);
    astring.ToUTF8CString(mystring);
  }
}

//---------------------------------------------------------------------------
//  Create an TCollection_AsciiString from a Standard_WideChar
//---------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString (const Standard_WideChar* theStringUtf)
: mystring (NULL),
  mylength (0)
{
  for (NCollection_UtfWideIter anIter (theStringUtf); *anIter != 0; ++anIter)
  {
    mylength += anIter.AdvanceBytesUtf8();
  }

  mystring = Allocate (mylength + 1);
  mystring[mylength] = '\0';
  NCollection_UtfWideIter anIterRead (theStringUtf);
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
  if (other != '\0') {
    mystring = Reallocate (mystring, mylength + 2);
    mystring[mylength] = other ;
    mylength += 1;
    mystring[mylength] = '\0';
  }
}

// ----------------------------------------------------------------------------
// AssignCat
// ----------------------------------------------------------------------------
void TCollection_AsciiString::AssignCat(const Standard_CString other)
{
  if (other) {
    Standard_Integer otherlength = Standard_Integer( strlen( other ));
    if ( otherlength ) {
      Standard_Integer newlength = mylength+otherlength;
      mystring = Reallocate (mystring, newlength + 1);
      strncpy( mystring + mylength, other, otherlength+1 );
      mylength = newlength;
    }
  }
  else {
    throw Standard_NullObject("TCollection_AsciiString::Operator += parameter other");
  }
}

// ----------------------------------------------------------------------------
// AssignCat
// ----------------------------------------------------------------------------
void TCollection_AsciiString::AssignCat(const TCollection_AsciiString& other)
{

  if (other.mylength) {
    Standard_Integer newlength = mylength+other.mylength;
    mystring = Reallocate (mystring, newlength + 1);
    strncpy( mystring + mylength, other.mystring, other.mylength+1 );
    mylength = newlength;
  }
}

// ---------------------------------------------------------------------------
// Capitalize
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Capitalize()
{
  if ( mylength ) mystring[0] = ::UpperCase(mystring[0]);
  for (int i = 1; i < mylength; i++ )
    mystring[i] = ::LowerCase(mystring[i]);
}

// ---------------------------------------------------------------------------
// Center
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Center(const Standard_Integer Width ,
                                     const Standard_Character Filler) 
{
  if(Width > mylength) {
    Standard_Integer newlength = mylength + ((Width - mylength)/2);
    LeftJustify(newlength,Filler);
    RightJustify(Width,Filler);
  }
  else if (Width < 0) {
    throw Standard_NegativeValue();
  }
}

// ----------------------------------------------------------------------------
// ChangeAll
// ----------------------------------------------------------------------------
void TCollection_AsciiString::ChangeAll(const Standard_Character aChar,
                                        const Standard_Character NewChar,
                                        const Standard_Boolean CaseSensitive)
{
  if (CaseSensitive){
    for (int i=0; i < mylength; i++)
      if (mystring[i] == aChar) mystring[i] = NewChar;
  }
  else{
    Standard_Character anUpperChar = ::UpperCase(aChar);
    for (int i=0; i < mylength; i++)
      if (::UpperCase(mystring[i]) == anUpperChar) mystring[i] = NewChar;
  }
}

// ----------------------------------------------------------------------------
// Clear
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Clear()
{
  if ( mylength > 0 )
  {
    Free (mystring);
    mylength = 0;
    mystring = Allocate(mylength+1);
    mystring[mylength] = '\0';
  }
}

// ----------------------------------------------------------------------------
// Copy
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Copy(const Standard_CString fromwhere)
{
  if (fromwhere) {
    mylength = Standard_Integer( strlen( fromwhere ));
    mystring = Reallocate (mystring, mylength + 1);
    strncpy( mystring, fromwhere, mylength+1 );
  }
  else {
    mylength = 0;
    mystring[mylength] = '\0';
  }
}

// ----------------------------------------------------------------------------
// Copy
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Copy(const TCollection_AsciiString& fromwhere)
{
  if (fromwhere.mystring) {
    mylength = fromwhere.mylength;
    mystring = Reallocate (mystring, mylength + 1);
    strncpy( mystring, fromwhere.mystring, mylength+1 );
  }
  else {
    mylength = 0;
    mystring[mylength] = '\0';
  }
}

// ----------------------------------------------------------------------------
// Destroy
// ----------------------------------------------------------------------------
TCollection_AsciiString::~TCollection_AsciiString()
{
  if (mystring) 
    Free (mystring);
  mystring = 0L;
}

// ----------------------------------------------------------------------------
// FirstLocationInSet
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::FirstLocationInSet
                                (const TCollection_AsciiString& Set,
                                 const Standard_Integer         FromIndex,
                                 const Standard_Integer         ToIndex) const
{
  if (mylength == 0 || Set.mylength == 0) return 0;
  if (FromIndex > 0 && ToIndex <= mylength && FromIndex <= ToIndex ) {
    for(int i = FromIndex-1 ; i < ToIndex; i++)
      for(int j = 0; j < Set.mylength; j++) 
        if (mystring[i] == Set.mystring[j]) return i+1;
    return 0;
  }
  throw Standard_OutOfRange();
}

// ----------------------------------------------------------------------------
// FirstLocationNotInSet
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::FirstLocationNotInSet
                                 (const TCollection_AsciiString& Set,
                                  const Standard_Integer         FromIndex,
                                  const Standard_Integer         ToIndex) const
{
  if (mylength == 0 || Set.mylength == 0) return 0;
  if (FromIndex > 0 && ToIndex <= mylength && FromIndex <= ToIndex ) {
    Standard_Boolean find;
    for (int i = FromIndex-1 ; i < ToIndex; i++) {
      find = Standard_False;
      for(int j = 0; j < Set.mylength; j++)  
        if (mystring[i] == Set.mystring[j]) find = Standard_True;
      if (!find)  return i+1;
    }
    return 0;
  }
  throw Standard_OutOfRange();
}

//----------------------------------------------------------------------------
// Insert a character before 'where'th character
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Insert(const Standard_Integer where,
                                     const Standard_Character what)
{
  if (where > mylength + 1 ) throw Standard_OutOfRange("TCollection_AsciiString::Insert : Parameter where is too big");
  if (where < 1)             throw Standard_OutOfRange("TCollection_AsciiString::Insert : Parameter where is too small");
  
  mystring = Reallocate (mystring, mylength + 2);
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
void TCollection_AsciiString::Insert(const Standard_Integer where,
                                     const Standard_CString what)
{
  if (where <= mylength + 1 && where > 0) {
    if(what) {
      Standard_Integer whatlength = Standard_Integer( strlen( what ) );
      Standard_Integer newlength = mylength + whatlength;
      
      mystring = Reallocate (mystring, newlength + 1);
      if (where != mylength +1) {
        for (int i=mylength-1; i >= where-1; i--)
          mystring[i+whatlength] = mystring[i];
      }
      for (int i=0; i < whatlength; i++)
        mystring[where-1+i] = what[i];
      
      mylength = newlength;
      mystring[mylength] = '\0';
    }
  }
  else {
    throw Standard_OutOfRange("TCollection_AsciiString::Insert : "
                              "Parameter where is invalid");
  }
}

// ----------------------------------------------------------------------------
// Insert
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Insert(const Standard_Integer where,
                                     const TCollection_AsciiString& what)
{
  Standard_CString swhat = what.mystring;
  if (where <= mylength + 1) {
    Standard_Integer whatlength = what.mylength;
    if(whatlength) {
      Standard_Integer newlength = mylength + whatlength;
      
      mystring = Reallocate (mystring, newlength + 1);

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
    throw Standard_OutOfRange("TCollection_AsciiString::Insert : "
                              "Parameter where is too big");
  }
}

//------------------------------------------------------------------------
//  InsertAfter
//------------------------------------------------------------------------
void TCollection_AsciiString::InsertAfter(const Standard_Integer Index,
                                          const TCollection_AsciiString& what)
{
   if (Index < 0 || Index > mylength) throw Standard_OutOfRange();
   Insert(Index+1,what);
}

//------------------------------------------------------------------------
//  InsertBefore
//------------------------------------------------------------------------
void TCollection_AsciiString::InsertBefore(const Standard_Integer Index,
                                           const TCollection_AsciiString& what)
{
   if (Index < 1 || Index > mylength) throw Standard_OutOfRange();
   Insert(Index,what);
}

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsEqual
                                        (const Standard_CString other)const
{
  if (other) {
    return ( strncmp( other, mystring, mylength+1 ) == 0 );
  }
  throw Standard_NullObject("TCollection_AsciiString::Operator == "
                             "Parameter 'other'");
}

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsEqual
                                (const TCollection_AsciiString& other)const
{
  if (mylength != other.mylength) return Standard_False;
  return ( strncmp( other.mystring, mystring, mylength ) == 0 );
}

// ----------------------------------------------------------------------------
// IsSameString
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsSameString (const TCollection_AsciiString& theString1,
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
    return (strncmp (theString1.ToCString(), theString2.ToCString(), aSize1) == 0);
  }

  for (Standard_Integer aCharIter = 1; aCharIter <= aSize1; ++aCharIter)
  {
    if (toupper (theString1.Value (aCharIter)) != toupper (theString2.Value (aCharIter)))
    {
      return Standard_False;
    }
  }
  return Standard_True;
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsDifferent
                                        (const Standard_CString other)const
{
  if (other) {
    return ( strncmp( other, mystring, mylength+1 ) != 0 );
  }
  throw Standard_NullObject("TCollection_AsciiString::Operator != "
                            "Parameter 'other'");
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsDifferent
                                (const TCollection_AsciiString& other)const
{

  if (mylength != other.mylength) return Standard_True;
  return ( strncmp( other.mystring, mystring, mylength ) != 0 );
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsLess
                                        (const Standard_CString other)const
{
  if (other) {
    return ( strncmp( mystring, other, mylength+1 ) < 0 );
  }
  throw Standard_NullObject("TCollection_AsciiString::Operator < "
                            "Parameter 'other'");
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsLess
                                (const TCollection_AsciiString& other)const
{
  return ( strncmp( mystring, other.mystring, mylength+1 ) < 0 );
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsGreater
                                        (const Standard_CString other)const
{
  if (other) {
    return ( strncmp( mystring, other, mylength+1 ) > 0 );
  }
  throw Standard_NullObject("TCollection_AsciiString::Operator > "
                            "Parameter 'other'");
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsGreater
                                (const TCollection_AsciiString& other)const
{
  return ( strncmp( mystring, other.mystring, mylength+1 ) > 0 );
}

// ----------------------------------------------------------------------------
// StartsWith
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::StartsWith (const TCollection_AsciiString& theStartString) const
{
  if (this == &theStartString)
  {
    return true;
  }

  return mylength >= theStartString.mylength
      && strncmp (theStartString.mystring, mystring, theStartString.mylength) == 0;
}

// ----------------------------------------------------------------------------
// EndsWith
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::EndsWith (const TCollection_AsciiString& theEndString) const
{
  if (this == &theEndString)
  {
    return true;
  }

  return mylength >= theEndString.mylength
      && strncmp (theEndString.mystring, mystring + mylength - theEndString.mylength, theEndString.mylength) == 0;
}

// ----------------------------------------------------------------------------
// IntegerValue
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::IntegerValue()const
{
  char *ptr;
  Standard_Integer value = (Standard_Integer)strtol(mystring,&ptr,10); 
  if (ptr != mystring) return value;

  throw Standard_NumericError("TCollection_AsciiString::IntegerValue");
}

// ----------------------------------------------------------------------------
// IsIntegerValue
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsIntegerValue()const
{
  char *ptr;
  strtol(mystring,&ptr,10);

  if (ptr != mystring) {
    for (int i=int(ptr-mystring); i < mylength; i++) {
      if (mystring[i] == '.') return Standard_False; // what about 'e','x',etc ???
    }
    return Standard_True;
  }
  return Standard_False;
}

// ----------------------------------------------------------------------------
// IsRealValue
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsRealValue()const
{
  char *ptr;
  Strtod(mystring,&ptr);
  return (ptr != mystring);
}

// ----------------------------------------------------------------------------
// IsAscii
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsAscii()const
{
// LD : Debuggee le 26/11/98
//      Cette fonction retournait TOUJOURS Standard_True !
  for (int i=0; i < mylength; i++)
    if (mystring[i] >= 127 || mystring[i] < ' ') return Standard_False;
  return Standard_True;
}

//------------------------------------------------------------------------
//  LeftAdjust
//------------------------------------------------------------------------
void TCollection_AsciiString::LeftAdjust ()
{
   Standard_Integer i ;
   for( i = 0 ; i < mylength ; i ++) if(!IsSpace(mystring[i])) break;
   if( i > 0 ) Remove(1,i);
}

//------------------------------------------------------------------------
//  LeftJustify
//------------------------------------------------------------------------
void TCollection_AsciiString::LeftJustify(const Standard_Integer Width,
                                          const Standard_Character Filler)
{
   if (Width > mylength) {
       mystring = Reallocate (mystring, Width + 1);
     for (int i = mylength; i < Width ; i++) mystring[i] = Filler;
     mylength = Width;
     mystring[mylength] = '\0';
   }
   else if (Width < 0) {
     throw Standard_NegativeValue();
   }
}

//------------------------------------------------------------------------
//  Location
//------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::Location
                                   (const Standard_Integer   N        ,
                                    const Standard_Character C        ,
                                    const Standard_Integer   FromIndex,
                                    const Standard_Integer   ToIndex  ) const
{
   if (FromIndex > 0 && ToIndex <= mylength && FromIndex <= ToIndex ) {
     for(int i = FromIndex-1, count = 0; i <= ToIndex-1; i++) {
       if(mystring[i] == C) {
         count++;
         if ( count == N ) return i+1;
       }
     }
     return 0 ;
   }
   throw Standard_OutOfRange();
}

//------------------------------------------------------------------------
//  Location
//------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::Location
                                (const TCollection_AsciiString& what,
                                 const Standard_Integer         FromIndex,
                                 const Standard_Integer         ToIndex) const
{
  if (mylength == 0 || what.mylength == 0) return 0;
  if (ToIndex <= mylength && FromIndex > 0 && FromIndex <= ToIndex ) {
    Standard_Integer i = FromIndex-1;
    Standard_Integer k = 1;
    Standard_Integer l = FromIndex-2;
    Standard_Boolean Find = Standard_False; 
    while (!Find && i < ToIndex)  {
      if (mystring[i] == what.Value(k)) {
        k++;
        if ( k > what.mylength) Find = Standard_True;
      }
      else {
        if (k > 1) i--;    // si on est en cours de recherche 
        k = 1;
        l = i;
      }
      i++;
    }
    if (Find) return l+2;
    else      return 0;
  }
  throw Standard_OutOfRange();
}

// ----------------------------------------------------------------------------
// LowerCase
// ----------------------------------------------------------------------------
void TCollection_AsciiString::LowerCase()
{
  for (int i=0; i < mylength; i++)
    mystring[i] = ::LowerCase(mystring[i]);
}

//------------------------------------------------------------------------
//  Prepend
//------------------------------------------------------------------------
void TCollection_AsciiString::Prepend(const TCollection_AsciiString& what)
{
  Insert(1,what);
}

// ----------------------------------------------------------------------------
// RealValue
// ----------------------------------------------------------------------------
Standard_Real TCollection_AsciiString::RealValue()const
{
  char *ptr;
  Standard_Real value = Strtod(mystring,&ptr);
  if (ptr != mystring) return value;

  throw Standard_NumericError("TCollection_AsciiString::RealValue");
}

// ----------------------------------------------------------------------------
// Read
//--------------------------------------------------------------------------
void TCollection_AsciiString::Read(Standard_IStream& astream)
{
  // get characters from astream
  const Standard_Integer bufSize = 8190;
  Standard_Character buffer[bufSize];
  std::streamsize oldWidth = astream.width (bufSize);
  astream >> buffer;
  astream.width( oldWidth );

  // put to string
  mylength = Standard_Integer( strlen( buffer ));
  mystring = Reallocate (mystring, mylength + 1);
  strncpy(mystring,buffer,mylength);
  mystring[mylength] = '\0';
}


//---------------------------------------------------------------------------
Standard_IStream& operator >> (Standard_IStream& astream,
                               TCollection_AsciiString& astring)
{
  astring.Read(astream);
  return astream;
}


// ----------------------------------------------------------------------------
// Print
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Print(Standard_OStream& astream)const
{
  if(mystring) astream << mystring;
}


// ----------------------------------------------------------------------------
Standard_OStream& operator << (Standard_OStream& astream,
                               const TCollection_AsciiString& astring)
{
  astring.Print(astream);
  return astream;
}

// ----------------------------------------------------------------------------
// RemoveAll
// ----------------------------------------------------------------------------
void TCollection_AsciiString::RemoveAll(const Standard_Character what,
                                        const Standard_Boolean CaseSensitive)
{   
  if (mylength == 0) return;
  int c = 0;
  if (CaseSensitive) {
    for (int i=0; i < mylength; i++)
      if (mystring[i] != what) mystring[c++] = mystring[i];
  }
  else {
    Standard_Character upperwhat = ::UpperCase(what);
    for (int i=0; i < mylength; i++) { 
      if (::UpperCase(mystring[i]) != upperwhat) mystring[c++] = mystring[i];
    }
  }
  mylength = c;
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// RemoveAll
// ----------------------------------------------------------------------------
void TCollection_AsciiString::RemoveAll(const Standard_Character what)
{
  if (mylength == 0) return;
  int c = 0;
  for (int i=0; i < mylength; i++)
    if (mystring[i] != what) mystring[c++] = mystring[i];
  mylength = c;
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// Remove
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Remove (const Standard_Integer where,
                                      const Standard_Integer ahowmany)
{
 if (where+ahowmany <= mylength+1) {
   int i,j;
   for(i = where+ahowmany-1, j = where-1; i < mylength; i++, j++)
     mystring[j] = mystring[i];
   mylength -= ahowmany;
   mystring[mylength] = '\0';
 }
 else {
  throw Standard_OutOfRange("TCollection_AsciiString::Remove: "
                            "Too many characters to erase or invalid "
                            "starting value.");
 }
}

//------------------------------------------------------------------------
//  RightAdjust
//------------------------------------------------------------------------
void TCollection_AsciiString::RightAdjust ()
{
  Standard_Integer i ;
  for ( i = mylength-1 ; i >= 0 ; i--)
    if(!IsSpace(mystring[i]))
      break;
  if( i < mylength-1 )
    Remove(i+2,mylength-(i+2)+1);
}

//------------------------------------------------------------------------
//  RightJustify
//------------------------------------------------------------------------
void TCollection_AsciiString::RightJustify(const Standard_Integer Width,
                                           const Standard_Character Filler)
{
  Standard_Integer i ;
  Standard_Integer k ;
  if (Width > mylength) {
    mystring = Reallocate (mystring, Width + 1);

    for ( i = mylength-1, k = Width-1 ; i >= 0 ; i--, k--) 
      mystring[k] = mystring[i];
    for(; k >= 0 ; k--) mystring[k] = Filler;
    mylength = Width;
    mystring[mylength] = '\0';
  }
  else if (Width < 0) {
    throw Standard_NegativeValue();
  }
}

// ----------------------------------------------------------------------------
// Search
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::Search
                                        (const Standard_CString what)const
{
  Standard_Integer size = Standard_Integer( what ? strlen( what ) : 0 );
  if (size) {
    int k,j;
    int i = 0;
    while ( i < mylength-size+1 ) {
      k = i++;
      j = 0;
      while (j < size && mystring[k++] == what[j++])
        if (j == size) return i;
    }
  }
  return -1;
}


// ----------------------------------------------------------------------------
// Search
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::Search
                                (const TCollection_AsciiString& what) const
{
  Standard_Integer size = what.mylength;
  Standard_CString swhat = what.mystring;  
  if (size) {
    int k,j;
    int i = 0;
    while ( i < mylength-size+1 ) {
      k = i++;
      j = 0;
      while (j < size && mystring[k++] == swhat[j++])
        if (j == size) return i;
    }
  }
  return -1;
}


// ----------------------------------------------------------------------------
// SearchFromEnd
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::SearchFromEnd
                                        (const Standard_CString what)const
{
  Standard_Integer size = Standard_Integer( what ? strlen( what ) : 0 );
  if (size) {
    int k,j;
    int i = mylength-1;
    while ( i >= size-1 ) {
      k = i--;
      j = size-1;
      while (j >= 0 && mystring[k--] == what[j--])
        if (j == -1) return i-size+3;
    }
  }
  return -1;
}


// ----------------------------------------------------------------------------
// SearchFromEnd
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::SearchFromEnd
                                (const TCollection_AsciiString& what)const
{
  int size = what.mylength;
  if (size) {
    Standard_CString swhat = what.mystring;  
    int k,j;
    int i = mylength-1;
    while ( i >= size-1 ) {
      k = i--;
      j = size-1;
      while (j >= 0 && mystring[k--] == swhat[j--])
        if (j == -1) return i-size+3;
    }
  }
  return -1;
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_AsciiString::SetValue(const Standard_Integer where,
                                       const Standard_Character what)
{
  if (where > 0 && where <= mylength) {
    mystring[where-1] = what;
  }
  else {
    throw Standard_OutOfRange("TCollection_AsciiString::SetValue : "
                              "parameter where");
  }
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_AsciiString::SetValue(const Standard_Integer where,
                                       const Standard_CString what)
{
 if (where > 0 && where <= mylength+1) {
   Standard_Integer size = Standard_Integer( what ? strlen( what ) : 0 );
   size += (where - 1);  
   if (size >= mylength) {
     mystring = Reallocate (mystring, size + 1);
     mylength = size;
   } 
   for (int i = where-1; i < size; i++)
     mystring[i] = what[i-(where-1)];
   mystring[mylength] = '\0';
 }
 else {
   throw Standard_OutOfRange("TCollection_AsciiString::SetValue : "
                             "parameter where");
 }
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_AsciiString::SetValue(const Standard_Integer where,
                                       const TCollection_AsciiString& what)
{
 if (where > 0 && where <= mylength+1) {
   Standard_Integer size = what.mylength;
   Standard_CString swhat = what.mystring;  
   size += (where - 1);  
   if (size >= mylength) {
     mystring = Reallocate (mystring, size + 1);
     mylength = size;
   } 
   for (int i = where-1; i < size; i++)
     mystring[i] = swhat[i-(where-1)];
   mystring[mylength] = '\0';
 }
 else {
   throw Standard_OutOfRange("TCollection_AsciiString::SetValue : "
                             "parameter where");
 }
}

// ----------------------------------------------------------------------------
// Split
// Private
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Split(const Standard_Integer where,
                                    TCollection_AsciiString& res)
{
  if (where >= 0 && where <= mylength) {
    res = &mystring[where] ;
    Trunc(where);
    return ;
  }
  throw Standard_OutOfRange("TCollection_AsciiString::Split index");
  return ;
}

// ----------------------------------------------------------------------------
// Split
// ----------------------------------------------------------------------------
TCollection_AsciiString TCollection_AsciiString::Split
                                                (const Standard_Integer where)
{
  if (where >= 0 && where <= mylength) {
    TCollection_AsciiString res( &mystring[where] , mylength - where );
    Trunc(where);
    return res;
  }
  throw Standard_OutOfRange("TCollection_AsciiString::Split index");
}

// ----------------------------------------------------------------------------
// SubString
// Private
// ----------------------------------------------------------------------------
void TCollection_AsciiString::SubString(const Standard_Integer FromIndex,
                                        const Standard_Integer ToIndex,
                                        TCollection_AsciiString& res) const
{

  if (ToIndex > mylength || FromIndex <= 0 || FromIndex > ToIndex )
    throw Standard_OutOfRange();
  Standard_Integer newlength = ToIndex-FromIndex+1;
  res.mystring =Reallocate (res.mystring, newlength + 1);
  strncpy( res.mystring, mystring + FromIndex - 1, newlength );
  res.mystring[newlength] = '\0';
  res.mylength = newlength;
  return ;
}

// ----------------------------------------------------------------------------
// Token
// Private
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Token(const Standard_CString separators,
                                    const Standard_Integer whichone,
                                    TCollection_AsciiString& res)const
{
  res = Token( separators , whichone ) ;
}

// ----------------------------------------------------------------------------
// Token
// ----------------------------------------------------------------------------
TCollection_AsciiString TCollection_AsciiString::Token
                                        (const Standard_CString separators,
                                         const Standard_Integer whichone) const
{
  if (!separators)
    throw Standard_NullObject("TCollection_AsciiString::Token : "
                              "parameter 'separators'");

  Standard_Integer theOne ;
  Standard_Integer StringIndex = 0 ;
  Standard_Integer SeparatorIndex ;
  Standard_Integer BeginIndex=0 ;
  Standard_Integer EndIndex=0 ;

//  cout << "'" << mystring <<  "'" << endl ;
  for ( theOne = 0 ; theOne < whichone ; theOne++ ) {
     BeginIndex = 0 ;
     EndIndex = 0 ;
//     cout << "theOne " << theOne << endl ;
     if ( StringIndex == mylength )
       break ;
     for (; StringIndex < mylength && EndIndex == 0 ; StringIndex++ ) {
        SeparatorIndex = 0 ;
//        cout << "StringIndex " << StringIndex << endl ;
        while ( separators [ SeparatorIndex ] ) {
             if ( mystring [ StringIndex ] == separators [ SeparatorIndex ] ) {
               break ;
             }
             SeparatorIndex += 1 ;
           }
        if ( separators [ SeparatorIndex ] != '\0' ) { // We have a Separator
          if ( BeginIndex && EndIndex == 0 ) {
            EndIndex = StringIndex ;
//            cout << "EndIndex " << EndIndex << " '" << SubString( BeginIndex , EndIndex ).ToCString() << "'" << endl ;
            break ;
          }
        }
        else if ( BeginIndex == 0 ) {               // We have not a Separator
          BeginIndex = StringIndex + 1 ;
//          cout << "BeginIndex " << BeginIndex << endl ;
        }
     }
//     cout << "BeginIndex " << BeginIndex << " EndIndex " << EndIndex << endl ;
  }
  if ( BeginIndex == 0 )
    return TCollection_AsciiString("",0) ;
  if ( EndIndex == 0 )
    EndIndex = mylength ;
//    cout << "'" << SubString( BeginIndex , EndIndex ).ToCString() << "'" << endl ;
  return TCollection_AsciiString( &mystring [ BeginIndex - 1 ] ,
                                  EndIndex - BeginIndex + 1 ) ;
}

// ----------------------------------------------------------------------------
// Trunc
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Trunc(const Standard_Integer ahowmany)
{
  if (ahowmany < 0 || ahowmany > mylength)
    throw Standard_OutOfRange("TCollection_AsciiString::Trunc : "
                              "parameter 'ahowmany'");
  mylength = ahowmany;
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// UpperCase
// ----------------------------------------------------------------------------
void TCollection_AsciiString::UpperCase()
{
  for (int i=0; i < mylength; i++)
    mystring[i] = ::UpperCase(mystring[i]);
}

//------------------------------------------------------------------------
//  UsefullLength
//------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::UsefullLength () const
{
  Standard_Integer i ;
  for ( i = mylength -1 ; i >= 0 ; i--) 
    if (IsGraphic(mystring[i])) break;
  return i+1;
}

// ----------------------------------------------------------------------------
// Value
// ----------------------------------------------------------------------------
Standard_Character TCollection_AsciiString::Value
                                        (const Standard_Integer where)const
{
 if (where > 0 && where <= mylength) {
   return mystring[where-1];
 }
 throw Standard_OutOfRange("TCollection_AsciiString::Value : parameter where");
}
