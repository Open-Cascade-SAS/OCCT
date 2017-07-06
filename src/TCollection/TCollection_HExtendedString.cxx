// Copyright (c) 1992-1999 Matra Datavision
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

//Modified: C. LEYNADIER Nov,21 1997 (Token et ChangeString)

#include <Standard_ExtCharacter.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TCollection_HExtendedString,Standard_Transient)

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString(){}


// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const Standard_CString message):myString(message)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const Standard_ExtString message):myString(message)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const Standard_ExtCharacter aChar):myString(aChar)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
          (const Standard_Integer length,const Standard_ExtCharacter filler )
          :myString(length,filler)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const TCollection_ExtendedString& astring):myString(astring)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const Handle(TCollection_HAsciiString)& astring)
        :myString(astring->String())
{
}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const Handle(TCollection_HExtendedString)& astring)
        :myString(astring->ChangeString())
{
}

// ---------------------------------------------------------------------------
// AssignCat
// ----------------------------------------------------------------------------
void  TCollection_HExtendedString::AssignCat
     (const Handle(TCollection_HExtendedString)& other) 
{
   myString.AssignCat(other->ChangeString());
}

// ---------------------------------------------------------------------------
// Cat
// ----------------------------------------------------------------------------
Handle(TCollection_HExtendedString)  TCollection_HExtendedString::Cat
     (const Handle(TCollection_HExtendedString)& other)  const
{
   return new TCollection_HExtendedString(myString.Cat(other->ChangeString() ) );
}

// ----------------------------------------------------------------------------
// ChangeAll
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::ChangeAll
                       (const Standard_ExtCharacter aChar,
                        const Standard_ExtCharacter NewChar)
{
   myString.ChangeAll(aChar,NewChar);
}

// ----------------------------------------------------------------------------
// IsEmpty
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HExtendedString::IsEmpty() const
{
   return (myString.Length() == 0);
}

// ----------------------------------------------------------------------------
// Clear
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::Clear()
{
   myString.Clear();
}

// ----------------------------------------------------------------------------
// Insert a Standard_ExtCharacter before 'where'th Standard_ExtCharacter
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::Insert(const Standard_Integer where,
                                 const Standard_ExtCharacter what)
{
   myString.Insert(where,what);
}

// ----------------------------------------------------------------------------
// Insert
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::Insert(const Standard_Integer where,
                          const Handle(TCollection_HExtendedString)& what)
{
   myString.Insert(where,what->ChangeString());
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HExtendedString::IsLess(
        const Handle(TCollection_HExtendedString)& other) const
{
   return myString.IsLess(other->ChangeString());
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HExtendedString::IsGreater
    (const Handle(TCollection_HExtendedString)& other) const
{
   return myString.IsGreater(other->ChangeString());
}

// ----------------------------------------------------------------------------
// IsAscii
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HExtendedString::IsAscii() const 
{
   return myString.IsAscii();
}

// ----------------------------------------------------------------------------
// Length
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HExtendedString::Length() const
{
   return myString.Length();
}

// ----------------------------------------------------------------------------
// Remove
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::Remove (const Standard_Integer where,
                                  const Standard_Integer ahowmany)
{
   myString.Remove(where,ahowmany);
}

// ----------------------------------------------------------------------------
// RemoveAll
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::RemoveAll(const Standard_ExtCharacter what)
{
   myString.RemoveAll(what);
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::SetValue(
           const Standard_Integer where,const Standard_ExtCharacter what)
{
   myString.SetValue(where,what);
}

// ----------------------------------------------------------------------------
// SetValue
// ---------------------------------------------------------------------------
void TCollection_HExtendedString::SetValue(const Standard_Integer where,
                       const Handle(TCollection_HExtendedString)& what)
{
   myString.SetValue(where, what->ChangeString());
}

// ----------------------------------------------------------------------------
// Split
// ----------------------------------------------------------------------------
Handle(TCollection_HExtendedString) TCollection_HExtendedString::Split
              (const Standard_Integer where)
{
   return new TCollection_HExtendedString(myString.Split(where));
}

// ----------------------------------------------------------------------------
// Search
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HExtendedString::Search
                       (const Handle(TCollection_HExtendedString)& what) const
{
   return  myString.Search(what->ChangeString());
}

// ----------------------------------------------------------------------------
// SearchFromEnd
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HExtendedString::SearchFromEnd
                       (const Handle(TCollection_HExtendedString)& what) const
{
   return  myString.SearchFromEnd(what->ChangeString());
}

// ----------------------------------------------------------------------------
// Token
// ----------------------------------------------------------------------------
Handle(TCollection_HExtendedString) TCollection_HExtendedString::Token
         (const Standard_ExtString separators,const Standard_Integer whichone) const
{
   return new TCollection_HExtendedString(myString.Token(separators,whichone));
}

// ----------------------------------------------------------------------------
// Trunc
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::Trunc(const Standard_Integer ahowmany)
{
   myString.Trunc(ahowmany);
}

// ----------------------------------------------------------------------------
// Value
// ----------------------------------------------------------------------------
Standard_ExtCharacter TCollection_HExtendedString::Value
             (const Standard_Integer where) const
{
   return myString.Value(where);
}

// ----------------------------------------------------------------------------
// String
// ----------------------------------------------------------------------------
const TCollection_ExtendedString& TCollection_HExtendedString::String() const
{
  return myString;
}


//---------------------------------------------------------------------
// Print
//---------------------------------------------------------------------
void TCollection_HExtendedString::Print(Standard_OStream& S) const
{
  S << "begin class HExtendedString "<<endl;
  myString.Print(S);
}

// ----------------------------------------------------------------------------
// Issamestate
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HExtendedString::IsSameState
   (const Handle(TCollection_HExtendedString)& other) const
 {
   return myString == other->String();
 }


TCollection_ExtendedString& TCollection_HExtendedString::ChangeString() const
{
  return (TCollection_ExtendedString&)myString;
}
