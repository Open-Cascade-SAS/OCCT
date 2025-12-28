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

#include <Standard_Type.hxx>
#include <StepGeom_BezierSurface.hxx>
#include <StepGeom_BezierSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_RationalBSplineSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_BezierSurfaceAndRationalBSplineSurface, StepGeom_BSplineSurface)

StepGeom_BezierSurfaceAndRationalBSplineSurface::StepGeom_BezierSurfaceAndRationalBSplineSurface()
{
}

void StepGeom_BezierSurfaceAndRationalBSplineSurface::Init(
  const occ::handle<TCollection_HAsciiString>&         aName,
  const int                          aUDegree,
  const int                          aVDegree,
  const occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
  const StepGeom_BSplineSurfaceForm               aSurfaceForm,
  const StepData_Logical                          aUClosed,
  const StepData_Logical                          aVClosed,
  const StepData_Logical                          aSelfIntersect,
  const occ::handle<StepGeom_BezierSurface>&           aBezierSurface,
  const occ::handle<StepGeom_RationalBSplineSurface>&  aRationalBSplineSurface)
{
  // --- classe own fields ---
  bezierSurface          = aBezierSurface;
  rationalBSplineSurface = aRationalBSplineSurface;
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

void StepGeom_BezierSurfaceAndRationalBSplineSurface::Init(
  const occ::handle<TCollection_HAsciiString>&         aName,
  const int                          aUDegree,
  const int                          aVDegree,
  const occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
  const StepGeom_BSplineSurfaceForm               aSurfaceForm,
  const StepData_Logical                          aUClosed,
  const StepData_Logical                          aVClosed,
  const StepData_Logical                          aSelfIntersect,
  const occ::handle<NCollection_HArray2<double>>&            aWeightsData)
{
  // --- classe inherited fields ---

  StepGeom_BSplineSurface::Init(aName,
                                aUDegree,
                                aVDegree,
                                aControlPointsList,
                                aSurfaceForm,
                                aUClosed,
                                aVClosed,
                                aSelfIntersect);

  // --- ANDOR component fields ---

  bezierSurface = new StepGeom_BezierSurface();
  bezierSurface->Init(aName,
                      aUDegree,
                      aVDegree,
                      aControlPointsList,
                      aSurfaceForm,
                      aUClosed,
                      aVClosed,
                      aSelfIntersect);

  // --- ANDOR component fields ---

  rationalBSplineSurface = new StepGeom_RationalBSplineSurface();
  rationalBSplineSurface->Init(aName,
                               aUDegree,
                               aVDegree,
                               aControlPointsList,
                               aSurfaceForm,
                               aUClosed,
                               aVClosed,
                               aSelfIntersect,
                               aWeightsData);
}

void StepGeom_BezierSurfaceAndRationalBSplineSurface::SetBezierSurface(
  const occ::handle<StepGeom_BezierSurface>& aBezierSurface)
{
  bezierSurface = aBezierSurface;
}

occ::handle<StepGeom_BezierSurface> StepGeom_BezierSurfaceAndRationalBSplineSurface::BezierSurface()
  const
{
  return bezierSurface;
}

void StepGeom_BezierSurfaceAndRationalBSplineSurface::SetRationalBSplineSurface(
  const occ::handle<StepGeom_RationalBSplineSurface>& aRationalBSplineSurface)
{
  rationalBSplineSurface = aRationalBSplineSurface;
}

occ::handle<StepGeom_RationalBSplineSurface> StepGeom_BezierSurfaceAndRationalBSplineSurface::
  RationalBSplineSurface() const
{
  return rationalBSplineSurface;
}

//--- Specific Methods for AND classe field access ---

//--- Specific Methods for AND classe field access ---

void StepGeom_BezierSurfaceAndRationalBSplineSurface::SetWeightsData(
  const occ::handle<NCollection_HArray2<double>>& aWeightsData)
{
  rationalBSplineSurface->SetWeightsData(aWeightsData);
}

occ::handle<NCollection_HArray2<double>> StepGeom_BezierSurfaceAndRationalBSplineSurface::WeightsData() const
{
  return rationalBSplineSurface->WeightsData();
}

double StepGeom_BezierSurfaceAndRationalBSplineSurface::WeightsDataValue(
  const int num1,
  const int num2) const
{
  return rationalBSplineSurface->WeightsDataValue(num1, num2);
}

int StepGeom_BezierSurfaceAndRationalBSplineSurface::NbWeightsDataI() const
{
  return rationalBSplineSurface->NbWeightsDataI();
}

int StepGeom_BezierSurfaceAndRationalBSplineSurface::NbWeightsDataJ() const
{
  return rationalBSplineSurface->NbWeightsDataJ();
}
