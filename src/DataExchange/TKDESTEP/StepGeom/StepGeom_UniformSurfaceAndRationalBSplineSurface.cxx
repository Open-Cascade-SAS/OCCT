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
#include <StepGeom_RationalBSplineSurface.hxx>
#include <StepGeom_UniformSurface.hxx>
#include <StepGeom_UniformSurfaceAndRationalBSplineSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_UniformSurfaceAndRationalBSplineSurface,
                           StepGeom_BSplineSurface)

StepGeom_UniformSurfaceAndRationalBSplineSurface::StepGeom_UniformSurfaceAndRationalBSplineSurface()
{
}

void StepGeom_UniformSurfaceAndRationalBSplineSurface::Init(
  const Handle(TCollection_HAsciiString)&         aName,
  const Standard_Integer                          aUDegree,
  const Standard_Integer                          aVDegree,
  const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
  const StepGeom_BSplineSurfaceForm               aSurfaceForm,
  const StepData_Logical                          aUClosed,
  const StepData_Logical                          aVClosed,
  const StepData_Logical                          aSelfIntersect,
  const Handle(StepGeom_UniformSurface)&          aUniformSurface,
  const Handle(StepGeom_RationalBSplineSurface)&  aRationalBSplineSurface)
{
  // --- classe own fields ---
  uniformSurface         = aUniformSurface;
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

void StepGeom_UniformSurfaceAndRationalBSplineSurface::Init(
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

  // --- ANDOR component fields ---

  uniformSurface = new StepGeom_UniformSurface();
  uniformSurface->Init(aName,
                       aUDegree,
                       aVDegree,
                       aControlPointsList,
                       aSurfaceForm,
                       aUClosed,
                       aVClosed,
                       aSelfIntersect);
}

void StepGeom_UniformSurfaceAndRationalBSplineSurface::SetUniformSurface(
  const Handle(StepGeom_UniformSurface)& aUniformSurface)
{
  uniformSurface = aUniformSurface;
}

Handle(StepGeom_UniformSurface) StepGeom_UniformSurfaceAndRationalBSplineSurface::UniformSurface()
  const
{
  return uniformSurface;
}

void StepGeom_UniformSurfaceAndRationalBSplineSurface::SetRationalBSplineSurface(
  const Handle(StepGeom_RationalBSplineSurface)& aRationalBSplineSurface)
{
  rationalBSplineSurface = aRationalBSplineSurface;
}

Handle(StepGeom_RationalBSplineSurface) StepGeom_UniformSurfaceAndRationalBSplineSurface::
  RationalBSplineSurface() const
{
  return rationalBSplineSurface;
}

//--- Specific Methods for AND classe field access ---

void StepGeom_UniformSurfaceAndRationalBSplineSurface::SetWeightsData(
  const Handle(TColStd_HArray2OfReal)& aWeightsData)
{
  rationalBSplineSurface->SetWeightsData(aWeightsData);
}

Handle(TColStd_HArray2OfReal) StepGeom_UniformSurfaceAndRationalBSplineSurface::WeightsData() const
{
  return rationalBSplineSurface->WeightsData();
}

Standard_Real StepGeom_UniformSurfaceAndRationalBSplineSurface::WeightsDataValue(
  const Standard_Integer num1,
  const Standard_Integer num2) const
{
  return rationalBSplineSurface->WeightsDataValue(num1, num2);
}

Standard_Integer StepGeom_UniformSurfaceAndRationalBSplineSurface::NbWeightsDataI() const
{
  return rationalBSplineSurface->NbWeightsDataI();
}

Standard_Integer StepGeom_UniformSurfaceAndRationalBSplineSurface::NbWeightsDataJ() const
{
  return rationalBSplineSurface->NbWeightsDataJ();
}

//--- Specific Methods for AND classe field access ---
