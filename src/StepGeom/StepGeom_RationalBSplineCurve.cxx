#include <StepGeom_RationalBSplineCurve.ixx>


StepGeom_RationalBSplineCurve::StepGeom_RationalBSplineCurve ()  {}

void StepGeom_RationalBSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect)
{

	StepGeom_BSplineCurve::Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}

void StepGeom_RationalBSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect,
	const Handle(TColStd_HArray1OfReal)& aWeightsData)
{
	// --- classe own fields ---
	weightsData = aWeightsData;
	// --- classe inherited fields ---
	StepGeom_BSplineCurve::Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}


void StepGeom_RationalBSplineCurve::SetWeightsData(const Handle(TColStd_HArray1OfReal)& aWeightsData)
{
	weightsData = aWeightsData;
}

Handle(TColStd_HArray1OfReal) StepGeom_RationalBSplineCurve::WeightsData() const
{
	return weightsData;
}

Standard_Real StepGeom_RationalBSplineCurve::WeightsDataValue(const Standard_Integer num) const
{
	return weightsData->Value(num);
}

Standard_Integer StepGeom_RationalBSplineCurve::NbWeightsData () const
{
	return weightsData->Length();
}
