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

// Modified: C. LEYNADIER Nov,21 1997 (Token et ChangeString)

#include <Standard_NegativeValue.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TCollection_HExtendedString, Standard_Transient)

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString() {}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(const Standard_CString message)
    : myString(message)
{
}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(const Standard_ExtString message)
    : myString(message)
{
}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(const Standard_ExtCharacter aChar)
    : myString(aChar)
{
}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(const Standard_Integer      length,
                                                         const Standard_ExtCharacter filler)
    : myString(length, filler)
{
}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(const TCollection_ExtendedString& astring)
    : myString(astring)
{
}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(
  const Handle(TCollection_HAsciiString)& astring)
    : myString(astring->String())
{
}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(
  const Handle(TCollection_HExtendedString)& astring)
    : myString(astring->ChangeString())
{
}

//=================================================================================================

void TCollection_HExtendedString::AssignCat(const Handle(TCollection_HExtendedString)& other)
{
  myString.AssignCat(other->ChangeString());
}

//=================================================================================================

Handle(TCollection_HExtendedString) TCollection_HExtendedString::Cat(
  const Handle(TCollection_HExtendedString)& other) const
{
  return new TCollection_HExtendedString(myString.Cat(other->ChangeString()));
}

//=================================================================================================

void TCollection_HExtendedString::ChangeAll(const Standard_ExtCharacter aChar,
                                            const Standard_ExtCharacter NewChar)
{
  myString.ChangeAll(aChar, NewChar);
}

//=================================================================================================

Standard_Boolean TCollection_HExtendedString::IsEmpty() const
{
  return (myString.Length() == 0);
}

//=================================================================================================

void TCollection_HExtendedString::Clear()
{
  myString.Clear();
}

//=================================================================================================

void TCollection_HExtendedString::Insert(const Standard_Integer      where,
                                         const Standard_ExtCharacter what)
{
  myString.Insert(where, what);
}

//=================================================================================================

void TCollection_HExtendedString::Insert(const Standard_Integer                     where,
                                         const Handle(TCollection_HExtendedString)& what)
{
  myString.Insert(where, what->ChangeString());
}

//=================================================================================================

Standard_Boolean TCollection_HExtendedString::IsLess(
  const Handle(TCollection_HExtendedString)& other) const
{
  return myString.IsLess(other->ChangeString());
}

//=================================================================================================

Standard_Boolean TCollection_HExtendedString::IsGreater(
  const Handle(TCollection_HExtendedString)& other) const
{
  return myString.IsGreater(other->ChangeString());
}

//=================================================================================================

Standard_Boolean TCollection_HExtendedString::IsAscii() const
{
  return myString.IsAscii();
}

//=================================================================================================

Standard_Integer TCollection_HExtendedString::Length() const
{
  return myString.Length();
}

//=================================================================================================

void TCollection_HExtendedString::Remove(const Standard_Integer where,
                                         const Standard_Integer ahowmany)
{
  myString.Remove(where, ahowmany);
}

//=================================================================================================

void TCollection_HExtendedString::RemoveAll(const Standard_ExtCharacter what)
{
  myString.RemoveAll(what);
}

//=================================================================================================

void TCollection_HExtendedString::SetValue(const Standard_Integer      where,
                                           const Standard_ExtCharacter what)
{
  myString.SetValue(where, what);
}

//=================================================================================================

void TCollection_HExtendedString::SetValue(const Standard_Integer                     where,
                                           const Handle(TCollection_HExtendedString)& what)
{
  myString.SetValue(where, what->ChangeString());
}

//=================================================================================================

Handle(TCollection_HExtendedString) TCollection_HExtendedString::Split(const Standard_Integer where)
{
  return new TCollection_HExtendedString(myString.Split(where));
}

//=================================================================================================

Standard_Integer TCollection_HExtendedString::Search(
  const Handle(TCollection_HExtendedString)& what) const
{
  return myString.Search(what->ChangeString());
}

//=================================================================================================

Standard_Integer TCollection_HExtendedString::SearchFromEnd(
  const Handle(TCollection_HExtendedString)& what) const
{
  return myString.SearchFromEnd(what->ChangeString());
}

//=================================================================================================

Handle(TCollection_HExtendedString) TCollection_HExtendedString::Token(
  const Standard_ExtString separators,
  const Standard_Integer   whichone) const
{
  return new TCollection_HExtendedString(myString.Token(separators, whichone));
}

//=================================================================================================

void TCollection_HExtendedString::Trunc(const Standard_Integer ahowmany)
{
  myString.Trunc(ahowmany);
}

//=================================================================================================

Standard_ExtCharacter TCollection_HExtendedString::Value(const Standard_Integer where) const
{
  return myString.Value(where);
}

//=================================================================================================

const TCollection_ExtendedString& TCollection_HExtendedString::String() const
{
  return myString;
}

//---------------------------------------------------------------------
// Print
//---------------------------------------------------------------------
void TCollection_HExtendedString::Print(Standard_OStream& S) const
{
  S << "begin class HExtendedString " << std::endl;
  myString.Print(S);
}

//=================================================================================================

Standard_Boolean TCollection_HExtendedString::IsSameState(
  const Handle(TCollection_HExtendedString)& other) const
{
  return myString == other->String();
}

TCollection_ExtendedString& TCollection_HExtendedString::ChangeString() const
{
  return (TCollection_ExtendedString&)myString;
}
