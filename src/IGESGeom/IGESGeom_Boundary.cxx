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

#include <IGESBasic_HArray1OfHArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_Boundary.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_Boundary,IGESData_IGESEntity)

IGESGeom_Boundary::IGESGeom_Boundary ()    {  }


    void  IGESGeom_Boundary::Init
  (const Standard_Integer aType,
   const Standard_Integer aPreference,
   const Handle(IGESData_IGESEntity)& aSurface,
   const Handle(IGESData_HArray1OfIGESEntity)& allModelCurves,
   const Handle(TColStd_HArray1OfInteger)& allSenses,
   const Handle(IGESBasic_HArray1OfHArray1OfIGESEntity)& allParameterCurves)
{
  Standard_Integer num1 = allSenses->Length();
  if ( allSenses->Lower()          != 1 ||
      (allModelCurves->Lower()     != 1 || allModelCurves->Length() != num1) ||
      (allParameterCurves->Lower() != 1 || allParameterCurves->Length() != num1))
    throw Standard_DimensionMismatch("IGESGeom_Boundary: Init");

  theType            = aType;
  thePreference      = aPreference;
  theSurface         = aSurface;
  theModelCurves     = allModelCurves;
  theSenses          = allSenses;
  theParameterCurves = allParameterCurves;
  InitTypeAndForm(141,0);
}


    Standard_Integer  IGESGeom_Boundary::BoundaryType () const
{
  return theType;
}

    Standard_Integer  IGESGeom_Boundary::PreferenceType () const
{
  return thePreference;
}

    Handle(IGESData_IGESEntity)  IGESGeom_Boundary::Surface () const
{
  return theSurface;
}

    Standard_Integer  IGESGeom_Boundary::NbModelSpaceCurves () const
{
  return theModelCurves->Length();
}

    Standard_Integer  IGESGeom_Boundary::Sense (const Standard_Integer Index) const
{
  return theSenses->Value(Index);
}

    Handle(IGESData_IGESEntity)  IGESGeom_Boundary::ModelSpaceCurve
  (const Standard_Integer Index) const
{
  return theModelCurves->Value(Index);
}

    Standard_Integer  IGESGeom_Boundary::NbParameterCurves
  (const Standard_Integer Index) const
{
  if (theParameterCurves->Value(Index).IsNull()) return 0;
  return  theParameterCurves->Value(Index)->Length();
}

    Handle(IGESData_HArray1OfIGESEntity)  IGESGeom_Boundary::ParameterCurves
  (const Standard_Integer Index) const
{
  return theParameterCurves->Value(Index);
}

    Handle(IGESData_IGESEntity)  IGESGeom_Boundary::ParameterCurve
  (const Standard_Integer Index, const Standard_Integer Num) const
{
  return theParameterCurves->Value(Index)->Value(Num);
}
