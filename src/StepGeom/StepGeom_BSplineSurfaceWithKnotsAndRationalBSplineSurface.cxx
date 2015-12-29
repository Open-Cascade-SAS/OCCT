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
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_RationalBSplineSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface,StepGeom_BSplineSurface)

StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface::StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface ()  {}

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
