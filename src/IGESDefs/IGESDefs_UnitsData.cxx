// Created by: CKY / Contract Toubro-Larsen
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDefs_UnitsData.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_UnitsData,IGESData_IGESEntity)

IGESDefs_UnitsData::IGESDefs_UnitsData ()    {  }


    void  IGESDefs_UnitsData::Init
  (const Handle(Interface_HArray1OfHAsciiString)& unitTypes,
   const Handle(Interface_HArray1OfHAsciiString)& unitValues,
   const Handle(TColStd_HArray1OfReal)& unitScales)
{
  Standard_Integer length = unitTypes->Length();
  if ( unitTypes->Lower()  != 1  ||
      (unitValues->Lower() != 1 || unitValues->Length() != length) ||
      (unitScales->Lower() != 1 || unitScales->Length() != length) )
    throw Standard_DimensionMismatch("IGESDefs_UnitsData : Init");
  theUnitTypes   = unitTypes;
  theUnitValues  = unitValues;
  theUnitScales  = unitScales;
  InitTypeAndForm(316,0);
}

    Standard_Integer  IGESDefs_UnitsData::NbUnits () const 
{
  return theUnitTypes->Length();
}

    Handle(TCollection_HAsciiString)  IGESDefs_UnitsData::UnitType
  (const Standard_Integer UnitNum) const 
{
  return theUnitTypes->Value(UnitNum);
}

    Handle(TCollection_HAsciiString)  IGESDefs_UnitsData::UnitValue
  (const Standard_Integer UnitNum) const 
{
  return theUnitValues->Value(UnitNum);
}

    Standard_Real  IGESDefs_UnitsData::ScaleFactor
  (const Standard_Integer UnitNum) const 
{
  return theUnitScales->Value(UnitNum);
}
