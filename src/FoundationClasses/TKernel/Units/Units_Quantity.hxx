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

#include <Units_Unit.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class Units_Dimensions;

//! This class stores in its field all the possible
//! units of all the unit systems for a given physical
//! quantity. Each unit's value is expressed in the
//! S.I. unit system.
class Units_Quantity : public Standard_Transient
{

public:
  //! Creates a new Quantity object with <aname> which is
  //! the name of the physical quantity, <adimensions> which
  //! is the physical dimensions, and <aunitssequence> which
  //! describes all the units known for this quantity.
  Units_Quantity(const char*             aname,
                 const occ::handle<Units_Dimensions>&    adimensions,
                 const occ::handle<NCollection_HSequence<occ::handle<Units_Unit>>>& aunitssequence);

  //! Returns in a AsciiString from TCollection the name of the quantity.
  TCollection_AsciiString Name() const;

  //! Returns the physical dimensions of the quantity.
  occ::handle<Units_Dimensions> Dimensions() const;

  //! Returns <theunitssequence>, which is the sequence of
  //! all the units stored for this physical quantity.
  occ::handle<NCollection_HSequence<occ::handle<Units_Unit>>> Sequence() const;

  //! Returns True if the name of the Quantity <me> is equal
  //! to <astring>, False otherwise.
  Standard_EXPORT bool IsEqual(const char* astring) const;

  //! Useful for debugging.
  Standard_EXPORT void Dump(const int ashift, const int alevel) const;

  DEFINE_STANDARD_RTTIEXT(Units_Quantity, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString> thename;
  occ::handle<Units_Dimensions>         thedimensions;
  occ::handle<NCollection_HSequence<occ::handle<Units_Unit>>>      theunitssequence;
};

#include <Units_Quantity.lxx>

#endif // _Units_Quantity_HeaderFile
