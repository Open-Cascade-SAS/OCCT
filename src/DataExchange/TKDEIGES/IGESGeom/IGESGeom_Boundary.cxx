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
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_Boundary, IGESData_IGESEntity)

IGESGeom_Boundary::IGESGeom_Boundary() {}

void IGESGeom_Boundary::Init(
  const int                                aType,
  const int                                aPreference,
  const occ::handle<IGESData_IGESEntity>&                    aSurface,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&           allModelCurves,
  const occ::handle<NCollection_HArray1<int>>&               allSenses,
  const occ::handle<IGESBasic_HArray1OfHArray1OfIGESEntity>& allParameterCurves)
{
  int num1 = allSenses->Length();
  if (allSenses->Lower() != 1 || (allModelCurves->Lower() != 1 || allModelCurves->Length() != num1)
      || (allParameterCurves->Lower() != 1 || allParameterCurves->Length() != num1))
    throw Standard_DimensionMismatch("IGESGeom_Boundary: Init");

  theType            = aType;
  thePreference      = aPreference;
  theSurface         = aSurface;
  theModelCurves     = allModelCurves;
  theSenses          = allSenses;
  theParameterCurves = allParameterCurves;
  InitTypeAndForm(141, 0);
}

int IGESGeom_Boundary::BoundaryType() const
{
  return theType;
}

int IGESGeom_Boundary::PreferenceType() const
{
  return thePreference;
}

occ::handle<IGESData_IGESEntity> IGESGeom_Boundary::Surface() const
{
  return theSurface;
}

int IGESGeom_Boundary::NbModelSpaceCurves() const
{
  return theModelCurves->Length();
}

int IGESGeom_Boundary::Sense(const int Index) const
{
  return theSenses->Value(Index);
}

occ::handle<IGESData_IGESEntity> IGESGeom_Boundary::ModelSpaceCurve(const int Index) const
{
  return theModelCurves->Value(Index);
}

int IGESGeom_Boundary::NbParameterCurves(const int Index) const
{
  if (theParameterCurves->Value(Index).IsNull())
    return 0;
  return theParameterCurves->Value(Index)->Length();
}

occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> IGESGeom_Boundary::ParameterCurves(
  const int Index) const
{
  return theParameterCurves->Value(Index);
}

occ::handle<IGESData_IGESEntity> IGESGeom_Boundary::ParameterCurve(const int Index,
                                                              const int Num) const
{
  return theParameterCurves->Value(Index)->Value(Num);
}
