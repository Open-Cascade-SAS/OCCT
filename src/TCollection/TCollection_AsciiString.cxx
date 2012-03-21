// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#define OptJr 1
// OCC6794: if OptJr is ON, we need to ensure that memory allocations are done by 4 bytes words,
// in order to avoid writing into unallocated memory at the string end when it is copied
// by CSTRINGCOPY or ASCIISTRINGCOPY macros
// (though, due to OCC memory manager roundings of allocated memory, the problem appears
//  only when MMGT_OPT is 0 and string size is greater than MMGT_THRESHOLD)
#ifdef OptJr
#define ROUNDMEM(len) (((len)+3)&~0x3)
#else
#define ROUNDMEM(len) (len)
#endif

//#if defined(WNT) || defined(LIN)
#include <stdio.h>
//#endif

#include <TCollection_AsciiString.ixx>
#include <Standard.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_ctype.hxx>
#include <Standard_String.hxx>

#include <TCollection_ExtendedString.hxx>

// ###### PLACER LE TYPE NON DEFINI strtol (portabilite) ######
#ifdef WNT
# include <string.h>
#else
extern "C" {
     long strtol(const char*, char**, int);
}
#endif  // WNT

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
     : mystring(0)
{
  if (astring) {
#if OptJr
    STRINGLEN( astring , mylength ) ;
#else
    STRLEN(astring,mylength);
#endif
    mystring = Allocate(ROUNDMEM(mylength+1));
#if OptJr
    CSTRINGCOPY( mystring , astring , mylength ) ;
#else
    STRCPY(mystring,astring,mylength);
    mystring[mylength] = '\0';
#endif
  }
  else {
    Standard_NullObject::Raise("TCollection_AsciiString : parameter 'astring'");
  }
}

// ----------------------------------------------------------------------------
// Create an asciistring from a Standard_CString
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_CString astring,
                                                 const Standard_Integer aLen )
     : mystring(0)
{
  if (astring) {
    mylength = aLen ;
    mystring = Allocate(ROUNDMEM(mylength+1));
    CSTRINGCOPY( mystring , astring , mylength ) ;
    mystring [ mylength ] = '\0' ;
  }
  else {
    Standard_NullObject::Raise("TCollection_AsciiString : parameter 'astring'");
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

  union { int bid ;      // ?? to ensure alignment of t[] by double-word??
          char t [13]; } CHN ; 
  sprintf(&CHN.t[0],"%d",aValue);
#if OptJr
  STRINGLEN( CHN.t , mylength ) ;
#else
  STRLEN(CHN.t,mylength);
#endif
  mystring = Allocate(ROUNDMEM(mylength+1));
#if OptJr
  ASCIISTRINGCOPY( mystring , CHN.t , mylength ) ;
#else
  STRCPY(mystring,CHN.t,mylength);
  mystring[mylength] = '\0';
#endif
}

// ----------------------------------------------------------------------------
// Create an asciistring from a real
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const Standard_Real aValue)
     : mystring(0)
{

  union { int bid ;
          char t [50]; } CHN ;
  sprintf(&CHN.t[0],"%g",aValue);
#if OptJr
  STRINGLEN( CHN.t , mylength ) ;
#else
  STRLEN(CHN.t,mylength);
#endif
  mystring = Allocate(ROUNDMEM(mylength+1));
#if OptJr
  ASCIISTRINGCOPY( mystring , CHN.t , mylength ) ;
#else
  STRCPY(mystring,CHN.t,mylength);
  mystring[mylength] = '\0';
#endif
}

// ----------------------------------------------------------------------------
// Create an asciistring from an asciistring
// ----------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const TCollection_AsciiString& astring)
     : mystring(0)
{

  mylength = astring.mylength;
  mystring = Allocate(ROUNDMEM(mylength+1));
  if ( astring.mystring ) {
#if OptJr
    ASCIISTRINGCOPY( mystring , astring.mystring , mylength ) ;
  }
  else
#else
    STRCPY(mystring,astring.mystring,mylength);
  }
#endif
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
  mystring = Allocate(ROUNDMEM(mylength+1));  
  if ( astring.mystring ) {
    ASCIISTRINGCOPY( mystring , astring.mystring , astring.mylength ) ;
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
 Standard_Integer otherlength;

  STRINGLEN( other , otherlength );
  mylength = astring.mylength + otherlength ;
  mystring = Allocate(ROUNDMEM(mylength+1));
  if ( astring.mystring ) {
    ASCIISTRINGCOPY( mystring , astring.mystring , astring.mylength ) ;
  }
  STRINGCAT( mystring , astring.mylength , other , otherlength ) ;
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
  mystring = Allocate(ROUNDMEM(mylength+1));
  if ( astring.mystring ) {
    ASCIISTRINGCOPY( mystring , astring.mystring , astring.mylength ) ;
  }
  if ( other.mystring ) {
    STRINGCAT( mystring , astring.mylength , other.mystring , other.mylength ) ;
  }
  if ( mylength == 0 )
    mystring[0] = '\0' ;

}

//---------------------------------------------------------------------------
//  Create an asciistring from an ExtendedString 
//---------------------------------------------------------------------------
TCollection_AsciiString::TCollection_AsciiString(const TCollection_ExtendedString& astring,
                                                 const Standard_Character replaceNonAscii) 
     : mystring(0)
{
  if (replaceNonAscii || astring.IsAscii()) {
    mylength = astring.Length(); 
    mystring = Allocate(mylength+1);
    for(int i = 0; i < mylength; i++) {
      Standard_ExtCharacter c = astring.Value(i+1);
      mystring[i] = ( IsAnAscii(c) ? ToCharacter(c) : replaceNonAscii );
    }
    mystring[mylength] = '\0';
  }
  else {
    Standard_SStream amsg;
    amsg << "It's not an ascii string : " ;
    astring.Print(amsg);
    Standard_OutOfRange::Raise(amsg);
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
    if (mystring) {
      mystring = Reallocate((void*&)mystring,
                                                        mylength+2);
    }
    else {
      mystring = Allocate(mylength+2);
    }
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
    if (other[0] != '\0') {
      Standard_Integer otherlength;
#if OptJr
      STRINGLEN( other , otherlength ) ;
#else
      STRLEN(other,otherlength);
#endif
      Standard_Integer newlength = mylength+otherlength;
      if (mystring) {
          mystring = Reallocate((void*&)mystring,
							       ROUNDMEM(newlength+1));
#if OptJr
        STRINGCAT( mystring , mylength , other , otherlength ) ;
#else
        STRCAT(mystring,mylength,other,otherlength);
#endif
      }
      else {
        mystring = Allocate(ROUNDMEM(newlength+1));
#if OptJr
        CSTRINGCOPY( mystring , other , newlength ) ;
#else
        STRCPY(mystring,other,newlength);
#endif
      }
      mylength = newlength;
#if !OptJr
      mystring[mylength] = '\0';
#endif
    }
  }
  else {
    Standard_NullObject::Raise("TCollection_AsciiString::Operator += parameter other");
  }
}

// ----------------------------------------------------------------------------
// AssignCat
// ----------------------------------------------------------------------------
void TCollection_AsciiString::AssignCat(const TCollection_AsciiString& other)
{

  Standard_Integer otherlength = other.mylength; 
  if (otherlength) {
    Standard_CString sother = other.mystring;
    Standard_Integer newlength = mylength+otherlength;
    if (mystring) {
        mystring = Reallocate((void*&)mystring,
                                                           ROUNDMEM(newlength+1));
#if OptJr
      STRINGCAT( mystring , mylength , sother , otherlength ) ;
#else
      STRCAT(mystring,mylength,sother,otherlength);
#endif
    }
    else {
      mystring =Allocate(ROUNDMEM(newlength+1));
#if OptJr
      ASCIISTRINGCOPY( mystring , sother , newlength ) ;
#else
      STRCPY(mystring,sother,newlength);
#endif
    }
    mylength = newlength;
#if !OptJr
    mystring[mylength] = '\0';
#endif
  }
}

// ---------------------------------------------------------------------------
// Capitalize
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Capitalize()
{
  for (int i=0; i < mylength; i++) {
    if  ( i==0 ) mystring[i] = toupper(mystring[i]);
    else         mystring[i] = tolower(mystring[i]);    
  }
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
    Standard_NegativeValue::Raise();
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
    Standard_Character anUpperChar = toupper(aChar);
    for (int i=0; i < mylength; i++)
      if (toupper(mystring[i]) == anUpperChar) mystring[i] = NewChar;
  }
}

// ----------------------------------------------------------------------------
// Clear
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Clear()
{
  if (mystring) Standard::Free((void*&)mystring);
  mylength = 0;
  mystring = Allocate(mylength+1);
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// Copy
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Copy(const Standard_CString fromwhere)
{

  if (fromwhere) {
    Standard_Integer newlength;
#if OptJr
    STRINGLEN( fromwhere , newlength ) ;
#else
    STRLEN(fromwhere,newlength);
#endif
    if (mystring) {
      
      mystring = Reallocate((void*&)mystring,
							ROUNDMEM(newlength+1));
     
    }
    else {
      mystring = Allocate( ROUNDMEM(newlength + 1) );
    }
#if OptJr
    CSTRINGCOPY( mystring , fromwhere , newlength ) ;
    mylength = newlength;
#else
    STRCPY(mystring, fromwhere,newlength);
    mystring[newlength] = '\0';
    mylength = newlength;
#endif
  }
  else {
    if (mystring) {
      mylength = 0;
      mystring[mylength] = '\0';
    }
  }
}

// ----------------------------------------------------------------------------
// Copy
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Copy(const TCollection_AsciiString& fromwhere)
{

  if (fromwhere.mystring) {
    Standard_Integer newlength = fromwhere.mylength;
    if (mystring) {
      
      mystring = Reallocate((void*&)mystring,
							ROUNDMEM(newlength+1));
      
    }
    else {
      mystring =Allocate(ROUNDMEM(newlength+1));
    }
#if OptJr
    ASCIISTRINGCOPY( mystring , fromwhere.mystring , newlength ) ;
    mylength = newlength;
#else
    STRCPY(mystring, fromwhere.mystring,newlength);
    mylength = newlength;
    mystring[mylength] = '\0';
#endif
  }
  else {
    if (mystring) {
      mylength = 0;
      mystring[mylength] = '\0';
    }
  }

}

// ----------------------------------------------------------------------------
// Destroy
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Destroy()
{
  if (mystring) Standard::Free((void*&)mystring);
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
  Standard_OutOfRange::Raise();
  return 0;
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
  Standard_OutOfRange::Raise();
  return 0;
}

//----------------------------------------------------------------------------
// Insert a character before 'where'th character
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Insert(const Standard_Integer where,
                                     const Standard_Character what)
{
  if (where > mylength + 1 ) Standard_OutOfRange::Raise
        ("TCollection_AsciiString::Insert : Parameter where is too big");
  if (where < 0)             Standard_OutOfRange::Raise
        ("TCollection_AsciiString::Insert : Parameter where is negative");
  
  if (mystring) {
//    Standard_Integer length = *(int*)((long)mystring-8);
//    if((mylength+1) >> 4 > length-1 >> 4)
    mystring = Reallocate((void*&)mystring,
                                                       mylength+2);
  }
  else {
    mystring = Allocate(mylength+2);
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
void TCollection_AsciiString::Insert(const Standard_Integer where,
                                     const Standard_CString what)
{
  if (where <= mylength + 1) {
    if(what) {
      Standard_Integer whatlength;
#if OptJr
      STRINGLEN( what , whatlength );
#else
      STRLEN(what,whatlength);
#endif
      Standard_Integer newlength = mylength + whatlength;
      
      if (mystring) {
//      Standard_Integer length = *(int*)((long)mystring-8);
//      if(newlength >> 4 > length-1 >> 4)
        mystring = Reallocate((void*&)mystring,
                                                           newlength+1);
      }
      else {
        mystring =Allocate(newlength+1);
      }
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
    Standard_OutOfRange::Raise("TCollection_AsciiString::Insert : "
                               "Parameter where is too big");
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
      
      if (mystring) {
//      Standard_Integer length = *(int*)((long)mystring-8);
//      if(newlength >> 4 > length-1 >> 4)
        mystring = Reallocate((void*&)mystring,
                                                           newlength+1);
      }
      else {
        mystring =Allocate(newlength+1);
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
    Standard_OutOfRange::Raise("TCollection_AsciiString::Insert : "
                               "Parameter where is too big");
  }
}

//------------------------------------------------------------------------
//  InsertAfter
//------------------------------------------------------------------------
void TCollection_AsciiString::InsertAfter(const Standard_Integer Index,
                                          const TCollection_AsciiString& what)
{
   if (Index < 0 || Index > mylength) Standard_OutOfRange::Raise();
   Insert(Index+1,what);
}

//------------------------------------------------------------------------
//  InsertBefore
//------------------------------------------------------------------------
void TCollection_AsciiString::InsertBefore(const Standard_Integer Index,
                                           const TCollection_AsciiString& what)
{
   if (Index < 1 || Index > mylength) Standard_OutOfRange::Raise();
   Insert(Index,what);
}

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsEqual
                                        (const Standard_CString other)const
{
  int otherlength;

  if (other) {
#if OptJr
//    STRINGLEN( other , otherlength );
#else
    STRLEN(other,otherlength);
    if(mylength != otherlength) return Standard_False;
#endif
#if OptJr
    Standard_Boolean KEqual ;
//    CSTRINGEQUAL( mystring , other , mylength , KEqual ) ;
    if ( mystring ) {
      LCSTRINGEQUAL( mystring , mylength , other , KEqual ) ;
      return KEqual ;
    }

    STRINGLEN( other , otherlength );
    if ( mylength != otherlength )
      return Standard_False ;
    return Standard_True;
#else
    for (int i = 0 ; i < mylength ; i++)
      if (mystring[i] != other[i]) return Standard_False;
    return Standard_True;
#endif
  }
  Standard_NullObject::Raise("TCollection_AsciiString::Operator == "
                             "Parameter 'other'");
  return Standard_False;
}

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsEqual
                                (const TCollection_AsciiString& other)const
{

  if (mylength != other.mylength) return Standard_False;
#if OptJr
  Standard_Boolean KEqual ;
  ASCIISTRINGEQUAL( mystring , other.mystring , mylength , KEqual ) ;
  return KEqual ;
#else
  Standard_CString sother = other.mystring;  
  for (int i = 0 ; i < mylength ; i++)
    if (mystring[i] != sother[i]) return Standard_False;
  return Standard_True;
#endif
}

// ----------------------------------------------------------------------------
// IsEmpty
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsEmpty() const
{
  return (mylength == 0);
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsDifferent
                                        (const Standard_CString other)const
{
 if (other) {
#if OptJr
//   STRINGLEN( other , otherlength );
#else
   STRLEN(other,otherlength);
   if(mylength != otherlength) return Standard_True;
#endif
#if OptJr
   Standard_Boolean KEqual ;
//   CSTRINGEQUAL( mystring , other , mylength , KEqual ) ;
   if ( mystring ) {
     LCSTRINGEQUAL( mystring , mylength , other , KEqual ) ;
     return !KEqual ;
   }
//   STRINGLEN( other , otherlength );
   return Standard_True;

#else
 int otherlength;

   for (int i = 0 ; i < mylength ; i++)
     if (mystring[i] != other[i]) return Standard_True;
   return Standard_False;
    if ( mylength != otherlength )
      return Standard_False ;
    return Standard_True;
#endif
 }
 Standard_NullObject::Raise("TCollection_AsciiString::Operator != "
                            "Parameter 'other'");
 return Standard_False;
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsDifferent
                                (const TCollection_AsciiString& other)const
{

  if (mylength != other.mylength) return Standard_True;
#if OptJr
  Standard_Boolean KEqual ;
  ASCIISTRINGEQUAL( mystring , other.mystring , mylength , KEqual ) ;
  return !KEqual ;
#else
  Standard_CString sother = other.mystring;  
  for (int i = 0 ; i < mylength ; i++)
    if (mystring[i] != sother[i]) return Standard_True;
  return Standard_False;
#endif
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsLess
                                        (const Standard_CString other)const
{
  if (other) {
#if OptJr
    
    Standard_Boolean KLess ;

//    STRINGLEN( other , otherlength );
//    CSTRINGLESS( mystring , mylength , other , otherlength ,
//                 INF( mylength , otherlength ) , KLess ) ;
    LCSTRINGLESS( mystring , mylength , other , KLess ) ;
    return KLess ;
#else
    Standard_Integer otherlength;
    Standard_Integer i = 0, j = 0;
    STRLEN(other,otherlength);
    while ( i < mylength && j < otherlength) {
      if (mystring[i] < other[j]) return Standard_True;
      if (mystring[i] > other[j]) return Standard_False;
      i++ ; 
      j++;
    }
    if (i == mylength && j < otherlength) return Standard_True;
    return Standard_False;
#endif
  }
  Standard_NullObject::Raise("TCollection_AsciiString::Operator < "
                             "Parameter 'other'");
  return Standard_False;
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsLess
                                (const TCollection_AsciiString& other)const
{
#if OptJr
  
  Standard_Boolean KLess ;
  ASCIISTRINGLESS( mystring , mylength , other.mystring , other.mylength ,
                   INF( mylength , other.mylength ) , KLess ) ;
  return KLess ;

#else
  Standard_Integer i = 0, j = 0;
  Standard_Integer otherlength = other.mylength;
  Standard_CString sother = other.mystring;  
  while ( i < mylength && j < otherlength) {
    if (mystring[i] < sother[j]) return Standard_True;
    if (mystring[i] > sother[j]) return Standard_False;
    i++ ; 
    j++;
  }
  if (i == mylength && j < otherlength) return Standard_True;
  return Standard_False;
#endif
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsGreater
                                        (const Standard_CString other)const
{
  if (other) {
#if OptJr
    
    Standard_Boolean KGreater ;

//    STRINGLEN( other , otherlength );
//    CSTRINGGREATER( mystring , mylength , other , otherlength ,
//                    INF( mylength , otherlength ) , KGreater ) ;
    LCSTRINGGREATER( mystring , mylength , other , KGreater ) ;
    return KGreater ;
#else
    Standard_Integer otherlength;
    Standard_Integer i = 0, j = 0;
    STRLEN(other,otherlength);
    while (i < mylength && j <= otherlength) {
      if (mystring[i] < other[j]) return Standard_False;
      if (mystring[i] > other[j]) return Standard_True;
      i++ ; 
      j++;
    }
    if (j == otherlength && i < mylength) return Standard_True;
    return Standard_False;
#endif
  }
  Standard_NullObject::Raise("TCollection_AsciiString::Operator > "
                             "Parameter 'other'");
  return Standard_False;
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsGreater
                                (const TCollection_AsciiString& other)const
{
#if OptJr
    
    Standard_Boolean KGreater ;

    ASCIISTRINGGREATER( mystring , mylength , other.mystring , other.mylength,
                        INF( mylength , other.mylength ) , KGreater ) ;
    return KGreater ;
#else
  Standard_Integer i = 0, j = 0;
  Standard_Integer otherlength = other.mylength;
  Standard_CString sother = other.mystring;  
  while (i < mylength && j <= otherlength) {
    if (mystring[i] < sother[j]) return Standard_False;
    if (mystring[i] > sother[j]) return Standard_True;
    i++ ; 
    j++;
  }
  if (j == otherlength && i < mylength) return Standard_True;
  return Standard_False;
#endif
}

// ----------------------------------------------------------------------------
// IntegerValue
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::IntegerValue()const
{
  char *ptr;
  Standard_Integer value = 0; 
  if(mystring) {
    value = (int)strtol(mystring,&ptr,10); 
    if (ptr != mystring) return value;
  }
  Standard_NumericError::Raise("TCollection_AsciiString::IntegerValue");
  return value;
}

// ----------------------------------------------------------------------------
// IsIntegerValue
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsIntegerValue()const
{
  char *ptr;
  if(mystring) {
//#ifdef DEB
//    Standard_Integer value = (int)strtol(mystring,&ptr,10); 
//#else
    strtol(mystring,&ptr,10); 
//#endif
    if (ptr != mystring) {
      for (int i=0; i < mylength; i++) {
        if (mystring[i] == '.') return Standard_False;
      }
      return Standard_True;
    }
  }
  return Standard_False;
}

// ----------------------------------------------------------------------------
// IsRealValue
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::IsRealValue()const
{
  char *ptr;
  if(mystring) {
//#ifdef DEB
//    Standard_Real value = strtod(mystring,&ptr);
//#else
    strtod(mystring,&ptr);
//#endif
    if (ptr != mystring) return Standard_True;
    else                 return Standard_False; 
  }
  return Standard_False;
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
     if (mystring) {
//       Standard_Integer length = *(int*)((long)mystring-8);
//       if(Width >> 4 > length-1 >> 4)
       mystring = Reallocate((void*&)mystring,
                                                          Width+1);
     }
     else {
       mystring = Allocate(Width+1);
     }
     for (int i = mylength; i < Width ; i++) mystring[i] = Filler;
     mylength = Width;
     mystring[mylength] = '\0';
   }
   else if (Width < 0) {
     Standard_NegativeValue::Raise();
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
   Standard_OutOfRange::Raise();
   return 0 ;
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
  Standard_OutOfRange::Raise();
  return 0;
}

// ----------------------------------------------------------------------------
// LowerCase
// ----------------------------------------------------------------------------
void TCollection_AsciiString::LowerCase()
{
  for (int i=0; i < mylength; i++)
    mystring[i] = tolower(mystring[i]);
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
  Standard_Real value = 0;
  if(mystring) {
    value = strtod(mystring,&ptr);
    if (ptr != mystring) return value;
  }
  Standard_NumericError::Raise("TCollection_AsciiString::RealValue");
  return value;
}

// ----------------------------------------------------------------------------
// Read
//--------------------------------------------------------------------------
void TCollection_AsciiString::Read(Standard_IStream& astream)
{

 int newlength;

 union { int bid ;
         Standard_Character buffer[8190]; } CHN ;
 astream >> CHN.buffer;               // Get characters from astream

#if OptJr
 STRINGLEN( CHN.buffer , newlength );
#else
 STRLEN(CHN.buffer,newlength);
#endif
 if (mystring) {
//   Standard_Integer length = *(int*)((long)mystring-8);
//   if(newlength >> 4 > length-1 >> 4)
   mystring = Reallocate((void*&)mystring,
                                                      ROUNDMEM(newlength+1));
 }
 else {
   mystring =Allocate(ROUNDMEM(newlength+1));
 }
#if OptJr
 ASCIISTRINGCOPY( mystring , CHN.buffer , newlength ) ;
 mylength = newlength;
#else
 STRCPY(mystring,CHN.buffer,newlength);
 mylength = newlength;
 mystring[mylength] = '\0';
#endif
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
    Standard_Character upperwhat = toupper(what);
    for (int i=0; i < mylength; i++) { 
      if (toupper(mystring[i]) != upperwhat) mystring[c++] = mystring[i];
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
  Standard_OutOfRange::Raise("TCollection_AsciiString::Remove: "
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
    if (mystring) {
//      Standard_Integer length = *(int*)((long)mystring-8);
//      if(Width >> 4 > length-1 >> 4)
      mystring = Reallocate((void*&)mystring,
                                                         Width+1);
    }
    else {
      mystring =Allocate(Width+1);
    }
    for ( i = mylength-1, k = Width-1 ; i >= 0 ; i--, k--) 
      mystring[k] = mystring[i];
    for(; k >= 0 ; k--) mystring[k] = Filler;
    mylength = Width;
    mystring[mylength] = '\0';
  }
  else if (Width < 0) {
    Standard_NegativeValue::Raise();
  }
}

// ----------------------------------------------------------------------------
// Search
// ----------------------------------------------------------------------------
Standard_Integer TCollection_AsciiString::Search
                                        (const Standard_CString what)const
{
  Standard_Integer size;
#if OptJr
  STRINGLEN( what , size );
#else
  STRLEN(what,size);
#endif
  if (size) {
    int k,j;
    int i = 0;
    Standard_Boolean find = Standard_False; 
    while ( i < mylength-size+1 && !find) {
      k = i++;
      j = 0;
      while (j < size && mystring[k++] == what[j++])
        if (j == size) find = Standard_True;
    }
    if (find)  return i;
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
Standard_Integer TCollection_AsciiString::SearchFromEnd
                                        (const Standard_CString what)const
{
  int size;
#if OptJr
  STRINGLEN( what , size );
#else
  STRLEN(what,size);
#endif
  if (size) {
    int k,j;
    int i = mylength-1;
    Standard_Boolean find = Standard_False; 
    while ( i >= size-1 && !find) {
      k = i--;
      j = size-1;
      while (j >= 0 && mystring[k--] == what[j--])
        if (j == -1) find = Standard_True;
    }
    if (find)  return i-size+3;
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
void TCollection_AsciiString::SetValue(const Standard_Integer where,
                                       const Standard_Character what)
{
  if (where > 0 && where <= mylength) {
    mystring[where-1] = what;
  }
  else {
    Standard_OutOfRange::Raise("TCollection_AsciiString::SetValue : "
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
   Standard_Integer size;
#if OptJr
   STRINGLEN( what , size );
#else
   STRLEN(what,size);
#endif
   size += (where - 1);  
   if (size >= mylength){
     if (mystring) {
//       Standard_Integer length = *(int*)((long)mystring-8);
//       if(size >> 4 > length-1 >> 4)
       mystring = Reallocate((void*&)mystring,
                                                          size+1);
     }
     else {
       mystring =Allocate(size+1);
     }
     mylength = size;
   } 
   for (int i = where-1; i < size; i++)
     mystring[i] = what[i-(where-1)];
   mystring[mylength] = '\0';
 }
 else {
   Standard_OutOfRange::Raise("TCollection_AsciiString::SetValue : "
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
   if (size >= mylength){
     if (mystring) {
//       Standard_Integer length = *(int*)((long)mystring-8);
//       if(size >> 4 > length-1 >> 4)
       mystring = Reallocate((void*&)mystring,
                                                          size+1);
     }
     else {
       mystring =Allocate(size+1);
     }
     mylength = size;
   } 
   for (int i = where-1; i < size; i++)
     mystring[i] = swhat[i-(where-1)];
   mystring[mylength] = '\0';
 }
 else {
   Standard_OutOfRange::Raise("TCollection_AsciiString::SetValue : "
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
  Standard_OutOfRange::Raise("TCollection_AsciiString::Split index");
  return ;
}

// ----------------------------------------------------------------------------
// Split
// ----------------------------------------------------------------------------
TCollection_AsciiString TCollection_AsciiString::Split
                                                (const Standard_Integer where)
{
  if (where >= 0 && where <= mylength) {
#if OptJr
    TCollection_AsciiString res( &mystring[where] , mylength - where );
#else
    TCollection_AsciiString res(&mystring[where]);
#endif
    Trunc(where);
    return res;
  }
  Standard_OutOfRange::Raise("TCollection_AsciiString::Split index");
  TCollection_AsciiString res;
  return res;
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
    Standard_OutOfRange::Raise();
  Standard_Integer newlength = ToIndex-FromIndex+1;
  res.mystring =Allocate(ROUNDMEM(newlength+1)); 

#if OptJr
  CSTRINGCOPY( res.mystring , &(mystring [ FromIndex - 1 ]) , newlength );
#else
  memcpy(res.mystring, &(mystring[FromIndex-1]),newlength);
#endif
  res.mystring[newlength] = '\0';
  res.mylength = newlength;
  return ;
}

#if !OptJr
// ----------------------------------------------------------------------------
// SubString
// ----------------------------------------------------------------------------
TCollection_AsciiString TCollection_AsciiString::SubString
                                (const Standard_Integer FromIndex,
                                 const Standard_Integer ToIndex) const
{

  if (ToIndex > mylength || FromIndex <= 0 || FromIndex > ToIndex )
    Standard_OutOfRange::Raise();

#if OptJr
  return TCollection_AsciiString( &mystring [ FromIndex - 1 ] ,
                                  ToIndex - FromIndex + 1 ) ;

#else
  TCollection_AsciiString res;
  Standard_Integer newlength = ToIndex-FromIndex+1;
  res.mystring = Allocate(ROUNDMEM(newlength+1)); 
#if OptJr
  CSTRINGCOPY( res.mystring , &(mystring [ FromIndex - 1 ]) , newlength );
#else
  memcpy(res.mystring, &(mystring[FromIndex-1]),newlength);
#endif
  res.mystring[newlength] = '\0';
  res.mylength = newlength;
  return res;
#endif
}
#endif

// ----------------------------------------------------------------------------
// Token
// Private
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Token(const Standard_CString separators,
                                    const Standard_Integer whichone,
                                    TCollection_AsciiString& res)const
{
  
#if OptJr
  res = Token( separators , whichone ) ;

#else
  if (!separators)
    Standard_NullObject::Raise("TCollection_AsciiString::Token : "
                               "parameter 'separators'");

  int              i,j,k,l;
  Standard_CString buftmp =Allocate(mylength+1);
  Standard_Character aSep;
  
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
    Standard::Free((void*&)buftmp);
  }
  else {
    res.mystring = buftmp;
#if OptJr
    STRINGLEN( buftmp , res.mylength );
#else
    STRLEN(buftmp,res.mylength);
#endif
  }
#endif
  return ;
}

// ----------------------------------------------------------------------------
// Token
// ----------------------------------------------------------------------------
TCollection_AsciiString TCollection_AsciiString::Token
                                        (const Standard_CString separators,
                                         const Standard_Integer whichone) const
{
  if (!separators)
    Standard_NullObject::Raise("TCollection_AsciiString::Token : "
                               "parameter 'separators'");

#if OptJr
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

// Hereafter : Why write simple when we can write complicated ! ... 
#else
  TCollection_AsciiString res;
  
  int                        i,j,k,l;
  Standard_CString buftmp = Allocate(mylength+1);
  Standard_Character       aSep;
  
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
    Standard::Free((void*&)buftmp);
  }
  else {
    res.mystring = buftmp;
#if OptJr
    STRINGLEN( buftmp , res.mylength );
#else
    STRLEN(buftmp,res.mylength);
#endif
  }
  return res;
#endif
}

// ----------------------------------------------------------------------------
// Trunc
// ----------------------------------------------------------------------------
void TCollection_AsciiString::Trunc(const Standard_Integer ahowmany)
{
  if (ahowmany < 0 || ahowmany > mylength)
    Standard_OutOfRange::Raise("TCollection_AsciiString::Trunc : "
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
    mystring[i] = toupper(mystring[i]);
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
   if(mystring) return mystring[where-1];
   else                 return '\0';
 }
 Standard_OutOfRange::Raise("TCollection_AsciiString::Value : parameter where");
 return '\0';
}


//------------------------------------------------------------------------
//  ISSIMILAR
//------------------------------------------------------------------------
Standard_Boolean TCollection_AsciiString::ISSIMILAR
                                (const TCollection_AsciiString& string1,
                                 const TCollection_AsciiString& string2)
{
  if ( string1.Length() != string2.Length() )
    return Standard_False ;
  return ::ISSIMILAR (string1.ToCString() ,
                      string1.Length() ,
                      string2.ToCString() );
}
