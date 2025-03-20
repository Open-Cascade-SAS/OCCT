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

#include <UnitsMethods.hxx>

#include <TCollection_AsciiString.hxx>

static Standard_Real UnitsMethods_CascadeLengthUnit = 1.;

//=================================================================================================

Standard_Real UnitsMethods::GetCasCadeLengthUnit(const UnitsMethods_LengthUnit theBaseUnit)
{
  return UnitsMethods_CascadeLengthUnit
         * GetLengthUnitScale(UnitsMethods_LengthUnit_Millimeter, theBaseUnit);
}

//=================================================================================================

void UnitsMethods::SetCasCadeLengthUnit(const Standard_Real           theUnitValue,
                                        const UnitsMethods_LengthUnit theBaseUnit)
{
  UnitsMethods_CascadeLengthUnit =
    theUnitValue * GetLengthUnitScale(theBaseUnit, UnitsMethods_LengthUnit_Millimeter);
}

//=================================================================================================

void UnitsMethods::SetCasCadeLengthUnit(const Standard_Integer theUnit)
{
  UnitsMethods_CascadeLengthUnit = GetLengthFactorValue(theUnit);
}

//=================================================================================================

Standard_Real UnitsMethods::GetLengthFactorValue(const Standard_Integer theUnit)
{
  switch (theUnit)
  {
    case 1:
      return 25.4; // inch
    case 2:
      return 1.; // millimeter
    case 4:
      return 304.8; // foot
    case 5:
      return 1609344.; // mile
    case 6:
      return 1000.; // meter
    case 7:
      return 1000000.; // kilometer
    case 8:
      return 0.0254; // mil (0.001 inch)
    case 9:
      return 0.001; // micron
    case 10:
      return 10.; // centimeter
    case 11:
      return 0.0000254; // microinch
    default:
      return 1.;
  }
}

//=================================================================================================

Standard_Real UnitsMethods::GetLengthUnitScale(const UnitsMethods_LengthUnit theFromUnit,
                                               const UnitsMethods_LengthUnit theToUnit)
{
  Standard_Real aVal1 = GetLengthFactorValue(theFromUnit);
  Standard_Real aVal2 = GetLengthFactorValue(theToUnit);
  return aVal1 / aVal2;
}

//=================================================================================================

UnitsMethods_LengthUnit UnitsMethods::GetLengthUnitByFactorValue(
  const Standard_Real           theFactorValue,
  const UnitsMethods_LengthUnit theBaseUnit)
{
  const Standard_Real aPreci = 1.e-6;
  const Standard_Real aValue =
    theFactorValue * GetLengthUnitScale(theBaseUnit, UnitsMethods_LengthUnit_Millimeter);
  if (Abs(1. - aValue) < aPreci)
  {
    return UnitsMethods_LengthUnit_Millimeter;
  }
  else if (Abs(25.4 - aValue) < aPreci)
  {
    return UnitsMethods_LengthUnit_Inch;
  }
  else if (Abs(304.8 - aValue) < aPreci)
  {
    return UnitsMethods_LengthUnit_Foot;
  }
  else if (Abs(1609344. - aValue) < aPreci)
  {
    return UnitsMethods_LengthUnit_Mile;
  }
  else if (Abs(1000. - aValue) < aPreci)
  {
    return UnitsMethods_LengthUnit_Meter;
  }
  else if (Abs(1000000. - aValue) < aPreci)
  {
    return UnitsMethods_LengthUnit_Kilometer;
  }
  else if (Abs(0.0254 - aValue) < aPreci)
  {
    return UnitsMethods_LengthUnit_Mil;
  }
  else if (Abs(0.001 - aValue) < aPreci)
  {
    return UnitsMethods_LengthUnit_Micron;
  }
  else if (Abs(10. - aValue) < aPreci)
  {
    return UnitsMethods_LengthUnit_Centimeter;
  }
  else if (Abs(0.0000254 - aValue) < aPreci)
  {
    return UnitsMethods_LengthUnit_Microinch;
  }
  return UnitsMethods_LengthUnit_Undefined;
}

//=================================================================================================

Standard_CString UnitsMethods::DumpLengthUnit(const UnitsMethods_LengthUnit theUnit)
{
  switch (theUnit)
  {
    case UnitsMethods_LengthUnit_Millimeter:
      return "mm";
    case UnitsMethods_LengthUnit_Meter:
      return "m";
    case UnitsMethods_LengthUnit_Centimeter:
      return "cm";
    case UnitsMethods_LengthUnit_Kilometer:
      return "km";
    case UnitsMethods_LengthUnit_Micron:
      return "micron";
    case UnitsMethods_LengthUnit_Inch:
      return "in";
    case UnitsMethods_LengthUnit_Mil:
      return "min";
    case UnitsMethods_LengthUnit_Microinch:
      return "nin";
    case UnitsMethods_LengthUnit_Foot:
      return "ft";
    case UnitsMethods_LengthUnit_Mile:
      return "stat.mile";
    default:
      return "UNDEFINED";
  }
}

//=================================================================================================

Standard_CString UnitsMethods::DumpLengthUnit(const Standard_Real           theScaleFactor,
                                              const UnitsMethods_LengthUnit theBaseUnit)
{
  const UnitsMethods_LengthUnit aUnit = GetLengthUnitByFactorValue(theScaleFactor, theBaseUnit);
  return DumpLengthUnit(aUnit);
}

//=================================================================================================

UnitsMethods_LengthUnit UnitsMethods::LengthUnitFromString(Standard_CString       theStr,
                                                           const Standard_Boolean theCaseSensitive)
{
  TCollection_AsciiString aStr(theStr);
  if (!theCaseSensitive)
  {
    aStr.LowerCase();
  }
  if (aStr.IsEqual("mm"))
  {
    return UnitsMethods_LengthUnit_Millimeter;
  }
  else if (aStr.IsEqual("m"))
  {
    return UnitsMethods_LengthUnit_Meter;
  }
  else if (aStr.IsEqual("cm"))
  {
    return UnitsMethods_LengthUnit_Centimeter;
  }
  else if (aStr.IsEqual("km"))
  {
    return UnitsMethods_LengthUnit_Kilometer;
  }
  else if (aStr.IsEqual("micron"))
  {
    return UnitsMethods_LengthUnit_Micron;
  }
  else if (aStr.IsEqual("in"))
  {
    return UnitsMethods_LengthUnit_Inch;
  }
  else if (aStr.IsEqual("min"))
  {
    return UnitsMethods_LengthUnit_Mil;
  }
  else if (aStr.IsEqual("nin"))
  {
    return UnitsMethods_LengthUnit_Microinch;
  }
  else if (aStr.IsEqual("ft"))
  {
    return UnitsMethods_LengthUnit_Foot;
  }
  else if (aStr.IsEqual("stat.mile"))
  {
    return UnitsMethods_LengthUnit_Mile;
  }
  else
  {
    return UnitsMethods_LengthUnit_Undefined;
  }
}
