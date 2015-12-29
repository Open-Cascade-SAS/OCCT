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
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_BSplineSurfaceWithKnots,StepGeom_BSplineSurface)

StepGeom_BSplineSurfaceWithKnots::StepGeom_BSplineSurfaceWithKnots ()  {}

void StepGeom_BSplineSurfaceWithKnots::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aUDegree,
	const Standard_Integer aVDegree,
	const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineSurfaceForm aSurfaceForm,
	const StepData_Logical aUClosed,
	const StepData_Logical aVClosed,
	const StepData_Logical aSelfIntersect,
	const Handle(TColStd_HArray1OfInteger)& aUMultiplicities,
	const Handle(TColStd_HArray1OfInteger)& aVMultiplicities,
	const Handle(TColStd_HArray1OfReal)& aUKnots,
	const Handle(TColStd_HArray1OfReal)& aVKnots,
	const StepGeom_KnotType aKnotSpec)
{
	// --- classe own fields ---
	uMultiplicities = aUMultiplicities;
	vMultiplicities = aVMultiplicities;
	uKnots = aUKnots;
	vKnots = aVKnots;
	knotSpec = aKnotSpec;
	// --- classe inherited fields ---
	StepGeom_BSplineSurface::Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect);
}


void StepGeom_BSplineSurfaceWithKnots::SetUMultiplicities(const Handle(TColStd_HArray1OfInteger)& aUMultiplicities)
{
	uMultiplicities = aUMultiplicities;
}

Handle(TColStd_HArray1OfInteger) StepGeom_BSplineSurfaceWithKnots::UMultiplicities() const
{
	return uMultiplicities;
}

Standard_Integer StepGeom_BSplineSurfaceWithKnots::UMultiplicitiesValue(const Standard_Integer num) const
{
	return uMultiplicities->Value(num);
}

Standard_Integer StepGeom_BSplineSurfaceWithKnots::NbUMultiplicities () const
{
	return uMultiplicities->Length();
}

void StepGeom_BSplineSurfaceWithKnots::SetVMultiplicities(const Handle(TColStd_HArray1OfInteger)& aVMultiplicities)
{
	vMultiplicities = aVMultiplicities;
}

Handle(TColStd_HArray1OfInteger) StepGeom_BSplineSurfaceWithKnots::VMultiplicities() const
{
	return vMultiplicities;
}

Standard_Integer StepGeom_BSplineSurfaceWithKnots::VMultiplicitiesValue(const Standard_Integer num) const
{
	return vMultiplicities->Value(num);
}

Standard_Integer StepGeom_BSplineSurfaceWithKnots::NbVMultiplicities () const
{
	return vMultiplicities->Length();
}

void StepGeom_BSplineSurfaceWithKnots::SetUKnots(const Handle(TColStd_HArray1OfReal)& aUKnots)
{
	uKnots = aUKnots;
}

Handle(TColStd_HArray1OfReal) StepGeom_BSplineSurfaceWithKnots::UKnots() const
{
	return uKnots;
}

Standard_Real StepGeom_BSplineSurfaceWithKnots::UKnotsValue(const Standard_Integer num) const
{
	return uKnots->Value(num);
}

Standard_Integer StepGeom_BSplineSurfaceWithKnots::NbUKnots () const
{
	return uKnots->Length();
}

void StepGeom_BSplineSurfaceWithKnots::SetVKnots(const Handle(TColStd_HArray1OfReal)& aVKnots)
{
	vKnots = aVKnots;
}

Handle(TColStd_HArray1OfReal) StepGeom_BSplineSurfaceWithKnots::VKnots() const
{
	return vKnots;
}

Standard_Real StepGeom_BSplineSurfaceWithKnots::VKnotsValue(const Standard_Integer num) const
{
	return vKnots->Value(num);
}

Standard_Integer StepGeom_BSplineSurfaceWithKnots::NbVKnots () const
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
