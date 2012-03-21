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

#include <StepShape_PolyLoop.ixx>


StepShape_PolyLoop::StepShape_PolyLoop ()  {}

void StepShape_PolyLoop::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_PolyLoop::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aPolygon)
{
	// --- classe own fields ---
	polygon = aPolygon;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_PolyLoop::SetPolygon(const Handle(StepGeom_HArray1OfCartesianPoint)& aPolygon)
{
	polygon = aPolygon;
}

Handle(StepGeom_HArray1OfCartesianPoint) StepShape_PolyLoop::Polygon() const
{
	return polygon;
}

Handle(StepGeom_CartesianPoint) StepShape_PolyLoop::PolygonValue(const Standard_Integer num) const
{
	return polygon->Value(num);
}

Standard_Integer StepShape_PolyLoop::NbPolygon () const
{
	if (polygon.IsNull()) return 0;
	return polygon->Length();
}
