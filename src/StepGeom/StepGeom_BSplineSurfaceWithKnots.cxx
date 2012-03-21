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

#include <StepGeom_BSplineSurfaceWithKnots.ixx>


StepGeom_BSplineSurfaceWithKnots::StepGeom_BSplineSurfaceWithKnots ()  {}

void StepGeom_BSplineSurfaceWithKnots::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aUDegree,
	const Standard_Integer aVDegree,
	const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineSurfaceForm aSurfaceForm,
	const StepData_Logical aUClosed,
	const StepData_Logical aVClosed,
	const StepData_Logical aSelfIntersect)
{

	StepGeom_BSplineSurface::Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect);
}

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
