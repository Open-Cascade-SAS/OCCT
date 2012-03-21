// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepGeom_BSplineCurve.ixx>


StepGeom_BSplineCurve::StepGeom_BSplineCurve ()  {}

void StepGeom_BSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_BSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect)
{
	// --- classe own fields ---
	degree = aDegree;
	controlPointsList = aControlPointsList;
	curveForm = aCurveForm;
	closedCurve = aClosedCurve;
	selfIntersect = aSelfIntersect;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_BSplineCurve::SetDegree(const Standard_Integer aDegree)
{
	degree = aDegree;
}

Standard_Integer StepGeom_BSplineCurve::Degree() const
{
	return degree;
}

void StepGeom_BSplineCurve::SetControlPointsList(const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList)
{
	controlPointsList = aControlPointsList;
}

Handle(StepGeom_HArray1OfCartesianPoint) StepGeom_BSplineCurve::ControlPointsList() const
{
	return controlPointsList;
}

Handle(StepGeom_CartesianPoint) StepGeom_BSplineCurve::ControlPointsListValue(const Standard_Integer num) const
{
	return controlPointsList->Value(num);
}

Standard_Integer StepGeom_BSplineCurve::NbControlPointsList () const
{
	if (controlPointsList.IsNull()) return 0;
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
