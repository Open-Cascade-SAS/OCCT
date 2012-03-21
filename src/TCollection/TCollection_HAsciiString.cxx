// Copyright (c) 1992-1999 Matra Datavision
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

#include <TCollection_HAsciiString.ixx>
#include <TCollection_HExtendedString.hxx>
#include <Standard_String.hxx>

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HAsciiString::TCollection_HAsciiString():myString(){}


// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HAsciiString::TCollection_HAsciiString
        (const Standard_CString message):myString(message)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HAsciiString::TCollection_HAsciiString
        (const TCollection_AsciiString& astring):myString(astring)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HAsciiString::TCollection_HAsciiString
        (const Standard_Character aChar):myString(aChar)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HAsciiString::TCollection_HAsciiString
   (const Standard_Integer length,const Standard_Character filler ):myString(length,filler)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HAsciiString::TCollection_HAsciiString
        (const Standard_Integer aValue):myString(aValue)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HAsciiString::TCollection_HAsciiString
        (const Standard_Real aValue):myString(aValue)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HAsciiString::TCollection_HAsciiString
 (const Handle(TCollection_HAsciiString)& astring):myString(astring->String())
{
}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HAsciiString::TCollection_HAsciiString
  (const Handle(TCollection_HExtendedString)& astring,
   const Standard_Character replaceNonAscii)
: myString(astring->String(), replaceNonAscii)
{
}


// ---------------------------------------------------------------------------
// Capitalize
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::Capitalize()
{
   myString.Capitalize();
}

// ---------------------------------------------------------------------------
// Cat
// ----------------------------------------------------------------------------
Handle(TCollection_HAsciiString) 
            TCollection_HAsciiString::Cat(const Standard_CString other) const
{
  return new TCollection_HAsciiString(myString.Cat(other));
}

// ---------------------------------------------------------------------------
// Cat
// ----------------------------------------------------------------------------
Handle(TCollection_HAsciiString)
  TCollection_HAsciiString::Cat(const Handle(TCollection_HAsciiString)& other)
const 
{
  return new TCollection_HAsciiString(myString.Cat(other->String() ) );
}

// ---------------------------------------------------------------------------
// Center
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::Center
                      (const Standard_Integer Width ,
                       const Standard_Character Filler) 
{
   if (Width < 0) Standard_NegativeValue::Raise();
   myString.Center(Width,Filler);
}

// ----------------------------------------------------------------------------
// ChangeAll
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::ChangeAll
                       (const Standard_Character aChar,
                        const Standard_Character NewChar,
                        const Standard_Boolean CaseSensitive)
{
   myString.ChangeAll(aChar,NewChar,CaseSensitive);
}

// ----------------------------------------------------------------------------
// Clear
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::Clear()
{
   myString.Clear();
}

// ----------------------------------------------------------------------------
// FirstLocationInSet
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HAsciiString::FirstLocationInSet
            (const Handle(TCollection_HAsciiString)& Set,
             const Standard_Integer FromIndex,
             const Standard_Integer ToIndex) const
{
   if (Length() == 0 || Set->Length() == 0) return 0;
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                     Standard_OutOfRange::Raise();
   return (myString.FirstLocationInSet(Set->String(),FromIndex,ToIndex));
}

// ----------------------------------------------------------------------------
// FirstLocationNotInSet
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HAsciiString::FirstLocationNotInSet
            (const Handle(TCollection_HAsciiString)& Set,
             const Standard_Integer FromIndex,
             const Standard_Integer ToIndex) const
{
   if (Length() == 0 || Set->Length() == 0) return 0;
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                     Standard_OutOfRange::Raise();
   return (myString.FirstLocationNotInSet(Set->String(),FromIndex,ToIndex));
}

// ----------------------------------------------------------------------------
// Insert a Standard_Character before 'where'th Standard_Character
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::Insert(const Standard_Integer where,
                                 const Standard_Character what)
{
   myString.Insert(where,what);
}

// ----------------------------------------------------------------------------
// Insert
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::Insert(const Standard_Integer where,
                                 const Standard_CString what)
{
   myString.Insert(where,what);
}

// ----------------------------------------------------------------------------
// Insert
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::Insert(const Standard_Integer where,
                                 const Handle(TCollection_HAsciiString)& what)
{
   myString.Insert(where,what->String());
}

//------------------------------------------------------------------------
//  InsertAfter
//------------------------------------------------------------------------
void TCollection_HAsciiString::InsertAfter 
           (const Standard_Integer Index, const Handle(TCollection_HAsciiString)& S)
{
   Standard_Integer size1 = Length();
#ifndef NOBOUNDCHECK
   if (Index < 0 || Index > size1) Standard_OutOfRange::Raise();
#endif
   myString.InsertAfter(Index,S->String());
}

//------------------------------------------------------------------------
//  InsertBefore
//------------------------------------------------------------------------
void TCollection_HAsciiString::InsertBefore 
           (const Standard_Integer Index, const Handle(TCollection_HAsciiString)& S)
{
   Standard_Integer size1 = Length();
#ifndef NOBOUNDCHECK
   if (Index < 1 || Index > size1) Standard_OutOfRange::Raise();
#endif
   myString.InsertBefore(Index,S->String());
}

// ----------------------------------------------------------------------------
// IsEmpty
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HAsciiString::IsEmpty() const
{
   return (myString.Length() == 0);
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean 
TCollection_HAsciiString::IsLess(const Handle(TCollection_HAsciiString)& other) const
{
   return myString.IsLess(other->String());
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean 
TCollection_HAsciiString::IsGreater(const Handle(TCollection_HAsciiString)& other) const
{
   return myString.IsGreater(other->String());
}

// ----------------------------------------------------------------------------
// IntegerValue
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HAsciiString::IntegerValue() const
{
   return myString.IntegerValue();
}

// ----------------------------------------------------------------------------
// IsIntegerValue
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HAsciiString::IsIntegerValue() const 
{
   return myString.IsIntegerValue();
}


// ----------------------------------------------------------------------------
// IsRealvalue
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HAsciiString::IsRealValue() const
{
   return myString.IsRealValue();
}

// ----------------------------------------------------------------------------
// IsAscii
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HAsciiString::IsAscii() const 
{
   return myString.IsAscii();
}

// ----------------------------------------------------------------------------
// IsDifferent
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HAsciiString::IsDifferent
       (const Handle(TCollection_HAsciiString)& S) const 
{

  if(S.IsNull()) Standard_NullObject::Raise("TCollection_HAsciiString::IsDifferent");
#if OptJr
  if ( myString.Length() == S->Length() ) {
    Standard_Boolean KEqual ;
    ASCIISTRINGEQUAL( myString.ToCString() , S->ToCString() ,
                      myString.mylength , KEqual ) ;
    return !KEqual ;
  }
  else
    return Standard_True ;
#else
  return ( strcmp(myString.ToCString(), S->ToCString()) );
#endif
}

// ----------------------------------------------------------------------------
// IsSameString
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HAsciiString::IsSameString
       (const Handle(TCollection_HAsciiString)& S) const 
{

  if(S.IsNull()) Standard_NullObject::Raise("TCollection_HAsciiString::IsSameString");
#if OptJr
  if ( myString.Length() == S->Length() ) {
    Standard_Boolean KEqual ;
    ASCIISTRINGEQUAL( myString.ToCString() , S->ToCString() ,
                      myString.mylength , KEqual ) ;
    return KEqual ;
  }
  else
    return Standard_False ;
#else
  return ( !strcmp(myString.ToCString(), S->ToCString()) );
#endif
}

// ----------------------------------------------------------------------------
// IsSameString
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HAsciiString::IsSameString
                    (const Handle(TCollection_HAsciiString)& S ,
                     const Standard_Boolean CaseSensitive) const 
{
//   Handle(TCollection_HAsciiString) H1,H2;
//   H1 = UpperCase(This);
//   H2 = UpperCase(S);
//  return ( H1 == H2));

  if(S.IsNull()) Standard_NullObject::Raise("TCollection_HAsciiString::IsSameString");

  Standard_Integer size1 = Length();
  if ( size1 != S->Length() ) return Standard_False;
#if OptJr
  if ( CaseSensitive ) {
    Standard_Boolean KEqual ;
    ASCIISTRINGEQUAL( myString.ToCString() , S->String().ToCString() ,
                      size1 , KEqual ) ;
    return KEqual ;
  }
  else {
    for ( Standard_Integer i = 1 ; i <= size1; i++) {
       if ( toupper( Value(i) ) != toupper( S->Value(i) ) )
         return Standard_False;
     }
    return Standard_True ;
  }

#else
// Example of bad sequence of test : CaseSensitive does not change in the loop
  Standard_Character C1,C2;
  for( Standard_Integer i = 1 ; i <= size1; i++) {
    if(CaseSensitive){
      if (Value(i) != S->Value(i)) return Standard_False;
    }
    else {
      C1 = Value(i);
      C2 = S->Value(i);
      if(toupper(C1) != toupper(C2)) return Standard_False;
    }
  }
  return Standard_True;
#endif
}

//------------------------------------------------------------------------
//  LeftAdjust
//------------------------------------------------------------------------
void TCollection_HAsciiString::LeftAdjust ()
{
   myString.LeftAdjust();
}

//------------------------------------------------------------------------
//  LeftJustify
//------------------------------------------------------------------------
void TCollection_HAsciiString::LeftJustify 
     (const Standard_Integer Width, const Standard_Character Filler)
{
   if (Width < 0) Standard_NegativeValue::Raise();
   myString.LeftJustify(Width,Filler);
}

//------------------------------------------------------------------------
//  Location
//------------------------------------------------------------------------
Standard_Integer TCollection_HAsciiString::Location 
   (const Standard_Integer N, const Standard_Character C, 
    const Standard_Integer FromIndex, const Standard_Integer ToIndex) const
{
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                      Standard_OutOfRange::Raise();
   return myString.Location(N,C,FromIndex,ToIndex);
}

//------------------------------------------------------------------------
//  Location
//------------------------------------------------------------------------
Standard_Integer TCollection_HAsciiString::Location 
        (const Handle(TCollection_HAsciiString)& S, const Standard_Integer FromIndex, 
         const Standard_Integer ToIndex) const
{
   if (Length() == 0 || S->Length() == 0) return 0;
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                      Standard_OutOfRange::Raise();
   return myString.Location(S->String(),FromIndex,ToIndex);
}

// ----------------------------------------------------------------------------
// LowerCase
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::LowerCase()
{
    myString.LowerCase();
}

//------------------------------------------------------------------------
//  Prepend
//------------------------------------------------------------------------
void TCollection_HAsciiString::Prepend 
                    (const Handle(TCollection_HAsciiString)& S)
{
   myString.Prepend(S->String());
}

//---------------------------------------------------------------------
// Print
//---------------------------------------------------------------------
void TCollection_HAsciiString::Print(Standard_OStream& S) const
{
  myString.Print(S);
}

// ----------------------------------------------------------------------------
// RealValue
// ----------------------------------------------------------------------------
Standard_Real TCollection_HAsciiString::RealValue() const
{
   return myString.RealValue();
}

// ----------------------------------------------------------------------------
// RemoveAll
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::RemoveAll
                              (const Standard_Character what,
                               const Standard_Boolean CaseSensitive)
{
   myString.RemoveAll(what,CaseSensitive);
}

// ----------------------------------------------------------------------------
// RemoveAll
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::RemoveAll(const Standard_Character what)
{
   myString.RemoveAll(what);
}

// ----------------------------------------------------------------------------
// Remove
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::Remove (const Standard_Integer where,
                                  const Standard_Integer ahowmany)
{
   myString.Remove(where,ahowmany);
}

//------------------------------------------------------------------------
//  RightAdjust
//------------------------------------------------------------------------
void TCollection_HAsciiString::RightAdjust ()
{
   myString.RightAdjust();
}

//------------------------------------------------------------------------
//  RightJustify
//------------------------------------------------------------------------
void TCollection_HAsciiString::RightJustify 
           (const Standard_Integer Width, const Standard_Character Filler)
{
   if (Width < 0) Standard_NegativeValue::Raise();
   myString.RightJustify(Width,Filler);
}

// ----------------------------------------------------------------------------
// Search
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HAsciiString::Search(const Standard_CString what)const
{
   return  myString.Search(what);
}

// ----------------------------------------------------------------------------
// Search
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HAsciiString::Search
                       (const Handle(TCollection_HAsciiString)& what) const
{
   return  myString.Search(what->String());
}

// ----------------------------------------------------------------------------
// SearchFromEnd
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HAsciiString::SearchFromEnd(const Standard_CString what)const
{
   return  myString.SearchFromEnd(what);
}

// ----------------------------------------------------------------------------
// SearchFromEnd
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HAsciiString::SearchFromEnd
                       (const Handle(TCollection_HAsciiString)& what) const
{
   return  myString.SearchFromEnd(what->String());
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::SetValue(const Standard_Integer where,
                                        const Standard_Character what)
{
   myString.SetValue(where,what);
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::SetValue(const Standard_Integer where,const Standard_CString what)
{
   myString.SetValue(where,what);
}

// ----------------------------------------------------------------------------
// SetValue
// ---------------------------------------------------------------------------
void TCollection_HAsciiString::SetValue(const Standard_Integer where,
                                 const Handle(TCollection_HAsciiString)& what)
{
   myString.SetValue(where, what->String());
}

// ----------------------------------------------------------------------------
// Split
// ----------------------------------------------------------------------------
Handle(TCollection_HAsciiString) 
         TCollection_HAsciiString::Split(const Standard_Integer where)
{
  return new TCollection_HAsciiString(myString.Split(where));
}

// ----------------------------------------------------------------------------
// SubString
// ----------------------------------------------------------------------------
Handle(TCollection_HAsciiString) 
         TCollection_HAsciiString::SubString(const Standard_Integer FromIndex,
                                             const Standard_Integer ToIndex) const
{
  return new TCollection_HAsciiString(myString.SubString(FromIndex,ToIndex));
}


// ----------------------------------------------------------------------------
// Token
// ----------------------------------------------------------------------------
Handle(TCollection_HAsciiString) TCollection_HAsciiString::Token
         (const Standard_CString separators,const Standard_Integer whichone) const
{
  return new TCollection_HAsciiString(myString.Token(separators,whichone));
}

// ----------------------------------------------------------------------------
// Trunc
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::Trunc(const Standard_Integer ahowmany)
{
   myString.Trunc(ahowmany);
}

// ----------------------------------------------------------------------------
// UpperCase
// ----------------------------------------------------------------------------
void TCollection_HAsciiString::UpperCase()
{
   myString.UpperCase();
}

// ----------------------------------------------------------------------------
// UsefullLength
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HAsciiString::UsefullLength() const
{
   return myString.UsefullLength();
}

// ----------------------------------------------------------------------------
// Value
// ----------------------------------------------------------------------------
Standard_Character TCollection_HAsciiString::Value(const Standard_Integer where) const
{
   return myString.Value(where);
}

// ----------------------------------------------------------------------------
// ShallowCopy
// ----------------------------------------------------------------------------
Handle(TCollection_HAsciiString) TCollection_HAsciiString::ShallowCopy() const
{
//   Handle(TCollection_HAsciiString) thecopy = new TCollection_HAsciiString;
//   for (Standard_Integer i = 1 ; i <= Length() ; i++) 
//          thecopy->Insert(i,Value(i));   
//   return thecopy;
   return new TCollection_HAsciiString(*this);
}

//---------------------------------------------------------------------
// ShallowDump
//---------------------------------------------------------------------
void TCollection_HAsciiString::ShallowDump(Standard_OStream& S) const
{
  S << "begin class HAsciiString "<<endl;
  myString.Print(S);
}

// ----------------------------------------------------------------------------
// IsSameState
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HAsciiString::IsSameState
   (const Handle(TCollection_HAsciiString)& other) const
 {

#if OptJr
  if ( myString.Length() == other->Length() ) {
    Standard_Boolean KEqual ;
    ASCIISTRINGEQUAL( myString.ToCString() , other->ToCString() ,
                      myString.mylength , KEqual ) ;
    return KEqual ;
  }
  else
    return Standard_False ;
#else
   return ( !strcmp(myString.mystring , other->ToCString() ));
#endif
 }

