// Created on: 1992-06-22
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

#ifndef _Units_Quantity_HeaderFile
#define _Units_Quantity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Units_UnitsSequence.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class Units_Dimensions;

class Units_Quantity;
DEFINE_STANDARD_HANDLE(Units_Quantity, Standard_Transient)

//! This  class stores  in its  field all the possible
//! units of all the unit systems for a given physical
//! quantity. Each unit's  value  is  expressed in the
//! S.I. unit system.
class Units_Quantity : public Standard_Transient
{

public:
  //! Creates  a new Quantity  object with <aname> which  is
  //! the name of the physical quantity, <adimensions> which
  //! is the physical dimensions, and <aunitssequence> which
  //! describes all the units known for this quantity.
  Units_Quantity(const Standard_CString             aname,
                 const Handle(Units_Dimensions)&    adimensions,
                 const Handle(Units_UnitsSequence)& aunitssequence);

  //! Returns in a AsciiString from TCollection the name of the quantity.
  TCollection_AsciiString Name() const;

  //! Returns the physical dimensions of the quantity.
  Handle(Units_Dimensions) Dimensions() const;

  //! Returns <theunitssequence>, which  is the  sequence of
  //! all the units stored for this physical quantity.
  Handle(Units_UnitsSequence) Sequence() const;

  //! Returns True if the name of the Quantity <me> is equal
  //! to <astring>, False otherwise.
  Standard_EXPORT Standard_Boolean IsEqual(const Standard_CString astring) const;

  //! Useful for debugging.
  Standard_EXPORT void Dump(const Standard_Integer ashift, const Standard_Integer alevel) const;

  DEFINE_STANDARD_RTTIEXT(Units_Quantity, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString) thename;
  Handle(Units_Dimensions)         thedimensions;
  Handle(Units_UnitsSequence)      theunitssequence;
};

#include <Units_Quantity.lxx>

#endif // _Units_Quantity_HeaderFile
