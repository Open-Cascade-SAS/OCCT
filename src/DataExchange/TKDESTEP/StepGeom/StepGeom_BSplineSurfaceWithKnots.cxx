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

#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_BSplineSurfaceWithKnots, StepGeom_BSplineSurface)

StepGeom_BSplineSurfaceWithKnots::StepGeom_BSplineSurfaceWithKnots() = default;

void StepGeom_BSplineSurfaceWithKnots::Init(
  const occ::handle<TCollection_HAsciiString>&                                  aName,
  const int                                                                     aUDegree,
  const int                                                                     aVDegree,
  const occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
  const StepGeom_BSplineSurfaceForm                                             aSurfaceForm,
  const StepData_Logical                                                        aUClosed,
  const StepData_Logical                                                        aVClosed,
  const StepData_Logical                                                        aSelfIntersect,
  const occ::handle<NCollection_HArray1<int>>&                                  aUMultiplicities,
  const occ::handle<NCollection_HArray1<int>>&                                  aVMultiplicities,
  const occ::handle<NCollection_HArray1<double>>&                               aUKnots,
  const occ::handle<NCollection_HArray1<double>>&                               aVKnots,
  const StepGeom_KnotType                                                       aKnotSpec)
{
  // --- classe own fields ---
  uMultiplicities = aUMultiplicities;
  vMultiplicities = aVMultiplicities;
  uKnots          = aUKnots;
  vKnots          = aVKnots;
  knotSpec        = aKnotSpec;
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

void StepGeom_BSplineSurfaceWithKnots::SetUMultiplicities(
  const occ::handle<NCollection_HArray1<int>>& aUMultiplicities)
{
  uMultiplicities = aUMultiplicities;
}

occ::handle<NCollection_HArray1<int>> StepGeom_BSplineSurfaceWithKnots::UMultiplicities() const
{
  return uMultiplicities;
}

int StepGeom_BSplineSurfaceWithKnots::UMultiplicitiesValue(const int num) const
{
  return uMultiplicities->Value(num);
}

int StepGeom_BSplineSurfaceWithKnots::NbUMultiplicities() const
{
  return uMultiplicities->Length();
}

void StepGeom_BSplineSurfaceWithKnots::SetVMultiplicities(
  const occ::handle<NCollection_HArray1<int>>& aVMultiplicities)
{
  vMultiplicities = aVMultiplicities;
}

occ::handle<NCollection_HArray1<int>> StepGeom_BSplineSurfaceWithKnots::VMultiplicities() const
{
  return vMultiplicities;
}

int StepGeom_BSplineSurfaceWithKnots::VMultiplicitiesValue(const int num) const
{
  return vMultiplicities->Value(num);
}

int StepGeom_BSplineSurfaceWithKnots::NbVMultiplicities() const
{
  return vMultiplicities->Length();
}

void StepGeom_BSplineSurfaceWithKnots::SetUKnots(
  const occ::handle<NCollection_HArray1<double>>& aUKnots)
{
  uKnots = aUKnots;
}

occ::handle<NCollection_HArray1<double>> StepGeom_BSplineSurfaceWithKnots::UKnots() const
{
  return uKnots;
}

double StepGeom_BSplineSurfaceWithKnots::UKnotsValue(const int num) const
{
  return uKnots->Value(num);
}

int StepGeom_BSplineSurfaceWithKnots::NbUKnots() const
{
  return uKnots->Length();
}

void StepGeom_BSplineSurfaceWithKnots::SetVKnots(
  const occ::handle<NCollection_HArray1<double>>& aVKnots)
{
  vKnots = aVKnots;
}

occ::handle<NCollection_HArray1<double>> StepGeom_BSplineSurfaceWithKnots::VKnots() const
{
  return vKnots;
}

double StepGeom_BSplineSurfaceWithKnots::VKnotsValue(const int num) const
{
  return vKnots->Value(num);
}

int StepGeom_BSplineSurfaceWithKnots::NbVKnots() const
{
  return vKnots->Length();
}

void StepGeom_BSplineSurfaceWithKnots::SetKnotSpec(const StepGeom_KnotType aKnotSpec)
{
  knotSpec = aKnotSpec;
}

StepGeom_KnotType StepGeom_BSplineSurfaceWithKnots::KnotSpec() const
{
  return knotSpec;
}
