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

#include <Standard_NegativeValue.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TCollection_HAsciiString, Standard_Transient)

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString()
    : myString()
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const Standard_CString message)
    : myString(message)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const TCollection_AsciiString& astring)
    : myString(astring)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const Standard_Character aChar)
    : myString(aChar)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const Standard_Integer   length,
                                                   const Standard_Character filler)
    : myString(length, filler)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const Standard_Integer aValue)
    : myString(aValue)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const Standard_Real aValue)
    : myString(aValue)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const Handle(TCollection_HAsciiString)& astring)
    : myString(astring->String())
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(
  const Handle(TCollection_HExtendedString)& astring,
  const Standard_Character                   replaceNonAscii)
    : myString(astring->String(), replaceNonAscii)
{
}

//=================================================================================================

void TCollection_HAsciiString::Capitalize()
{
  myString.Capitalize();
}

//=================================================================================================

Handle(TCollection_HAsciiString) TCollection_HAsciiString::Cat(const Standard_CString other) const
{
  return new TCollection_HAsciiString(myString.Cat(other));
}

//=================================================================================================

Handle(TCollection_HAsciiString) TCollection_HAsciiString::Cat(
  const Handle(TCollection_HAsciiString)& other) const
{
  return new TCollection_HAsciiString(myString.Cat(other->String()));
}

//=================================================================================================

void TCollection_HAsciiString::Center(const Standard_Integer Width, const Standard_Character Filler)
{
  if (Width < 0)
    throw Standard_NegativeValue();
  myString.Center(Width, Filler);
}

//=================================================================================================

void TCollection_HAsciiString::ChangeAll(const Standard_Character aChar,
                                         const Standard_Character NewChar,
                                         const Standard_Boolean   CaseSensitive)
{
  myString.ChangeAll(aChar, NewChar, CaseSensitive);
}

//=================================================================================================

void TCollection_HAsciiString::Clear()
{
  myString.Clear();
}

//=================================================================================================

Standard_Integer TCollection_HAsciiString::FirstLocationInSet(
  const Handle(TCollection_HAsciiString)& Set,
  const Standard_Integer                  FromIndex,
  const Standard_Integer                  ToIndex) const
{
  if (Length() == 0 || Set->Length() == 0)
    return 0;
  if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex)
    throw Standard_OutOfRange();
  return (myString.FirstLocationInSet(Set->String(), FromIndex, ToIndex));
}

//=================================================================================================

Standard_Integer TCollection_HAsciiString::FirstLocationNotInSet(
  const Handle(TCollection_HAsciiString)& Set,
  const Standard_Integer                  FromIndex,
  const Standard_Integer                  ToIndex) const
{
  if (Length() == 0 || Set->Length() == 0)
    return 0;
  if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex)
    throw Standard_OutOfRange();
  return (myString.FirstLocationNotInSet(Set->String(), FromIndex, ToIndex));
}

//=================================================================================================

void TCollection_HAsciiString::Insert(const Standard_Integer where, const Standard_Character what)
{
  myString.Insert(where, what);
}

//=================================================================================================

void TCollection_HAsciiString::Insert(const Standard_Integer where, const Standard_CString what)
{
  myString.Insert(where, what);
}

//=================================================================================================

void TCollection_HAsciiString::Insert(const Standard_Integer                  where,
                                      const Handle(TCollection_HAsciiString)& what)
{
  myString.Insert(where, what->String());
}

//=================================================================================================

void TCollection_HAsciiString::InsertAfter(const Standard_Integer                  Index,
                                           const Handle(TCollection_HAsciiString)& S)
{
  Standard_Integer size1 = Length();
#ifndef NOBOUNDCHECK
  if (Index < 0 || Index > size1)
    throw Standard_OutOfRange();
#endif
  myString.InsertAfter(Index, S->String());
}

//=================================================================================================

void TCollection_HAsciiString::InsertBefore(const Standard_Integer                  Index,
                                            const Handle(TCollection_HAsciiString)& S)
{
  Standard_Integer size1 = Length();
#ifndef NOBOUNDCHECK
  if (Index < 1 || Index > size1)
    throw Standard_OutOfRange();
#endif
  myString.InsertBefore(Index, S->String());
}

//=================================================================================================

Standard_Boolean TCollection_HAsciiString::IsEmpty() const
{
  return (myString.Length() == 0);
}

//=================================================================================================

Standard_Boolean TCollection_HAsciiString::IsLess(
  const Handle(TCollection_HAsciiString)& other) const
{
  return myString.IsLess(other->String());
}

//=================================================================================================

Standard_Boolean TCollection_HAsciiString::IsGreater(
  const Handle(TCollection_HAsciiString)& other) const
{
  return myString.IsGreater(other->String());
}

//=================================================================================================

Standard_Integer TCollection_HAsciiString::IntegerValue() const
{
  return myString.IntegerValue();
}

//=================================================================================================

Standard_Boolean TCollection_HAsciiString::IsIntegerValue() const
{
  return myString.IsIntegerValue();
}

//=================================================================================================

Standard_Boolean TCollection_HAsciiString::IsRealValue() const
{
  return myString.IsRealValue();
}

//=================================================================================================

Standard_Boolean TCollection_HAsciiString::IsAscii() const
{
  return myString.IsAscii();
}

//=================================================================================================

Standard_Boolean TCollection_HAsciiString::IsDifferent(
  const Handle(TCollection_HAsciiString)& S) const
{
  if (S.IsNull())
    throw Standard_NullObject("TCollection_HAsciiString::IsDifferent");
  if (S->Length() != myString.Length())
    return Standard_True;
  return (strncmp(myString.ToCString(), S->ToCString(), myString.Length()) != 0);
}

//=================================================================================================

Standard_Boolean TCollection_HAsciiString::IsSameString(
  const Handle(TCollection_HAsciiString)& S) const
{
  if (S.IsNull())
    throw Standard_NullObject("TCollection_HAsciiString::IsSameString");
  if (myString.Length() == S->Length())
    return (strncmp(myString.ToCString(), S->ToCString(), myString.Length()) == 0);
  else
    return Standard_False;
}

//=================================================================================================

Standard_Boolean TCollection_HAsciiString::IsSameString(const Handle(TCollection_HAsciiString)& S,
                                                        const Standard_Boolean CaseSensitive) const
{
  if (S.IsNull())
    throw Standard_NullObject("TCollection_HAsciiString::IsSameString");
  return TCollection_AsciiString::IsSameString(myString, S->myString, CaseSensitive);
}

//=================================================================================================

void TCollection_HAsciiString::LeftAdjust()
{
  myString.LeftAdjust();
}

//=================================================================================================

void TCollection_HAsciiString::LeftJustify(const Standard_Integer   Width,
                                           const Standard_Character Filler)
{
  if (Width < 0)
    throw Standard_NegativeValue();
  myString.LeftJustify(Width, Filler);
}

//=================================================================================================

Standard_Integer TCollection_HAsciiString::Location(const Standard_Integer   N,
                                                    const Standard_Character C,
                                                    const Standard_Integer   FromIndex,
                                                    const Standard_Integer   ToIndex) const
{
  if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex)
    throw Standard_OutOfRange();
  return myString.Location(N, C, FromIndex, ToIndex);
}

//=================================================================================================

Standard_Integer TCollection_HAsciiString::Location(const Handle(TCollection_HAsciiString)& S,
                                                    const Standard_Integer FromIndex,
                                                    const Standard_Integer ToIndex) const
{
  if (Length() == 0 || S->Length() == 0)
    return 0;
  if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex)
    throw Standard_OutOfRange();
  return myString.Location(S->String(), FromIndex, ToIndex);
}

//=================================================================================================

void TCollection_HAsciiString::LowerCase()
{
  myString.LowerCase();
}

//=================================================================================================

void TCollection_HAsciiString::Prepend(const Handle(TCollection_HAsciiString)& S)
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

//=================================================================================================

Standard_Real TCollection_HAsciiString::RealValue() const
{
  return myString.RealValue();
}

//=================================================================================================

void TCollection_HAsciiString::RemoveAll(const Standard_Character what,
                                         const Standard_Boolean   CaseSensitive)
{
  myString.RemoveAll(what, CaseSensitive);
}

//=================================================================================================

void TCollection_HAsciiString::RemoveAll(const Standard_Character what)
{
  myString.RemoveAll(what);
}

//=================================================================================================

void TCollection_HAsciiString::Remove(const Standard_Integer where, const Standard_Integer ahowmany)
{
  myString.Remove(where, ahowmany);
}

//=================================================================================================

void TCollection_HAsciiString::RightAdjust()
{
  myString.RightAdjust();
}

//=================================================================================================

void TCollection_HAsciiString::RightJustify(const Standard_Integer   Width,
                                            const Standard_Character Filler)
{
  if (Width < 0)
    throw Standard_NegativeValue();
  myString.RightJustify(Width, Filler);
}

//=================================================================================================

Standard_Integer TCollection_HAsciiString::Search(const Standard_CString what) const
{
  return myString.Search(what);
}

//=================================================================================================

Standard_Integer TCollection_HAsciiString::Search(
  const Handle(TCollection_HAsciiString)& what) const
{
  return myString.Search(what->String());
}

//=================================================================================================

Standard_Integer TCollection_HAsciiString::SearchFromEnd(const Standard_CString what) const
{
  return myString.SearchFromEnd(what);
}

//=================================================================================================

Standard_Integer TCollection_HAsciiString::SearchFromEnd(
  const Handle(TCollection_HAsciiString)& what) const
{
  return myString.SearchFromEnd(what->String());
}

//=================================================================================================

void TCollection_HAsciiString::SetValue(const Standard_Integer where, const Standard_Character what)
{
  myString.SetValue(where, what);
}

//=================================================================================================

void TCollection_HAsciiString::SetValue(const Standard_Integer where, const Standard_CString what)
{
  myString.SetValue(where, what);
}

//=================================================================================================

void TCollection_HAsciiString::SetValue(const Standard_Integer                  where,
                                        const Handle(TCollection_HAsciiString)& what)
{
  myString.SetValue(where, what->String());
}

//=================================================================================================

Handle(TCollection_HAsciiString) TCollection_HAsciiString::Split(const Standard_Integer where)
{
  return new TCollection_HAsciiString(myString.Split(where));
}

//=================================================================================================

Handle(TCollection_HAsciiString) TCollection_HAsciiString::SubString(
  const Standard_Integer FromIndex,
  const Standard_Integer ToIndex) const
{
  return new TCollection_HAsciiString(myString.SubString(FromIndex, ToIndex));
}

//=================================================================================================

Handle(TCollection_HAsciiString) TCollection_HAsciiString::Token(
  const Standard_CString separators,
  const Standard_Integer whichone) const
{
  return new TCollection_HAsciiString(myString.Token(separators, whichone));
}

//=================================================================================================

void TCollection_HAsciiString::Trunc(const Standard_Integer ahowmany)
{
  myString.Trunc(ahowmany);
}

//=================================================================================================

void TCollection_HAsciiString::UpperCase()
{
  myString.UpperCase();
}

//=================================================================================================

Standard_Integer TCollection_HAsciiString::UsefullLength() const
{
  return myString.UsefullLength();
}

//=================================================================================================

Standard_Character TCollection_HAsciiString::Value(const Standard_Integer where) const
{
  return myString.Value(where);
}

//=================================================================================================

Standard_Boolean TCollection_HAsciiString::IsSameState(
  const Handle(TCollection_HAsciiString)& other) const
{
  if (myString.Length() == other->Length())
    return (strncmp(myString.ToCString(), other->ToCString(), myString.Length()) == 0);
  else
    return Standard_False;
}
