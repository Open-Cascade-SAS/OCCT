// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _UnitsMethods_HeaderFile
#define _UnitsMethods_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <UnitsMethods_LengthUnit.hxx>

//! Class for using global units variables
class UnitsMethods
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns value of unit encoded by parameter theUnit
  //! (integer value denoting unit, as described in IGES
  //! standard) in millimeters by default
  Standard_EXPORT static Standard_Real GetLengthFactorValue(const Standard_Integer theUnit);

  //! Returns value of current internal unit for CASCADE
  //! in millemeters by default
  Standard_EXPORT static Standard_Real GetCasCadeLengthUnit(
    const UnitsMethods_LengthUnit theBaseUnit = UnitsMethods_LengthUnit_Millimeter);

  //! Sets value of current internal unit for CASCADE
  Standard_EXPORT static void SetCasCadeLengthUnit(
    const Standard_Real           theUnitValue,
    const UnitsMethods_LengthUnit theBaseUnit = UnitsMethods_LengthUnit_Millimeter);

  //! Sets value of current internal unit for CASCADE
  //! by parameter theUnit (integer value denoting unit,
  //! as described in IGES standard)
  Standard_EXPORT static void SetCasCadeLengthUnit(const Standard_Integer theUnit);

  //! Returns the scale factor for switch from first given unit to second given unit
  Standard_EXPORT static Standard_Real GetLengthUnitScale(const UnitsMethods_LengthUnit theFromUnit,
                                                          const UnitsMethods_LengthUnit theToUnit);

  //! Returns the enumeration corresponding to the given scale factor
  Standard_EXPORT static UnitsMethods_LengthUnit GetLengthUnitByFactorValue(
    const Standard_Real           theFactorValue,
    const UnitsMethods_LengthUnit theBaseUnit = UnitsMethods_LengthUnit_Millimeter);

  //! Returns string name for the given scale factor
  Standard_EXPORT static Standard_CString DumpLengthUnit(
    const Standard_Real           theScaleFactor,
    const UnitsMethods_LengthUnit theBaseUnit = UnitsMethods_LengthUnit_Millimeter);

  //! Returns string for the given value of LengthUnit
  Standard_EXPORT static Standard_CString DumpLengthUnit(const UnitsMethods_LengthUnit theUnit);

  //! Make conversion of given string to value of LengthUnit
  Standard_EXPORT static UnitsMethods_LengthUnit LengthUnitFromString(
    Standard_CString       theStr,
    const Standard_Boolean theCaseSensitive);
};

#endif // _UnitsMethods_HeaderFile
