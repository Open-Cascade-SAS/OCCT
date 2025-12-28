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

#include <StepGeom_BSplineCurve.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_BSplineCurve, StepGeom_BoundedCurve)

StepGeom_BSplineCurve::StepGeom_BSplineCurve() {}

void StepGeom_BSplineCurve::Init(const occ::handle<TCollection_HAsciiString>&         aName,
                                 const int                          aDegree,
                                 const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
                                 const StepGeom_BSplineCurveForm                 aCurveForm,
                                 const StepData_Logical                          aClosedCurve,
                                 const StepData_Logical                          aSelfIntersect)
{
  // --- classe own fields ---
  degree            = aDegree;
  controlPointsList = aControlPointsList;
  curveForm         = aCurveForm;
  closedCurve       = aClosedCurve;
  selfIntersect     = aSelfIntersect;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_BSplineCurve::SetDegree(const int aDegree)
{
  degree = aDegree;
}

int StepGeom_BSplineCurve::Degree() const
{
  return degree;
}

void StepGeom_BSplineCurve::SetControlPointsList(
  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList)
{
  controlPointsList = aControlPointsList;
}

occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> StepGeom_BSplineCurve::ControlPointsList() const
{
  return controlPointsList;
}

occ::handle<StepGeom_CartesianPoint> StepGeom_BSplineCurve::ControlPointsListValue(
  const int num) const
{
  return controlPointsList->Value(num);
}

int StepGeom_BSplineCurve::NbControlPointsList() const
{
  if (controlPointsList.IsNull())
    return 0;
  return controlPointsList->Length();
}

void StepGeom_BSplineCurve::SetCurveForm(const StepGeom_BSplineCurveForm aCurveForm)
{
  curveForm = aCurveForm;
}

StepGeom_BSplineCurveForm StepGeom_BSplineCurve::CurveForm() const
{
  return curveForm;
}

void StepGeom_BSplineCurve::SetClosedCurve(const StepData_Logical aClosedCurve)
{
  closedCurve = aClosedCurve;
}

StepData_Logical StepGeom_BSplineCurve::ClosedCurve() const
{
  return closedCurve;
}

void StepGeom_BSplineCurve::SetSelfIntersect(const StepData_Logical aSelfIntersect)
{
  selfIntersect = aSelfIntersect;
}

StepData_Logical StepGeom_BSplineCurve::SelfIntersect() const
{
  return selfIntersect;
}
