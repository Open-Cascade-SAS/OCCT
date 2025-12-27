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
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_RationalBSplineSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface,
                           StepGeom_BSplineSurface)

StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::
  StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface()
{
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::Init(
  const occ::handle<TCollection_HAsciiString>&         aName,
  const int                          aUDegree,
  const int                          aVDegree,
  const occ::handle<StepGeom_HArray2OfCartesianPoint>& aControlPointsList,
  const StepGeom_BSplineSurfaceForm               aSurfaceForm,
  const StepData_Logical                          aUClosed,
  const StepData_Logical                          aVClosed,
  const StepData_Logical                          aSelfIntersect,
  const occ::handle<StepGeom_BSplineSurfaceWithKnots>& aBSplineSurfaceWithKnots,
  const occ::handle<StepGeom_RationalBSplineSurface>&  aRationalBSplineSurface)
{
  // --- classe own fields ---
  bSplineSurfaceWithKnots = aBSplineSurfaceWithKnots;
  rationalBSplineSurface  = aRationalBSplineSurface;
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

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::Init(
  const occ::handle<TCollection_HAsciiString>&         aName,
  const int                          aUDegree,
  const int                          aVDegree,
  const occ::handle<StepGeom_HArray2OfCartesianPoint>& aControlPointsList,
  const StepGeom_BSplineSurfaceForm               aSurfaceForm,
  const StepData_Logical                          aUClosed,
  const StepData_Logical                          aVClosed,
  const StepData_Logical                          aSelfIntersect,
  const occ::handle<TColStd_HArray1OfInteger>&         aUMultiplicities,
  const occ::handle<TColStd_HArray1OfInteger>&         aVMultiplicities,
  const occ::handle<TColStd_HArray1OfReal>&            aUKnots,
  const occ::handle<TColStd_HArray1OfReal>&            aVKnots,
  const StepGeom_KnotType                         aKnotSpec,
  const occ::handle<TColStd_HArray2OfReal>&            aWeightsData)
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

  bSplineSurfaceWithKnots = new StepGeom_BSplineSurfaceWithKnots();
  bSplineSurfaceWithKnots->Init(aName,
                                aUDegree,
                                aVDegree,
                                aControlPointsList,
                                aSurfaceForm,
                                aUClosed,
                                aVClosed,
                                aSelfIntersect,
                                aUMultiplicities,
                                aVMultiplicities,
                                aUKnots,
                                aVKnots,
                                aKnotSpec);

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

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetBSplineSurfaceWithKnots(
  const occ::handle<StepGeom_BSplineSurfaceWithKnots>& aBSplineSurfaceWithKnots)
{
  bSplineSurfaceWithKnots = aBSplineSurfaceWithKnots;
}

occ::handle<StepGeom_BSplineSurfaceWithKnots> StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::
  BSplineSurfaceWithKnots() const
{
  return bSplineSurfaceWithKnots;
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetRationalBSplineSurface(
  const occ::handle<StepGeom_RationalBSplineSurface>& aRationalBSplineSurface)
{
  rationalBSplineSurface = aRationalBSplineSurface;
}

occ::handle<StepGeom_RationalBSplineSurface> StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::
  RationalBSplineSurface() const
{
  return rationalBSplineSurface;
}

//--- Specific Methods for AND classe field access ---

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetUMultiplicities(
  const occ::handle<TColStd_HArray1OfInteger>& aUMultiplicities)
{
  bSplineSurfaceWithKnots->SetUMultiplicities(aUMultiplicities);
}

occ::handle<TColStd_HArray1OfInteger> StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::
  UMultiplicities() const
{
  return bSplineSurfaceWithKnots->UMultiplicities();
}

int StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::UMultiplicitiesValue(
  const int num) const
{
  return bSplineSurfaceWithKnots->UMultiplicitiesValue(num);
}

int StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbUMultiplicities()
  const
{
  return bSplineSurfaceWithKnots->NbUMultiplicities();
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetVMultiplicities(
  const occ::handle<TColStd_HArray1OfInteger>& aVMultiplicities)
{
  bSplineSurfaceWithKnots->SetVMultiplicities(aVMultiplicities);
}

occ::handle<TColStd_HArray1OfInteger> StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::
  VMultiplicities() const
{
  return bSplineSurfaceWithKnots->VMultiplicities();
}

int StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::VMultiplicitiesValue(
  const int num) const
{
  return bSplineSurfaceWithKnots->VMultiplicitiesValue(num);
}

int StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbVMultiplicities()
  const
{
  return bSplineSurfaceWithKnots->NbVMultiplicities();
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetUKnots(
  const occ::handle<TColStd_HArray1OfReal>& aUKnots)
{
  bSplineSurfaceWithKnots->SetUKnots(aUKnots);
}

occ::handle<TColStd_HArray1OfReal> StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::UKnots()
  const
{
  return bSplineSurfaceWithKnots->UKnots();
}

double StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::UKnotsValue(
  const int num) const
{
  return bSplineSurfaceWithKnots->UKnotsValue(num);
}

int StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbUKnots() const
{
  return bSplineSurfaceWithKnots->NbUKnots();
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetVKnots(
  const occ::handle<TColStd_HArray1OfReal>& aVKnots)
{
  bSplineSurfaceWithKnots->SetVKnots(aVKnots);
}

occ::handle<TColStd_HArray1OfReal> StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::VKnots()
  const
{
  return bSplineSurfaceWithKnots->VKnots();
}

double StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::VKnotsValue(
  const int num) const
{
  return bSplineSurfaceWithKnots->VKnotsValue(num);
}

int StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbVKnots() const
{
  return bSplineSurfaceWithKnots->NbVKnots();
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetKnotSpec(
  const StepGeom_KnotType aKnotSpec)
{
  bSplineSurfaceWithKnots->SetKnotSpec(aKnotSpec);
}

StepGeom_KnotType StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::KnotSpec() const
{
  return bSplineSurfaceWithKnots->KnotSpec();
}

//--- Specific Methods for AND classe field access ---

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetWeightsData(
  const occ::handle<TColStd_HArray2OfReal>& aWeightsData)
{
  rationalBSplineSurface->SetWeightsData(aWeightsData);
}

occ::handle<TColStd_HArray2OfReal> StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::
  WeightsData() const
{
  return rationalBSplineSurface->WeightsData();
}

double StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::WeightsDataValue(
  const int num1,
  const int num2) const
{
  return rationalBSplineSurface->WeightsDataValue(num1, num2);
}

int StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbWeightsDataI() const
{
  return rationalBSplineSurface->NbWeightsDataI();
}

int StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbWeightsDataJ() const
{
  return rationalBSplineSurface->NbWeightsDataJ();
}
