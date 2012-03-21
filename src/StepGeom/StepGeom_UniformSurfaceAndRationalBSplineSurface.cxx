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

#include <StepGeom_UniformSurfaceAndRationalBSplineSurface.ixx>

#include <StepGeom_UniformSurface.hxx>

#include <StepGeom_RationalBSplineSurface.hxx>


StepGeom_UniformSurfaceAndRationalBSplineSurface::StepGeom_UniformSurfaceAndRationalBSplineSurface ()  {}

void StepGeom_UniformSurfaceAndRationalBSplineSurface::Init(
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

void StepGeom_UniformSurfaceAndRationalBSplineSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aUDegree,
	const Standard_Integer aVDegree,
	const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineSurfaceForm aSurfaceForm,
	const StepData_Logical aUClosed,
	const StepData_Logical aVClosed,
	const StepData_Logical aSelfIntersect,
	const Handle(StepGeom_UniformSurface)& aUniformSurface,
	const Handle(StepGeom_RationalBSplineSurface)& aRationalBSplineSurface)
{
	// --- classe own fields ---
	uniformSurface = aUniformSurface;
	rationalBSplineSurface = aRationalBSplineSurface;
	// --- classe inherited fields ---
	StepGeom_BSplineSurface::Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect);
}


void StepGeom_UniformSurfaceAndRationalBSplineSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aUDegree,
	const Standard_Integer aVDegree,
	const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineSurfaceForm aSurfaceForm,
	const StepData_Logical aUClosed,
	const StepData_Logical aVClosed,
	const StepData_Logical aSelfIntersect,
	const Handle(TColStd_HArray2OfReal)& aWeightsData)
{
	// --- classe inherited fields ---

	StepGeom_BSplineSurface::Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect);

	// --- ANDOR componant fields ---

	rationalBSplineSurface = new StepGeom_RationalBSplineSurface();
	rationalBSplineSurface->Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect, aWeightsData);

	// --- ANDOR componant fields ---

	uniformSurface = new StepGeom_UniformSurface();
	uniformSurface->Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect);
}


void StepGeom_UniformSurfaceAndRationalBSplineSurface::SetUniformSurface(const Handle(StepGeom_UniformSurface)& aUniformSurface)
{
	uniformSurface = aUniformSurface;
}

Handle(StepGeom_UniformSurface) StepGeom_UniformSurfaceAndRationalBSplineSurface::UniformSurface() const
{
	return uniformSurface;
}

void StepGeom_UniformSurfaceAndRationalBSplineSurface::SetRationalBSplineSurface(const Handle(StepGeom_RationalBSplineSurface)& aRationalBSplineSurface)
{
	rationalBSplineSurface = aRationalBSplineSurface;
}

Handle(StepGeom_RationalBSplineSurface) StepGeom_UniformSurfaceAndRationalBSplineSurface::RationalBSplineSurface() const
{
	return rationalBSplineSurface;
}

	//--- Specific Methods for AND classe field access ---


void StepGeom_UniformSurfaceAndRationalBSplineSurface::SetWeightsData(const Handle(TColStd_HArray2OfReal)& aWeightsData)
{
	rationalBSplineSurface->SetWeightsData(aWeightsData);
}

Handle(TColStd_HArray2OfReal) StepGeom_UniformSurfaceAndRationalBSplineSurface::WeightsData() const
{
	return rationalBSplineSurface->WeightsData();
}

Standard_Real StepGeom_UniformSurfaceAndRationalBSplineSurface::WeightsDataValue(const Standard_Integer num1,const Standard_Integer num2) const
{
	return rationalBSplineSurface->WeightsDataValue(num1,num2);
}

Standard_Integer StepGeom_UniformSurfaceAndRationalBSplineSurface::NbWeightsDataI () const
{
	return rationalBSplineSurface->NbWeightsDataI ();
}

Standard_Integer StepGeom_UniformSurfaceAndRationalBSplineSurface::NbWeightsDataJ () const
{
	return rationalBSplineSurface->NbWeightsDataJ ();
}

	//--- Specific Methods for AND classe field access ---

