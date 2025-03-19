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
#include <StepGeom_QuasiUniformSurface.hxx>
#include <StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_RationalBSplineSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface,
                           StepGeom_BSplineSurface)

StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::
  StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface()
{
}

void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::Init(
  const Handle(TCollection_HAsciiString)&         aName,
  const Standard_Integer                          aUDegree,
  const Standard_Integer                          aVDegree,
  const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
  const StepGeom_BSplineSurfaceForm               aSurfaceForm,
  const StepData_Logical                          aUClosed,
  const StepData_Logical                          aVClosed,
  const StepData_Logical                          aSelfIntersect,
  const Handle(StepGeom_QuasiUniformSurface)&     aQuasiUniformSurface,
  const Handle(StepGeom_RationalBSplineSurface)&  aRationalBSplineSurface)
{
  // --- classe own fields ---
  quasiUniformSurface    = aQuasiUniformSurface;
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

void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::Init(
  const Handle(TCollection_HAsciiString)&         aName,
  const Standard_Integer                          aUDegree,
  const Standard_Integer                          aVDegree,
  const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
  const StepGeom_BSplineSurfaceForm               aSurfaceForm,
  const StepData_Logical                          aUClosed,
  const StepData_Logical                          aVClosed,
  const StepData_Logical                          aSelfIntersect,
  const Handle(TColStd_HArray2OfReal)&            aWeightsData)
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

  quasiUniformSurface = new StepGeom_QuasiUniformSurface();
  quasiUniformSurface->Init(aName,
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

void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::SetQuasiUniformSurface(
  const Handle(StepGeom_QuasiUniformSurface)& aQuasiUniformSurface)
{
  quasiUniformSurface = aQuasiUniformSurface;
}

Handle(StepGeom_QuasiUniformSurface) StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::
  QuasiUniformSurface() const
{
  return quasiUniformSurface;
}

void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::SetRationalBSplineSurface(
  const Handle(StepGeom_RationalBSplineSurface)& aRationalBSplineSurface)
{
  rationalBSplineSurface = aRationalBSplineSurface;
}

Handle(StepGeom_RationalBSplineSurface) StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::
  RationalBSplineSurface() const
{
  return rationalBSplineSurface;
}

//--- Specific Methods for AND classe field access ---

//--- Specific Methods for AND classe field access ---

void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::SetWeightsData(
  const Handle(TColStd_HArray2OfReal)& aWeightsData)
{
  rationalBSplineSurface->SetWeightsData(aWeightsData);
}

Handle(TColStd_HArray2OfReal) StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::WeightsData()
  const
{
  return rationalBSplineSurface->WeightsData();
}

Standard_Real StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::WeightsDataValue(
  const Standard_Integer num1,
  const Standard_Integer num2) const
{
  return rationalBSplineSurface->WeightsDataValue(num1, num2);
}

Standard_Integer StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::NbWeightsDataI() const
{
  return rationalBSplineSurface->NbWeightsDataI();
}

Standard_Integer StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::NbWeightsDataJ() const
{
  return rationalBSplineSurface->NbWeightsDataJ();
}
