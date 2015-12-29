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
#include <StepGeom_Curve.hxx>
#include <StepGeom_PcurveOrSurface.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_SurfaceCurve,StepGeom_Curve)

StepGeom_SurfaceCurve::StepGeom_SurfaceCurve ()  {}

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
