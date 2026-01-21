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

  = default;

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const char* const message)
    : myString(message)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const TCollection_AsciiString& astring)
    : myString(astring)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const char aChar)
    : myString(aChar)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const int length, const char filler)
    : myString(length, filler)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const int aValue)
    : myString(aValue)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(const double aValue)
    : myString(aValue)
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(
  const occ::handle<TCollection_HAsciiString>& astring)
    : myString(astring->String())
{
}

//=================================================================================================

TCollection_HAsciiString::TCollection_HAsciiString(
  const occ::handle<TCollection_HExtendedString>& astring,
  const char                                      replaceNonAscii)
    : myString(astring->String(), replaceNonAscii)
{
}

//=================================================================================================

void TCollection_HAsciiString::Capitalize()
{
  myString.Capitalize();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> TCollection_HAsciiString::Cat(const char* const other) const
{
  return new TCollection_HAsciiString(myString.Cat(other));
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> TCollection_HAsciiString::Cat(
  const occ::handle<TCollection_HAsciiString>& other) const
{
  return new TCollection_HAsciiString(myString.Cat(other->String()));
}

//=================================================================================================

void TCollection_HAsciiString::Center(const int Width, const char Filler)
{
  if (Width < 0)
    throw Standard_NegativeValue();
  myString.Center(Width, Filler);
}

//=================================================================================================

void TCollection_HAsciiString::ChangeAll(const char aChar,
                                         const char NewChar,
                                         const bool CaseSensitive)
{
  myString.ChangeAll(aChar, NewChar, CaseSensitive);
}

//=================================================================================================

void TCollection_HAsciiString::Clear()
{
  myString.Clear();
}

//=================================================================================================

int TCollection_HAsciiString::FirstLocationInSet(const occ::handle<TCollection_HAsciiString>& Set,
                                                 const int FromIndex,
                                                 const int ToIndex) const
{
  if (Length() == 0 || Set->Length() == 0)
    return 0;
  if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex)
    throw Standard_OutOfRange();
  return (myString.FirstLocationInSet(Set->String(), FromIndex, ToIndex));
}

//=================================================================================================

int TCollection_HAsciiString::FirstLocationNotInSet(
  const occ::handle<TCollection_HAsciiString>& Set,
  const int                                    FromIndex,
  const int                                    ToIndex) const
{
  if (Length() == 0 || Set->Length() == 0)
    return 0;
  if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex)
    throw Standard_OutOfRange();
  return (myString.FirstLocationNotInSet(Set->String(), FromIndex, ToIndex));
}

//=================================================================================================

void TCollection_HAsciiString::Insert(const int where, const char what)
{
  myString.Insert(where, what);
}

//=================================================================================================

void TCollection_HAsciiString::Insert(const int where, const char* const what)
{
  myString.Insert(where, what);
}

//=================================================================================================

void TCollection_HAsciiString::Insert(const int                                    where,
                                      const occ::handle<TCollection_HAsciiString>& what)
{
  myString.Insert(where, what->String());
}

//=================================================================================================

void TCollection_HAsciiString::InsertAfter(const int                                    Index,
                                           const occ::handle<TCollection_HAsciiString>& S)
{
  int size1 = Length();
#ifndef NOBOUNDCHECK
  if (Index < 0 || Index > size1)
    throw Standard_OutOfRange();
#endif
  myString.InsertAfter(Index, S->String());
}

//=================================================================================================

void TCollection_HAsciiString::InsertBefore(const int                                    Index,
                                            const occ::handle<TCollection_HAsciiString>& S)
{
  int size1 = Length();
#ifndef NOBOUNDCHECK
  if (Index < 1 || Index > size1)
    throw Standard_OutOfRange();
#endif
  myString.InsertBefore(Index, S->String());
}

//=================================================================================================

bool TCollection_HAsciiString::IsEmpty() const
{
  return (myString.Length() == 0);
}

//=================================================================================================

bool TCollection_HAsciiString::IsLess(const occ::handle<TCollection_HAsciiString>& other) const
{
  return myString.IsLess(other->String());
}

//=================================================================================================

bool TCollection_HAsciiString::IsGreater(const occ::handle<TCollection_HAsciiString>& other) const
{
  return myString.IsGreater(other->String());
}

//=================================================================================================

int TCollection_HAsciiString::IntegerValue() const
{
  return myString.IntegerValue();
}

//=================================================================================================

bool TCollection_HAsciiString::IsIntegerValue() const
{
  return myString.IsIntegerValue();
}

//=================================================================================================

bool TCollection_HAsciiString::IsRealValue() const
{
  return myString.IsRealValue();
}

//=================================================================================================

bool TCollection_HAsciiString::IsAscii() const
{
  return myString.IsAscii();
}

//=================================================================================================

bool TCollection_HAsciiString::IsDifferent(const occ::handle<TCollection_HAsciiString>& S) const
{
  if (S.IsNull())
    throw Standard_NullObject("TCollection_HAsciiString::IsDifferent");
  if (S->Length() != myString.Length())
    return true;
  return (strncmp(myString.ToCString(), S->ToCString(), myString.Length()) != 0);
}

//=================================================================================================

bool TCollection_HAsciiString::IsSameString(const occ::handle<TCollection_HAsciiString>& S) const
{
  if (S.IsNull())
    throw Standard_NullObject("TCollection_HAsciiString::IsSameString");
  if (myString.Length() == S->Length())
    return (strncmp(myString.ToCString(), S->ToCString(), myString.Length()) == 0);
  else
    return false;
}

//=================================================================================================

bool TCollection_HAsciiString::IsSameString(const occ::handle<TCollection_HAsciiString>& S,
                                            const bool CaseSensitive) const
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

void TCollection_HAsciiString::LeftJustify(const int Width, const char Filler)
{
  if (Width < 0)
    throw Standard_NegativeValue();
  myString.LeftJustify(Width, Filler);
}

//=================================================================================================

int TCollection_HAsciiString::Location(const int  N,
                                       const char C,
                                       const int  FromIndex,
                                       const int  ToIndex) const
{
  if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex)
    throw Standard_OutOfRange();
  return myString.Location(N, C, FromIndex, ToIndex);
}

//=================================================================================================

int TCollection_HAsciiString::Location(const occ::handle<TCollection_HAsciiString>& S,
                                       const int                                    FromIndex,
                                       const int                                    ToIndex) const
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

void TCollection_HAsciiString::Prepend(const occ::handle<TCollection_HAsciiString>& S)
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

double TCollection_HAsciiString::RealValue() const
{
  return myString.RealValue();
}

//=================================================================================================

void TCollection_HAsciiString::RemoveAll(const char what, const bool CaseSensitive)
{
  myString.RemoveAll(what, CaseSensitive);
}

//=================================================================================================

void TCollection_HAsciiString::RemoveAll(const char what)
{
  myString.RemoveAll(what);
}

//=================================================================================================

void TCollection_HAsciiString::Remove(const int where, const int ahowmany)
{
  myString.Remove(where, ahowmany);
}

//=================================================================================================

void TCollection_HAsciiString::RightAdjust()
{
  myString.RightAdjust();
}

//=================================================================================================

void TCollection_HAsciiString::RightJustify(const int Width, const char Filler)
{
  if (Width < 0)
    throw Standard_NegativeValue();
  myString.RightJustify(Width, Filler);
}

//=================================================================================================

int TCollection_HAsciiString::Search(const char* const what) const
{
  return myString.Search(what);
}

//=================================================================================================

int TCollection_HAsciiString::Search(const occ::handle<TCollection_HAsciiString>& what) const
{
  return myString.Search(what->String());
}

//=================================================================================================

int TCollection_HAsciiString::SearchFromEnd(const char* const what) const
{
  return myString.SearchFromEnd(what);
}

//=================================================================================================

int TCollection_HAsciiString::SearchFromEnd(const occ::handle<TCollection_HAsciiString>& what) const
{
  return myString.SearchFromEnd(what->String());
}

//=================================================================================================

void TCollection_HAsciiString::SetValue(const int where, const char what)
{
  myString.SetValue(where, what);
}

//=================================================================================================

void TCollection_HAsciiString::SetValue(const int where, const char* const what)
{
  myString.SetValue(where, what);
}

//=================================================================================================

void TCollection_HAsciiString::SetValue(const int                                    where,
                                        const occ::handle<TCollection_HAsciiString>& what)
{
  myString.SetValue(where, what->String());
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> TCollection_HAsciiString::Split(const int where)
{
  return new TCollection_HAsciiString(myString.Split(where));
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> TCollection_HAsciiString::SubString(const int FromIndex,
                                                                          const int ToIndex) const
{
  return new TCollection_HAsciiString(myString.SubString(FromIndex, ToIndex));
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> TCollection_HAsciiString::Token(const char* const separators,
                                                                      const int whichone) const
{
  return new TCollection_HAsciiString(myString.Token(separators, whichone));
}

//=================================================================================================

void TCollection_HAsciiString::Trunc(const int ahowmany)
{
  myString.Trunc(ahowmany);
}

//=================================================================================================

void TCollection_HAsciiString::UpperCase()
{
  myString.UpperCase();
}

//=================================================================================================

int TCollection_HAsciiString::UsefullLength() const
{
  return myString.UsefullLength();
}

//=================================================================================================

char TCollection_HAsciiString::Value(const int where) const
{
  return myString.Value(where);
}

//=================================================================================================

bool TCollection_HAsciiString::IsSameState(const occ::handle<TCollection_HAsciiString>& other) const
{
  if (myString.Length() == other->Length())
    return (strncmp(myString.ToCString(), other->ToCString(), myString.Length()) == 0);
  else
    return false;
}
