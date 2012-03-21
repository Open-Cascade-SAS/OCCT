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

#include <StepGeom_SurfaceCurve.ixx>


StepGeom_SurfaceCurve::StepGeom_SurfaceCurve ()  {}

void StepGeom_SurfaceCurve::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_SurfaceCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aCurve3d,
	const Handle(StepGeom_HArray1OfPcurveOrSurface)& aAssociatedGeometry,
	const StepGeom_PreferredSurfaceCurveRepresentation aMasterRepresentation)
{
	// --- classe own fields ---
	curve3d = aCurve3d;
	associatedGeometry = aAssociatedGeometry;
	masterRepresentation = aMasterRepresentation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_SurfaceCurve::SetCurve3d(const Handle(StepGeom_Curve)& aCurve3d)
{
	curve3d = aCurve3d;
}

Handle(StepGeom_Curve) StepGeom_SurfaceCurve::Curve3d() const
{
	return curve3d;
}

void StepGeom_SurfaceCurve::SetAssociatedGeometry(const Handle(StepGeom_HArray1OfPcurveOrSurface)& aAssociatedGeometry)
{
	associatedGeometry = aAssociatedGeometry;
}

Handle(StepGeom_HArray1OfPcurveOrSurface) StepGeom_SurfaceCurve::AssociatedGeometry() const
{
	return associatedGeometry;
}

StepGeom_PcurveOrSurface StepGeom_SurfaceCurve::AssociatedGeometryValue(const Standard_Integer num) const
{
	return associatedGeometry->Value(num);
}

Standard_Integer StepGeom_SurfaceCurve::NbAssociatedGeometry () const
{
	return associatedGeometry->Length();
}

void StepGeom_SurfaceCurve::SetMasterRepresentation(const StepGeom_PreferredSurfaceCurveRepresentation aMasterRepresentation)
{
	masterRepresentation = aMasterRepresentation;
}

StepGeom_PreferredSurfaceCurveRepresentation StepGeom_SurfaceCurve::MasterRepresentation() const
{
	return masterRepresentation;
}
