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

#include <StepGeom_RationalBSplineSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_RationalBSplineSurface, StepGeom_BSplineSurface)

StepGeom_RationalBSplineSurface::StepGeom_RationalBSplineSurface() {}

void StepGeom_RationalBSplineSurface::Init(
  const occ::handle<TCollection_HAsciiString>&                                  aName,
  const int                                                                     aUDegree,
  const int                                                                     aVDegree,
  const occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
  const StepGeom_BSplineSurfaceForm                                             aSurfaceForm,
  const StepData_Logical                                                        aUClosed,
  const StepData_Logical                                                        aVClosed,
  const StepData_Logical                                                        aSelfIntersect,
  const occ::handle<NCollection_HArray2<double>>&                               aWeightsData)
{
  // --- classe own fields ---
  weightsData = aWeightsData;
  // --- classe inherited fields ---
  StepGeom_BSplineSurface::Init(aName,
                                aUDegree,
                                aVDegree,
                                aControlPointsList,
                                aSurfaceForm,
                                aUClosed,
                                aVClosed,
                                aSelfIntersect);
}

void StepGeom_RationalBSplineSurface::SetWeightsData(
  const occ::handle<NCollection_HArray2<double>>& aWeightsData)
{
  weightsData = aWeightsData;
}

occ::handle<NCollection_HArray2<double>> StepGeom_RationalBSplineSurface::WeightsData() const
{
  return weightsData;
}

double StepGeom_RationalBSplineSurface::WeightsDataValue(const int num1, const int num2) const
{
  return weightsData->Value(num1, num2);
}

int StepGeom_RationalBSplineSurface::NbWeightsDataI() const
{
  return weightsData->UpperRow() - weightsData->LowerRow() + 1;
}

int StepGeom_RationalBSplineSurface::NbWeightsDataJ() const
{
  return weightsData->UpperCol() - weightsData->LowerCol() + 1;
}
