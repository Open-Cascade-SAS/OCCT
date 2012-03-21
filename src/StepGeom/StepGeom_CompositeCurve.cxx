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

#include <StepGeom_CompositeCurve.ixx>


StepGeom_CompositeCurve::StepGeom_CompositeCurve ()  {}

void StepGeom_CompositeCurve::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_CompositeCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_HArray1OfCompositeCurveSegment)& aSegments,
	const StepData_Logical aSelfIntersect)
{
	// --- classe own fields ---
	segments = aSegments;
	selfIntersect = aSelfIntersect;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_CompositeCurve::SetSegments(const Handle(StepGeom_HArray1OfCompositeCurveSegment)& aSegments)
{
	segments = aSegments;
}

Handle(StepGeom_HArray1OfCompositeCurveSegment) StepGeom_CompositeCurve::Segments() const
{
	return segments;
}

Handle(StepGeom_CompositeCurveSegment) StepGeom_CompositeCurve::SegmentsValue(const Standard_Integer num) const
{
	return segments->Value(num);
}

Standard_Integer StepGeom_CompositeCurve::NbSegments () const
{
	if (segments.IsNull()) return 0;
	return segments->Length();
}

void StepGeom_CompositeCurve::SetSelfIntersect(const StepData_Logical aSelfIntersect)
{
	selfIntersect = aSelfIntersect;
}

StepData_Logical StepGeom_CompositeCurve::SelfIntersect() const
{
	return selfIntersect;
}
