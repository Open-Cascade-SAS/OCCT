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

#include <StepGeom_RectangularCompositeSurface.ixx>


StepGeom_RectangularCompositeSurface::StepGeom_RectangularCompositeSurface ()  {}

void StepGeom_RectangularCompositeSurface::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_RectangularCompositeSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_HArray2OfSurfacePatch)& aSegments)
{
	// --- classe own fields ---
	segments = aSegments;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_RectangularCompositeSurface::SetSegments(const Handle(StepGeom_HArray2OfSurfacePatch)& aSegments)
{
	segments = aSegments;
}

Handle(StepGeom_HArray2OfSurfacePatch) StepGeom_RectangularCompositeSurface::Segments() const
{
	return segments;
}

Handle(StepGeom_SurfacePatch) StepGeom_RectangularCompositeSurface::SegmentsValue(const Standard_Integer num1,const Standard_Integer num2) const
{
	return segments->Value(num1,num2);
}

Standard_Integer StepGeom_RectangularCompositeSurface::NbSegmentsI () const
{
	if (segments.IsNull()) return 0;
	return segments->UpperRow() - segments->LowerRow() + 1;
}

Standard_Integer StepGeom_RectangularCompositeSurface::NbSegmentsJ () const
{
	if (segments.IsNull()) return 0;
	return segments->UpperCol() - segments->LowerCol() + 1;
}
