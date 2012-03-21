// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDefs_UnitsData.ixx>


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
    Standard_DimensionMismatch::Raise("IGESDefs_UnitsData : Init");
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
