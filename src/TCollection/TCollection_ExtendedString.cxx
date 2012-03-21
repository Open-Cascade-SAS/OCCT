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

#include <TCollection_ExtendedString.ixx>
#include <Standard.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ctype.hxx>
#include <Standard_String.hxx>
#include <TCollection_AsciiString.hxx>

static
  Standard_PExtCharacter Allocate(const Standard_Size aLength);

static
  Standard_PExtCharacter Reallocate(Standard_Address aAddr,
				    const Standard_Size aLength);

Standard_EXPORT short NULL_EXTSTRING[1] = {0};

//============================== input value have len = 2 bytes ====
inline Standard_ExtCharacter ConvertToUnicode2B (unsigned char *p)
{
  // *p, *(p+1)
  union {
    struct {
      unsigned char  h;
      unsigned char  l;
    } hl;
    Standard_ExtCharacter chr;
  } EL;

  unsigned char l = *(p+1);
  l |= 0xC0;
  unsigned char h = *p;
  h <<= 6;
  h |= 0x3F;
  l &= h;// l is defined
  h = *p;
  h >>= 2;
  h &= 0x07;
  EL.hl.l = l;
  EL.hl.h = h;
  return EL.chr;
}

//============================== input value have len = 3 bytes ====
inline Standard_ExtCharacter ConvertToUnicode3B (unsigned char *p)
{
  // *p, *(p+1), *(p+2) =>0 , 1, 2
  union {
    struct {
      unsigned char  h;
      unsigned char  l;
    } hl;
    Standard_ExtCharacter chr;
  } EL;


  unsigned char h = *(p+1);//h = 10yyyyyy
  unsigned char l = *(p+2);//h = 10zzzzzz

  l |= 0xC0;
  h <<= 6;   //yy------
  h |= 0x3F; //yy111111
  l &= h;   //  yyzzzzzz - l is defined 
  EL.hl.l = l; 
 
  unsigned char a = *p;// a = 1110xxxx
  a <<= 4;//xxxx----
  a |= 0x0F;//a = xxxx1111
  h = *(p+1);
  h >>= 2;//----yyyy
  h |= 0xF0; //1111yyyy
  a &= h; // a = xxxxyyyy
  EL.hl.h = a; //h is defined
  return EL.chr;
}
//============================== returns number of symbols in UTF8 string ====
inline Standard_Integer  nbSymbols(const Standard_CString aStr) {
  Standard_Integer aLen = 0;// length in symbols
  int i = 0;
  while (aStr[i] != '\0') {  
    if((aStr[i] & 0x80) == 0x00) //1byte => 1 symb - Lat1
      {aLen++; i++;}
    else if((aStr[i] & 0xE0) == 0xC0 && 
	    (aStr[i+1] && 
	     (aStr[i+1] & 0xC0) == 0x80)) {//2 bytes => 1 symb
      aLen++;
      i += 2;
    } else if((aStr[i] & 0xF0) == 0xE0 && 
	      ((aStr[i+1] && (aStr[i+1] & 0xC0) == 0x80)) &&
	      (aStr[i+2] && (aStr[i+2] & 0xC0) == 0x80)) {
      aLen++;
      i += 3;
    } else 
      i++; 
  }
  return aLen;
}

//-----------------------------------------------------------------------------
// Create an empty ExtendedString
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString()
{
//  mystring = 0L;
  mylength = 0;
  mystring = Allocate((mylength+1)*2);
  mystring[mylength] = '\0';
}

//----------------------------------------------------------------------------
// Create an ExtendedString from a Standard_CString
//----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                          (const Standard_CString astring, 
					   const Standard_Boolean isMultiByte) 
{
  if (astring) {
    if(!isMultiByte) {
#if OptJr
      STRINGLEN( astring , mylength ) ;
#else
      STRLEN(astring,mylength);
#endif
      mystring = Allocate((mylength+1)*2);
      for (int i = 0 ; i < mylength ; i++)
	mystring[i] = ToExtCharacter(astring[i]); 
      mystring[mylength] =  '\0';
    }
    else {
      mylength = nbSymbols(astring);
      mystring = Allocate(ROUNDMEM((mylength+1)*2));
      if(!ConvertToUnicode (astring))
#ifdef DEB
	cout <<"UTF8 decoding failure..." <<endl;
#endif
      ;
    }
  }
  else {
    Standard_NullObject::Raise("TCollection_ExtendedString : "
                               "parameter 'astring'");
  }
}

//---------------------------------------------------------------------------
// Create an ExtendedString from an ExtString
//--------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_ExtString astring) 
{

  if (astring) {
#if OptJr
    EXTSTRINGLEN( astring , mylength ) ;
#else
    EXTSTRLEN(astring,mylength);
#endif
    mystring = Allocate(ROUNDMEM((mylength+1)*2));
#if OptJr
    EXTSTRINGCOPY( mystring , astring , mylength );
#else
    STRCPY(mystring,astring,mylength);
    mystring[mylength] =  '\0';
#endif
  }
  else {
    Standard_NullObject::Raise("TCollection_ExtendedString : null parameter ");
  }
}

// ----------------------------------------------------------------------------
// Create an asciistring from a Standard_Character
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_Character aChar)
{
  if ( aChar != '\0' ) {
    mylength    = 1;
    mystring    = Allocate(2*2);
    mystring[0] = ToExtCharacter(aChar);
    mystring[1] =  '\0';
  }
  else {
    //    mystring = 0L;
    mylength = 0;
    mystring = Allocate((mylength+1)*2);
    mystring[mylength] = '\0';
  }
}

//--------------------------------------------------------------------------
// Create a string from a ExtCharacter
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_ExtCharacter aChar)
{
  mylength    = 1;
  mystring    = Allocate(2*2);
  mystring[0] = aChar;
  mystring[1] =  '\0';
}

// ----------------------------------------------------------------------------
// Create an AsciiString from a filler
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_Integer      length,
                                         const Standard_ExtCharacter filler )
{
  mystring = Allocate((length+1)*2);
  mylength = length;
  for (int i = 0 ; i < length ; i++) mystring[i] = filler;
  mystring[length] =  '\0';
}

// ----------------------------------------------------------------------------
// Create a String from an Integer
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_Integer aValue)
{
  union {int bid ;
         char t [13];} CHN ;
  sprintf(&CHN.t[0],"%d",aValue);
#if OptJr
  STRINGLEN( CHN.t , mylength ) ;
#else
  STRLEN(CHN.t,mylength);
#endif
  mystring = Allocate((mylength+1)*2);
  for (int i = 0 ; i < mylength ; i++) mystring[i] = ToExtCharacter(CHN.t[i]);
  mystring[mylength] =  '\0';
}

// ----------------------------------------------------------------------------
// Create a String from a real
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                        (const Standard_Real aValue)
{
  union {int bid ;
         char t [50];} CHN ;
  sprintf(&CHN.t[0],"%g",aValue);
#if OptJr
  STRINGLEN( CHN.t , mylength ) ;
#else
  STRLEN(CHN.t,mylength);
#endif
  mystring = Allocate((mylength+1)*2);
  for (int i = 0 ; i < mylength ; i++) mystring[i] = ToExtCharacter(CHN.t[i]);
  mystring[mylength] =  '\0';
}

//-----------------------------------------------------------------------------
// create an extendedstring from an extendedstring
// ----------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                (const TCollection_ExtendedString& astring)
{

  mylength = astring.mylength;
  mystring = Allocate(ROUNDMEM((mylength+1)*2));
  if (astring.mystring)
#if OptJr
    EXTENDEDSTRINGCOPY( mystring , astring.mystring , mylength );
#else
    STRCPY(mystring,astring.mystring,mylength);
  mystring[mylength] =  '\0';
#endif
}

//---------------------------------------------------------------------------
//  Create an extendedstring from an AsciiString 
//---------------------------------------------------------------------------
TCollection_ExtendedString::TCollection_ExtendedString
                                (const TCollection_AsciiString& astring) 
{
  mylength = astring.Length();
  mystring = Allocate((mylength+1)*2);
#if OptJr
  Standard_CString aCString = astring.ToCString() ;
  for (Standard_Integer i = 0; i <= mylength ; i++)
    mystring[i] = ToExtCharacter( aCString[i] ); 
#else
  for (Standard_Integer i = 0; i < mylength ; i++)
    mystring[i] = ToExtCharacter(astring.Value(i+1)); 
  mystring[mylength] =  '\0';
#endif
}

// ----------------------------------------------------------------------------
//  AssignCat
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::AssignCat
                                (const TCollection_ExtendedString& other) 
{
  Standard_Integer otherlength = other.mylength; 
  if (otherlength) {
    Standard_ExtString sother = other.mystring;
    Standard_Integer newlength = mylength +otherlength; 
    if (mystring) {
        mystring = Reallocate((void*&)mystring,
                                                             ROUNDMEM((newlength+1)*2));
#if OptJr
//      if ( ((long ) ( &mystring[ mylength ] ) & 3) == 0 ) {
      EXTENDEDSTRINGCAT( mystring , mylength , sother , otherlength ) ;
//        }
//      else
//        STRCAT( mystring , mylength , sother , otherlength + 1 ) ;
#else
      STRCAT(mystring,mylength,sother,otherlength);
#endif
    }
    else {
      mystring = Allocate(ROUNDMEM((newlength+1)*2));
#if OptJr
      EXTENDEDSTRINGCOPY( mystring , sother , newlength );
#else
      STRCPY(mystring,sother,newlength);
#endif
    }
    mylength = newlength;
#if !OptJr
    mystring[mylength] =  '\0';
#endif
  }
}

// ----------------------------------------------------------------------------
// Cat
// ----------------------------------------------------------------------------
TCollection_ExtendedString TCollection_ExtendedString::Cat
                                (const TCollection_ExtendedString& other) const 
{
  Standard_ExtString sother = other.mystring;
  const Standard_Integer otherlength = sother ? other.mylength : 0;
  const Standard_Integer newlength = mylength + otherlength;
#ifdef OptJr
  // ensure rounding allocated memory to 4 bytes
  TCollection_ExtendedString res (newlength | 0x1, 0);
  res.mylength = newlength;
#else
  TCollection_ExtendedString res (newlength, 0);
#endif
 
  if (otherlength) {
#if OptJr
    EXTENDEDSTRINGCOPY( res.mystring , mystring , mylength );
      //    if ( ((long ) ( &res.mystring[ mylength ] ) & 3) == 0 ) {
    EXTENDEDSTRINGCAT( res.mystring , mylength , sother , otherlength ) ;
//      }
//    else
//      STRCAT( res.mystring , mylength , sother , otherlength + 1 ) ;
#else
    if (mylength > 0) STRCPY (res.mystring, mystring, mylength);
    STRCPY (&res.mystring[mylength], sother, otherlength);
    res.mystring[newlength] =  '\0';
#endif
  }
  else if (mylength > 0) {
//    TCollection_ExtendedString res;
//    res.mystring = (Standard_ExtString)Standard::Allocate((mylength+1)*2);
#if OptJr
    EXTENDEDSTRINGCOPY( res.mystring , mystring , mylength );
#else
    STRCPY (res.mystring, mystring,mylength);
    res.mystring[res.mylength] =  '\0';
#endif
  }
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
  if (mystring) Standard::Free((void*&)mystring);
//  mystring = 0L;
  mylength = 0;
  mystring = Allocate((mylength+1)*2);
  mystring[mylength] = '\0';
}

// ----------------------------------------------------------------------------
// Copy
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Copy (const TCollection_ExtendedString& fromwhere)
{

  if (fromwhere.mystring) {
    Standard_Integer newlength = fromwhere.mylength;
    if (mystring) {
      mystring = Reallocate((void*&)mystring, ROUNDMEM(( newlength + 1)*2 ));
    }
    else {
      mystring = Allocate(ROUNDMEM((newlength+1)*2));
    }
    mylength = newlength;
#if OptJr
    EXTENDEDSTRINGCOPY( mystring , fromwhere.mystring , newlength );
#else
    STRCPY(mystring, fromwhere.mystring,newlength);
    mystring[mylength] =  '\0';
#endif
  }
  else {
    if (mystring) {
      mylength = 0;
      mystring[mylength] =  '\0';
    }
  }
}

// ----------------------------------------------------------------------------
// Destroy
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Destroy()
{
  if (mystring) Standard::Free((void*&)mystring);
  mystring = 0L;
}

//----------------------------------------------------------------------------
// Insert a character before 'where'th character
// ----------------------------------------------------------------------------
void TCollection_ExtendedString::Insert(const Standard_Integer where,
                                        const Standard_ExtCharacter what)
{
  if (where > mylength + 1 )
    Standard_OutOfRange::Raise("TCollection_ExtendedString::Insert : "
                               "Parameter where is too big");
  if (where < 0)
    Standard_OutOfRange::Raise("TCollection_ExtendedString::Insert : "
                               "Parameter where is negative");

  if (mystring) {
      mystring = Reallocate((void*&)mystring,
                                                           (mylength+2)*2);
  }
  else {
    mystring = Allocate((mylength+2)*2);
  }
  if (where != mylength +1) {
    for (int i=mylength-1; i >= where-1; i--)
      mystring[i+1] = mystring[i];
  }
  mystring[where-1] = what;
  mylength++;
  mystring[mylength] =  '\0';
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
          mystring = Reallocate(
                                         (void*&)mystring,(newlength+1)*2);
      }
      else {
        mystring = Allocate((newlength+1)*2);
      }
      if (where != mylength +1) {
        for (int i=mylength-1; i >= where-1; i--)
          mystring[i+whatlength] = mystring[i];
      }
      for (int i=0; i < whatlength; i++)
        mystring[where-1+i] = swhat[i];
      
      mylength = newlength;
      mystring[mylength] =  '\0';
    }
  }
  else {
    Standard_OutOfRange::Raise("TCollection_ExtendedString::Insert : "
                               "Parameter where is too big");
  }
}

#if OptJr
// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsEqual
                                (const Standard_ExtString other) const
{
// Standard_Integer otherlength ;

//  EXTSTRINGLEN( other , otherlength )
//  if ( mylength != otherlength ) return Standard_False;
  Standard_Boolean KEqual ;
  LEXTSTRINGEQUAL( mystring , mylength , other , KEqual ) ;
  return KEqual ;
}
#else
Standard_Boolean TCollection_ExtendedString::NoIsEqual
                                (const Standard_ExtString other) const
{return Standard_False ;}
#endif

// ----------------------------------------------------------------------------
// IsEqual
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsEqual
                                (const TCollection_ExtendedString& other) const
{
  if (mylength != other.mylength) return Standard_False;
  Standard_ExtString sother = other.mystring;  
#if OptJr
  Standard_Boolean KEqual ;
  EXTENDEDSTRINGEQUAL( mystring , sother , mylength , KEqual ) ;
  return KEqual ;
#else
  for (int i = 0 ; i < mylength ; i++)
    if (mystring[i] != sother[i]) return Standard_False;
  return Standard_True;
#endif
}

#if OptJr
// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsDifferent
                                (const Standard_ExtString other ) const
{
// Standard_Integer otherlength ;

//  EXTSTRINGLEN( other , otherlength )
//  if ( mylength != otherlength ) return Standard_True;
  Standard_Boolean KEqual ;
  LEXTSTRINGEQUAL( mystring , mylength , other , KEqual ) ;
  return !KEqual ;
}
#else
Standard_Boolean TCollection_ExtendedString::NoIsDifferent
                                (const Standard_ExtString other ) const
{return Standard_False ;}
#endif

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsDifferent
                                (const TCollection_ExtendedString& other) const
{
  if (mylength != other.mylength) return Standard_True;
  Standard_ExtString sother = other.mystring;  
#if OptJr
  Standard_Boolean KEqual ;
  EXTENDEDSTRINGEQUAL( mystring , sother , mylength , KEqual ) ;
  return !KEqual ;
#else
  for (int i = 0 ; i < mylength ; i++)
    if (mystring[i] != sother[i]) return Standard_True;
  return Standard_False;
#endif
}

#if OptJr
// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsLess
                                (const Standard_ExtString other) const
{
// Standard_Integer otherlength ;

//  EXTSTRINGLEN( other , otherlength )
  Standard_Boolean KLess ;
//  EXTSTRINGLESS( mystring , mylength , other , otherlength ,
//                 INF( mylength , otherlength ) , KLess ) ;
  LEXTSTRINGLESS( mystring , mylength , other , KLess ) ;
  return KLess ;
}
#else
Standard_Boolean TCollection_ExtendedString::NoIsLess
                                (const Standard_ExtString other) const
{return Standard_False ;}
#endif

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsLess
                                (const TCollection_ExtendedString& other) const
{

  Standard_Integer otherlength = other.mylength;
  Standard_ExtString sother = other.mystring;  
#if OptJr
  Standard_Boolean KLess ;
  EXTENDEDSTRINGLESS( mystring , mylength , sother , otherlength ,
                      INF( mylength , otherlength ) , KLess ) ;
  return KLess ;
#else
  Standard_Integer i = 0, j = 0;
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

#if OptJr
// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsGreater
                                (const Standard_ExtString other) const
{
// Standard_Integer otherlength ;

//  EXTSTRINGLEN( other , otherlength )
  Standard_Boolean KGreater ;
//  EXTSTRINGGREATER( mystring , mylength , other , otherlength ,
//                    INF( mylength , otherlength ) , KGreater ) ;
  LEXTSTRINGGREATER( mystring , mylength , other , KGreater ) ;
  return KGreater ;
}
#else
Standard_Boolean TCollection_ExtendedString::NoIsGreater
                                (const Standard_ExtString other) const
{return Standard_False ;}
#endif

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::IsGreater
                                (const TCollection_ExtendedString& other) const
{
  Standard_Integer otherlength = other.mylength;
  Standard_ExtString sother = other.mystring;  
#if OptJr
  Standard_Boolean KGreater ;
  EXTENDEDSTRINGGREATER( mystring , mylength , sother , otherlength ,
                         INF( mylength , otherlength ) , KGreater ) ;
  return KGreater ;
#else
  Standard_Integer i = 0, j = 0;
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
    Standard_OutOfRange::Raise("TCollection_ExtendedString::Remove: "
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
    Standard_OutOfRange::Raise("TCollection_ExtendedString::SetValue : parameter where");
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
        mystring = Reallocate ((void*&)mystring,(size+1)*2);
      }
      else {
        mystring = Allocate((size+1)*2);
      }
      mylength = size;
    } 
    for (int i = where-1; i < size; i++)
      mystring[i] = swhat[i-(where-1)];
    mystring[mylength] =  '\0';
  }
  else
    Standard_OutOfRange::Raise("TCollection_ExtendedString::SetValue : "
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
  Standard_OutOfRange::Raise("TCollection_ExtendedString::Split index");
  TCollection_ExtendedString res;
  return res;
}

// ----------------------------------------------------------------------------
// Token
// ----------------------------------------------------------------------------
TCollection_ExtendedString TCollection_ExtendedString::Token
                                      (const Standard_ExtString separators,
                                       const Standard_Integer   whichone) const
{
  TCollection_ExtendedString res("");
  if (!separators)
    Standard_NullObject::Raise("TCollection_ExtendedString::Token : "
                               "parameter 'separators'");
  
  int                   i,j,k,l;
  Standard_PExtCharacter  buftmp = Allocate((mylength+1)*2); 
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
    Standard::Free((void*&)buftmp);
  }
  else {
    res.mystring = buftmp;
#if OptJr
    EXTSTRINGLEN( buftmp , res.mylength ) ;
#else
    EXTSTRLEN(buftmp,res.mylength);
#endif
  }
  return res;
}

// ----------------------------------------------------------------------------
// ToExtString
// ----------------------------------------------------------------------------
const Standard_ExtString TCollection_ExtendedString::ToExtString() const
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
    Standard_OutOfRange::Raise("TCollection_ExtendedString::Trunc : "
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
 Standard_OutOfRange::Raise("TCollection_ExtendedString::Value : "
                            "parameter where");
 return 0;
}


//----------------------------------------------------------------------------
// Convert CString (including multibyte case) to UniCode representation
//----------------------------------------------------------------------------
Standard_Boolean TCollection_ExtendedString::ConvertToUnicode 
                                                (const Standard_CString aStr)
{
  Standard_Boolean aRes = Standard_True;
  short * p = mystring;
  int i = 0;
  while (aStr[i] != '\0') { 
    if((aStr[i] & 0x80) == 0x00) //1byte => 1 symb - Lat1
      {*p++ = ToExtCharacter(aStr[i]); i++;}
    else if((aStr[i] & 0xE0) == 0xC0 && 
	    (aStr[i+1] && 
	     (aStr[i+1] & 0xC0) == 0x80)) {//2 bytes => 1 symb
      *p++ = ConvertToUnicode2B((unsigned char*)&aStr[i]);
      i += 2;
    } else if((aStr[i] & 0xF0) == 0xE0 && 
	      ((aStr[i+1] && (aStr[i+1] & 0xC0) == 0x80)) &&
	      (aStr[i+2] && (aStr[i+2] & 0xC0) == 0x80)) {      
      *p++ = ConvertToUnicode3B((unsigned char*)&aStr[i]);
      i += 3;
    } else { //unsupported case ==> not UTF8
      aRes = Standard_False;
      break;
    }
  }
  *p = 0x0000;
  return aRes;
}
//----------------------------------------------------------------------------
// Returns expected CString length in UTF8 coding.
//----------------------------------------------------------------------------
Standard_Integer TCollection_ExtendedString::LengthOfCString() const
{
  Standard_Integer i=0, aLen=0;
  while(mystring[i]) {
    if((mystring[i] & 0xFF80) == 0) 
      aLen++;
    else if((mystring[i] & 0xF800) == 0) 
      aLen +=2;
    else  
      aLen += 3;
    i++;
  }
 return aLen;
}

//----------------------------------------------------------------------------
// Converts the internal <mystring> to UTF8 coding and returns length of the 
// out CString.
//----------------------------------------------------------------------------
Standard_Integer TCollection_ExtendedString::ToUTF8CString(Standard_PCharacter& theCString) const
{
  Standard_Integer i=0, j=0;
  unsigned char a,b,c;
  while(mystring[i]) {
    if((mystring[i] & 0xFF80) == 0) {
      theCString[j++] = (char)mystring[i];
    } 
    else if((mystring[i] & 0xF800) == 0) {
      b = (unsigned char)mystring[i];//yyzzzzzz
      c = (unsigned char)mystring[i];//yyzzzzzz
      a = (unsigned char)(mystring[i]>>8);//00000yyy
      b &= 0x3F;
      b |= 0x80;
      a <<=2;
      a |= 0xC0;//110yyy00;
      c >>=6;
      c &= 0x03;
      a |=c;
      theCString[j++] = a;
      theCString[j++] = b;
    } else {
      b = (unsigned char)mystring[i];//yyzzzzzz
      c = (unsigned char)mystring[i];//yyzzzzzz
      unsigned char d = a = (unsigned char)(mystring[i]>>8);//xxxxyyyy
      c &= 0x3F;
      c |= 0x80;//10zzzzzz
      b >>= 6; //000000yy
      d <<= 2;//xxyyyy00;
      b |= d;
      b = (b & 0x3F) | 0x80;//10yyyyyy
      a >>= 4; //0000xxxx
      a |= 0xE0;//1110xxxx
      theCString[j++] = a;
      theCString[j++] = b;
      theCString[j++] = c;
    }
    i++;
  }
  theCString[j] = 0x00;
  return j;
}
//=======================================================================
//function : Allocate
//purpose  : 
//=======================================================================
Standard_PExtCharacter Allocate(const Standard_Size aLength)
{
  Standard_PExtCharacter pChar;
  //
  pChar=(Standard_PExtCharacter)Standard::Allocate(aLength);
  //
  return pChar;
}
//=======================================================================
//function : Reallocate
//purpose  : 
//=======================================================================
Standard_PExtCharacter Reallocate(Standard_Address aAddr,
				  const Standard_Size aLength)
{
  Standard_PExtCharacter pChar;
  //
  pChar= (Standard_PExtCharacter)Standard::Reallocate(aAddr, aLength);
  //
  return pChar;
}
