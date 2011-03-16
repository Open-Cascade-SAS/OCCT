#include <StepGeom_QuasiUniformCurveAndRationalBSplineCurve.ixx>

#include <StepGeom_QuasiUniformCurve.hxx>

#include <StepGeom_RationalBSplineCurve.hxx>


StepGeom_QuasiUniformCurveAndRationalBSplineCurve::StepGeom_QuasiUniformCurveAndRationalBSplineCurve ()  {}

void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect)
{

	StepGeom_BSplineCurve::Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}

void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect,
	const Handle(StepGeom_QuasiUniformCurve)& aQuasiUniformCurve,
	const Handle(StepGeom_RationalBSplineCurve)& aRationalBSplineCurve)
{
	// --- classe own fields ---
	quasiUniformCurve = aQuasiUniformCurve;
	rationalBSplineCurve = aRationalBSplineCurve;
	// --- classe inherited fields ---
	StepGeom_BSplineCurve::Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}


void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect,
	const Handle(TColStd_HArray1OfReal)& aWeightsData)
{
	// --- classe inherited fields ---

	StepGeom_BSplineCurve::Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);

	// --- ANDOR componant fields ---

	quasiUniformCurve = new StepGeom_QuasiUniformCurve();
	quasiUniformCurve->Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);

	// --- ANDOR componant fields ---

	rationalBSplineCurve = new StepGeom_RationalBSplineCurve();
	rationalBSplineCurve->Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect, aWeightsData);
}


void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::SetQuasiUniformCurve(const Handle(StepGeom_QuasiUniformCurve)& aQuasiUniformCurve)
{
	quasiUniformCurve = aQuasiUniformCurve;
}

Handle(StepGeom_QuasiUniformCurve) StepGeom_QuasiUniformCurveAndRationalBSplineCurve::QuasiUniformCurve() const
{
	return quasiUniformCurve;
}

void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::SetRationalBSplineCurve(const Handle(StepGeom_RationalBSplineCurve)& aRationalBSplineCurve)
{
	rationalBSplineCurve = aRationalBSplineCurve;
}

Handle(StepGeom_RationalBSplineCurve) StepGeom_QuasiUniformCurveAndRationalBSplineCurve::RationalBSplineCurve() const
{
	return rationalBSplineCurve;
}

	//--- Specific Methods for AND classe field access ---


	//--- Specific Methods for AND classe field access ---


void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::SetWeightsData(const Handle(TColStd_HArray1OfReal)& aWeightsData)
{
	rationalBSplineCurve->SetWeightsData(aWeightsData);
}

Handle(TColStd_HArray1OfReal) StepGeom_QuasiUniformCurveAndRationalBSplineCurve::WeightsData() const
{
	return rationalBSplineCurve->WeightsData();
}

Standard_Real StepGeom_QuasiUniformCurveAndRationalBSplineCurve::WeightsDataValue(const Standard_Integer num) const
{
	return rationalBSplineCurve->WeightsDataValue(num);
}

Standard_Integer StepGeom_QuasiUniformCurveAndRationalBSplineCurve::NbWeightsData () const
{
	return rationalBSplineCurve->NbWeightsData();
}
