// Created on: 1993-01-13
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESDefs_UnitsData_HeaderFile
#define _IGESDefs_UnitsData_HeaderFile

#include <Standard.hxx>

#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class TCollection_HAsciiString;

//! defines IGES UnitsData Entity, Type <316> Form <0>
//! in package IGESDefs
//! This class stores data about a model's fundamental units.
class IGESDefs_UnitsData : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDefs_UnitsData();

  //! This method is used to set the fields of the class
  //! UnitsData
  //! - unitTypes  : Types of the units being defined
  //! - unitValues : Unit Values of the units
  //! - unitScales : Multiplicative Scale Factors
  //! raises exception if lengths of unitTypes, unitValues and
  //! unitScale are not same
  Standard_EXPORT void Init(const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& unitTypes,
                            const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& unitValues,
                            const occ::handle<NCollection_HArray1<double>>&           unitScales);

  //! returns the Number of units defined by this entity
  Standard_EXPORT int NbUnits() const;

  //! returns the Type of the UnitNum'th unit being defined
  //! raises exception if UnitNum <= 0 or UnitNum > NbUnits()
  Standard_EXPORT occ::handle<TCollection_HAsciiString> UnitType(const int UnitNum) const;

  //! returns the Units of the UnitNum'th unit being defined
  //! raises exception if UnitNum <= 0 or UnitNum > NbUnits()
  Standard_EXPORT occ::handle<TCollection_HAsciiString> UnitValue(const int UnitNum) const;

  //! returns the multiplicative scale factor to be applied to the
  //! UnitNum'th unit being defined
  //! raises exception if UnitNum <= 0 or UnitNum > NbUnits()
  Standard_EXPORT double ScaleFactor(const int UnitNum) const;

  DEFINE_STANDARD_RTTIEXT(IGESDefs_UnitsData, IGESData_IGESEntity)

private:
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> theUnitTypes;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> theUnitValues;
  occ::handle<NCollection_HArray1<double>>           theUnitScales;
};

#endif // _IGESDefs_UnitsData_HeaderFile
