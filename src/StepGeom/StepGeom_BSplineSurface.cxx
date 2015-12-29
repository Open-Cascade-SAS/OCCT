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
#include <StepGeom_BSplineSurface.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_BSplineSurface,StepGeom_BoundedSurface)

StepGeom_BSplineSurface::StepGeom_BSplineSurface ()  {}

void StepGeom_BSplineSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aUDegree,
	const Standard_Integer aVDegree,
	const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineSurfaceForm aSurfaceForm,
	const StepData_Logical aUClosed,
	const StepData_Logical aVClosed,
	const StepData_Logical aSelfIntersect)
{
	// --- classe own fields ---
	uDegree = aUDegree;
	vDegree = aVDegree;
	controlPointsList = aControlPointsList;
	surfaceForm = aSurfaceForm;
	uClosed = aUClosed;
	vClosed = aVClosed;
	selfIntersect = aSelfIntersect;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_BSplineSurface::SetUDegree(const Standard_Integer aUDegree)
{
	uDegree = aUDegree;
}

Standard_Integer StepGeom_BSplineSurface::UDegree() const
{
	return uDegree;
}

void StepGeom_BSplineSurface::SetVDegree(const Standard_Integer aVDegree)
{
	vDegree = aVDegree;
}

Standard_Integer StepGeom_BSplineSurface::VDegree() const
{
	return vDegree;
}

void StepGeom_BSplineSurface::SetControlPointsList(const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList)
{
	controlPointsList = aControlPointsList;
}

Handle(StepGeom_HArray2OfCartesianPoint) StepGeom_BSplineSurface::ControlPointsList() const
{
	return controlPointsList;
}

Handle(StepGeom_CartesianPoint) StepGeom_BSplineSurface::ControlPointsListValue(const Standard_Integer num1,const Standard_Integer num2) const
{
	return controlPointsList->Value(num1,num2);
}

Standard_Integer StepGeom_BSplineSurface::NbControlPointsListI () const
{
	if (controlPointsList.IsNull()) return 0;
	return controlPointsList->UpperRow() - controlPointsList->LowerRow() + 1;
}

Standard_Integer StepGeom_BSplineSurface::NbControlPointsListJ () const
{
	if (controlPointsList.IsNull()) return 0;
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
