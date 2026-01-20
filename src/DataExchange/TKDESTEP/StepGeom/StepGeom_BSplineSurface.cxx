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

#include <StepGeom_BSplineSurface.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_BSplineSurface, StepGeom_BoundedSurface)

StepGeom_BSplineSurface::StepGeom_BSplineSurface() {}

void StepGeom_BSplineSurface::Init(
  const occ::handle<TCollection_HAsciiString>&         aName,
  const int                          aUDegree,
  const int                          aVDegree,
  const occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
  const StepGeom_BSplineSurfaceForm               aSurfaceForm,
  const StepData_Logical                          aUClosed,
  const StepData_Logical                          aVClosed,
  const StepData_Logical                          aSelfIntersect)
{
  // --- classe own fields ---
  uDegree           = aUDegree;
  vDegree           = aVDegree;
  controlPointsList = aControlPointsList;
  surfaceForm       = aSurfaceForm;
  uClosed           = aUClosed;
  vClosed           = aVClosed;
  selfIntersect     = aSelfIntersect;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_BSplineSurface::SetUDegree(const int aUDegree)
{
  uDegree = aUDegree;
}

int StepGeom_BSplineSurface::UDegree() const
{
  return uDegree;
}

void StepGeom_BSplineSurface::SetVDegree(const int aVDegree)
{
  vDegree = aVDegree;
}

int StepGeom_BSplineSurface::VDegree() const
{
  return vDegree;
}

void StepGeom_BSplineSurface::SetControlPointsList(
  const occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList)
{
  controlPointsList = aControlPointsList;
}

occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>> StepGeom_BSplineSurface::ControlPointsList() const
{
  return controlPointsList;
}

occ::handle<StepGeom_CartesianPoint> StepGeom_BSplineSurface::ControlPointsListValue(
  const int num1,
  const int num2) const
{
  return controlPointsList->Value(num1, num2);
}

int StepGeom_BSplineSurface::NbControlPointsListI() const
{
  if (controlPointsList.IsNull())
    return 0;
  return controlPointsList->UpperRow() - controlPointsList->LowerRow() + 1;
}

int StepGeom_BSplineSurface::NbControlPointsListJ() const
{
  if (controlPointsList.IsNull())
    return 0;
  return controlPointsList->UpperCol() - controlPointsList->LowerCol() + 1;
}

void StepGeom_BSplineSurface::SetSurfaceForm(const StepGeom_BSplineSurfaceForm aSurfaceForm)
{
  surfaceForm = aSurfaceForm;
}

StepGeom_BSplineSurfaceForm StepGeom_BSplineSurface::SurfaceForm() const
{
  return surfaceForm;
}

void StepGeom_BSplineSurface::SetUClosed(const StepData_Logical aUClosed)
{
  uClosed = aUClosed;
}

StepData_Logical StepGeom_BSplineSurface::UClosed() const
{
  return uClosed;
}

void StepGeom_BSplineSurface::SetVClosed(const StepData_Logical aVClosed)
{
  vClosed = aVClosed;
}

StepData_Logical StepGeom_BSplineSurface::VClosed() const
{
  return vClosed;
}

void StepGeom_BSplineSurface::SetSelfIntersect(const StepData_Logical aSelfIntersect)
{
  selfIntersect = aSelfIntersect;
}

StepData_Logical StepGeom_BSplineSurface::SelfIntersect() const
{
  return selfIntersect;
}
