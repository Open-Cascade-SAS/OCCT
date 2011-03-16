#include <StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface.ixx>

#include <StepGeom_QuasiUniformSurface.hxx>

#include <StepGeom_RationalBSplineSurface.hxx>


StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface ()  {}

void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::Init(
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

void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aUDegree,
	const Standard_Integer aVDegree,
	const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineSurfaceForm aSurfaceForm,
	const StepData_Logical aUClosed,
	const StepData_Logical aVClosed,
	const StepData_Logical aSelfIntersect,
	const Handle(StepGeom_QuasiUniformSurface)& aQuasiUniformSurface,
	const Handle(StepGeom_RationalBSplineSurface)& aRationalBSplineSurface)
{
	// --- classe own fields ---
	quasiUniformSurface = aQuasiUniformSurface;
	rationalBSplineSurface = aRationalBSplineSurface;
	// --- classe inherited fields ---
	StepGeom_BSplineSurface::Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect);
}


void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::Init(
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

	quasiUniformSurface = new StepGeom_QuasiUniformSurface();
	quasiUniformSurface->Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect);

	// --- ANDOR componant fields ---

	rationalBSplineSurface = new StepGeom_RationalBSplineSurface();
	rationalBSplineSurface->Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect, aWeightsData);
}


void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::SetQuasiUniformSurface(const Handle(StepGeom_QuasiUniformSurface)& aQuasiUniformSurface)
{
	quasiUniformSurface = aQuasiUniformSurface;
}

Handle(StepGeom_QuasiUniformSurface) StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::QuasiUniformSurface() const
{
	return quasiUniformSurface;
}

void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::SetRationalBSplineSurface(const Handle(StepGeom_RationalBSplineSurface)& aRationalBSplineSurface)
{
	rationalBSplineSurface = aRationalBSplineSurface;
}

Handle(StepGeom_RationalBSplineSurface) StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::RationalBSplineSurface() const
{
	return rationalBSplineSurface;
}

	//--- Specific Methods for AND classe field access ---


	//--- Specific Methods for AND classe field access ---


void StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::SetWeightsData(const Handle(TColStd_HArray2OfReal)& aWeightsData)
{
	rationalBSplineSurface->SetWeightsData(aWeightsData);
}

Handle(TColStd_HArray2OfReal) StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::WeightsData() const
{
	return rationalBSplineSurface->WeightsData();
}

Standard_Real StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::WeightsDataValue(const Standard_Integer num1,const Standard_Integer num2) const
{
	return rationalBSplineSurface->WeightsDataValue(num1,num2);
}

Standard_Integer StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::NbWeightsDataI () const
{
	return rationalBSplineSurface->NbWeightsDataI ();
}

Standard_Integer StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface::NbWeightsDataJ () const
{
	return rationalBSplineSurface->NbWeightsDataJ ();
}
