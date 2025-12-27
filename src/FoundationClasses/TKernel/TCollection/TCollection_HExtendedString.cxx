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

TCollection_HExtendedString::TCollection_HExtendedString(const char* message)
    : myString(message)
{
}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(const char16_t* message)
    : myString(message)
{
}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(const char16_t aChar)
    : myString(aChar)
{
}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(const int      length,
                                                         const char16_t filler)
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
  const occ::handle<TCollection_HAsciiString>& astring)
    : myString(astring->String())
{
}

//=================================================================================================

TCollection_HExtendedString::TCollection_HExtendedString(
  const occ::handle<TCollection_HExtendedString>& astring)
    : myString(astring->ChangeString())
{
}

//=================================================================================================

void TCollection_HExtendedString::AssignCat(const occ::handle<TCollection_HExtendedString>& other)
{
  myString.AssignCat(other->ChangeString());
}

//=================================================================================================

occ::handle<TCollection_HExtendedString> TCollection_HExtendedString::Cat(
  const occ::handle<TCollection_HExtendedString>& other) const
{
  return new TCollection_HExtendedString(myString.Cat(other->ChangeString()));
}

//=================================================================================================

void TCollection_HExtendedString::ChangeAll(const char16_t aChar,
                                            const char16_t NewChar)
{
  myString.ChangeAll(aChar, NewChar);
}

//=================================================================================================

bool TCollection_HExtendedString::IsEmpty() const
{
  return (myString.Length() == 0);
}

//=================================================================================================

void TCollection_HExtendedString::Clear()
{
  myString.Clear();
}

//=================================================================================================

void TCollection_HExtendedString::Insert(const int      where,
                                         const char16_t what)
{
  myString.Insert(where, what);
}

//=================================================================================================

void TCollection_HExtendedString::Insert(const int                     where,
                                         const occ::handle<TCollection_HExtendedString>& what)
{
  myString.Insert(where, what->ChangeString());
}

//=================================================================================================

bool TCollection_HExtendedString::IsLess(
  const occ::handle<TCollection_HExtendedString>& other) const
{
  return myString.IsLess(other->ChangeString());
}

//=================================================================================================

bool TCollection_HExtendedString::IsGreater(
  const occ::handle<TCollection_HExtendedString>& other) const
{
  return myString.IsGreater(other->ChangeString());
}

//=================================================================================================

bool TCollection_HExtendedString::IsAscii() const
{
  return myString.IsAscii();
}

//=================================================================================================

int TCollection_HExtendedString::Length() const
{
  return myString.Length();
}

//=================================================================================================

void TCollection_HExtendedString::Remove(const int where,
                                         const int ahowmany)
{
  myString.Remove(where, ahowmany);
}

//=================================================================================================

void TCollection_HExtendedString::RemoveAll(const char16_t what)
{
  myString.RemoveAll(what);
}

//=================================================================================================

void TCollection_HExtendedString::SetValue(const int      where,
                                           const char16_t what)
{
  myString.SetValue(where, what);
}

//=================================================================================================

void TCollection_HExtendedString::SetValue(const int                     where,
                                           const occ::handle<TCollection_HExtendedString>& what)
{
  myString.SetValue(where, what->ChangeString());
}

//=================================================================================================

occ::handle<TCollection_HExtendedString> TCollection_HExtendedString::Split(const int where)
{
  return new TCollection_HExtendedString(myString.Split(where));
}

//=================================================================================================

int TCollection_HExtendedString::Search(
  const occ::handle<TCollection_HExtendedString>& what) const
{
  return myString.Search(what->ChangeString());
}

//=================================================================================================

int TCollection_HExtendedString::SearchFromEnd(
  const occ::handle<TCollection_HExtendedString>& what) const
{
  return myString.SearchFromEnd(what->ChangeString());
}

//=================================================================================================

occ::handle<TCollection_HExtendedString> TCollection_HExtendedString::Token(
  const char16_t* separators,
  const int   whichone) const
{
  return new TCollection_HExtendedString(myString.Token(separators, whichone));
}

//=================================================================================================

void TCollection_HExtendedString::Trunc(const int ahowmany)
{
  myString.Trunc(ahowmany);
}

//=================================================================================================

char16_t TCollection_HExtendedString::Value(const int where) const
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

bool TCollection_HExtendedString::IsSameState(
  const occ::handle<TCollection_HExtendedString>& other) const
{
  return myString == other->String();
}

TCollection_ExtendedString& TCollection_HExtendedString::ChangeString() const
{
  return (TCollection_ExtendedString&)myString;
}
