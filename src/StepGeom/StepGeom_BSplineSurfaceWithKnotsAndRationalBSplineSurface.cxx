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

#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.ixx>

#include <StepGeom_BSplineSurfaceWithKnots.hxx>

#include <StepGeom_RationalBSplineSurface.hxx>


StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface ()  {}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::Init(
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

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aUDegree,
	const Standard_Integer aVDegree,
	const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineSurfaceForm aSurfaceForm,
	const StepData_Logical aUClosed,
	const StepData_Logical aVClosed,
	const StepData_Logical aSelfIntersect,
	const Handle(StepGeom_BSplineSurfaceWithKnots)& aBSplineSurfaceWithKnots,
	const Handle(StepGeom_RationalBSplineSurface)& aRationalBSplineSurface)
{
	// --- classe own fields ---
	bSplineSurfaceWithKnots = aBSplineSurfaceWithKnots;
	rationalBSplineSurface = aRationalBSplineSurface;
	// --- classe inherited fields ---
	StepGeom_BSplineSurface::Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect);
}


void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::Init(
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
	const StepGeom_KnotType aKnotSpec,
	const Handle(TColStd_HArray2OfReal)& aWeightsData)
{
	// --- classe inherited fields ---

	StepGeom_BSplineSurface::Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect);

	// --- ANDOR componant fields ---

	bSplineSurfaceWithKnots = new StepGeom_BSplineSurfaceWithKnots();
	bSplineSurfaceWithKnots->Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect, aUMultiplicities, aVMultiplicities, aUKnots, aVKnots, aKnotSpec);

	// --- ANDOR componant fields ---

	rationalBSplineSurface = new StepGeom_RationalBSplineSurface();
	rationalBSplineSurface->Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect, aWeightsData);
}


void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetBSplineSurfaceWithKnots(const Handle(StepGeom_BSplineSurfaceWithKnots)& aBSplineSurfaceWithKnots)
{
	bSplineSurfaceWithKnots = aBSplineSurfaceWithKnots;
}

Handle(StepGeom_BSplineSurfaceWithKnots) StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::BSplineSurfaceWithKnots() const
{
	return bSplineSurfaceWithKnots;
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetRationalBSplineSurface(const Handle(StepGeom_RationalBSplineSurface)& aRationalBSplineSurface)
{
	rationalBSplineSurface = aRationalBSplineSurface;
}

Handle(StepGeom_RationalBSplineSurface) StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::RationalBSplineSurface() const
{
	return rationalBSplineSurface;
}

	//--- Specific Methods for AND classe field access ---


void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetUMultiplicities(const Handle(TColStd_HArray1OfInteger)& aUMultiplicities)
{
	bSplineSurfaceWithKnots->SetUMultiplicities(aUMultiplicities);
}

Handle(TColStd_HArray1OfInteger) StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::UMultiplicities() const
{
	return bSplineSurfaceWithKnots->UMultiplicities();
}

Standard_Integer StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::UMultiplicitiesValue(const Standard_Integer num) const
{
	return bSplineSurfaceWithKnots->UMultiplicitiesValue(num);
}

Standard_Integer StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbUMultiplicities () const
{
	return bSplineSurfaceWithKnots->NbUMultiplicities();
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetVMultiplicities(const Handle(TColStd_HArray1OfInteger)& aVMultiplicities)
{
	bSplineSurfaceWithKnots->SetVMultiplicities(aVMultiplicities);
}

Handle(TColStd_HArray1OfInteger) StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::VMultiplicities() const
{
	return bSplineSurfaceWithKnots->VMultiplicities();
}

Standard_Integer StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::VMultiplicitiesValue(const Standard_Integer num) const
{
	return bSplineSurfaceWithKnots->VMultiplicitiesValue(num);
}

Standard_Integer StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbVMultiplicities () const
{
	return bSplineSurfaceWithKnots->NbVMultiplicities();
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetUKnots(const Handle(TColStd_HArray1OfReal)& aUKnots)
{
	bSplineSurfaceWithKnots->SetUKnots(aUKnots);
}

Handle(TColStd_HArray1OfReal) StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::UKnots() const
{
	return bSplineSurfaceWithKnots->UKnots();
}

Standard_Real StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::UKnotsValue(const Standard_Integer num) const
{
	return bSplineSurfaceWithKnots->UKnotsValue(num);
}

Standard_Integer StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbUKnots () const
{
	return bSplineSurfaceWithKnots->NbUKnots();
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetVKnots(const Handle(TColStd_HArray1OfReal)& aVKnots)
{
	bSplineSurfaceWithKnots->SetVKnots(aVKnots);
}

Handle(TColStd_HArray1OfReal) StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::VKnots() const
{
	return bSplineSurfaceWithKnots->VKnots();
}

Standard_Real StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::VKnotsValue(const Standard_Integer num) const
{
	return bSplineSurfaceWithKnots->VKnotsValue(num);
}

Standard_Integer StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbVKnots () const
{
	return bSplineSurfaceWithKnots->NbVKnots();
}

void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetKnotSpec(const StepGeom_KnotType aKnotSpec)
{
	bSplineSurfaceWithKnots->SetKnotSpec(aKnotSpec);
}

StepGeom_KnotType StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::KnotSpec() const
{
	return bSplineSurfaceWithKnots->KnotSpec();
}

	//--- Specific Methods for AND classe field access ---


void StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::SetWeightsData(const Handle(TColStd_HArray2OfReal)& aWeightsData)
{
	rationalBSplineSurface->SetWeightsData(aWeightsData);
}

Handle(TColStd_HArray2OfReal) StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::WeightsData() const
{
	return rationalBSplineSurface->WeightsData();
}

Standard_Real StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::WeightsDataValue(const Standard_Integer num1,const Standard_Integer num2) const
{
	return rationalBSplineSurface->WeightsDataValue(num1,num2);
}

Standard_Integer StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbWeightsDataI () const
{
	return rationalBSplineSurface->NbWeightsDataI ();
}

Standard_Integer StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::NbWeightsDataJ () const
{
	return rationalBSplineSurface->NbWeightsDataJ ();
}
