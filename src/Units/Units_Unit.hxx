// Created on: 1992-10-29
// Created by: Gilles DEBARBOUILLE
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

#ifndef _Units_Unit_HeaderFile
#define _Units_Unit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class Units_Quantity;
class Units_Token;

class Units_Unit;
DEFINE_STANDARD_HANDLE(Units_Unit, Standard_Transient)

//! This class defines an elementary word contained in
//! a physical quantity.
class Units_Unit : public Standard_Transient
{

public:
  //! Creates  and returns a  unit.  <aname> is  the name of
  //! the  unit, <asymbol> is the  usual abbreviation of the
  //! unit,  and  <avalue> is the  value in relation to  the
  //! International System of Units.
  Standard_EXPORT Units_Unit(const Standard_CString        aname,
                             const Standard_CString        asymbol,
                             const Standard_Real           avalue,
                             const Handle(Units_Quantity)& aquantity);

  //! Creates  and returns a  unit.  <aname> is  the name of
  //! the  unit, <asymbol> is the  usual abbreviation of the
  //! unit.
  Standard_EXPORT Units_Unit(const Standard_CString aname, const Standard_CString asymbol);

  //! Creates  and returns a  unit.  <aname> is  the name of
  //! the  unit.
  Standard_EXPORT Units_Unit(const Standard_CString aname);

  //! Returns the name of the unit <thename>
  TCollection_AsciiString Name() const;

  //! Adds a new symbol <asymbol> attached to <me>.
  Standard_EXPORT void Symbol(const Standard_CString asymbol);

  //! Returns the  value in relation  with the International
  //! System of Units.
  Standard_Real Value() const;

  //! Returns <thequantity> contained in <me>.
  Handle(Units_Quantity) Quantity() const;

  //! Returns the sequence of symbols <thesymbolssequence>
  Handle(TColStd_HSequenceOfHAsciiString) SymbolsSequence() const;

  //! Sets the value <avalue> to <me>.
  void Value(const Standard_Real avalue);

  //! Sets the physical Quantity <aquantity> to <me>.
  void Quantity(const Handle(Units_Quantity)& aquantity);

  //! Starting with <me>, returns a new Token object.
  Standard_EXPORT virtual Handle(Units_Token) Token() const;

  //! Compares all the symbols  linked  within <me> with the
  //! name of <atoken>,  and returns  True  if there is  one
  //! symbol equal to the name, False otherwise.
  Standard_EXPORT Standard_Boolean IsEqual(const Standard_CString astring) const;

  //! Useful for debugging
  Standard_EXPORT virtual void Dump(const Standard_Integer ashift,
                                    const Standard_Integer alevel) const;

  DEFINE_STANDARD_RTTIEXT(Units_Unit, Standard_Transient)

protected:
  Handle(TColStd_HSequenceOfHAsciiString) thesymbolssequence;
  Standard_Real                           thevalue;

private:
  Handle(TCollection_HAsciiString) thename;
  Handle(Units_Quantity)           thequantity;
};

#include <Units_Unit.lxx>

#endif // _Units_Unit_HeaderFile
