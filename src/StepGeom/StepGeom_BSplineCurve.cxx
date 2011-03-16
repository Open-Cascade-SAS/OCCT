#include <StepGeom_BSplineCurve.ixx>


StepGeom_BSplineCurve::StepGeom_BSplineCurve ()  {}

void StepGeom_BSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_BSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect)
{
	// --- classe own fields ---
	degree = aDegree;
	controlPointsList = aControlPointsList;
	curveForm = aCurveForm;
	closedCurve = aClosedCurve;
	selfIntersect = aSelfIntersect;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_BSplineCurve::SetDegree(const Standard_Integer aDegree)
{
	degree = aDegree;
}

Standard_Integer StepGeom_BSplineCurve::Degree() const
{
	return degree;
}

void StepGeom_BSplineCurve::SetControlPointsList(const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList)
{
	controlPointsList = aControlPointsList;
}

Handle(StepGeom_HArray1OfCartesianPoint) StepGeom_BSplineCurve::ControlPointsList() const
{
	return controlPointsList;
}

Handle(StepGeom_CartesianPoint) StepGeom_BSplineCurve::ControlPointsListValue(const Standard_Integer num) const
{
	return controlPointsList->Value(num);
}

Standard_Integer StepGeom_BSplineCurve::NbControlPointsList () const
{
	if (controlPointsList.IsNull()) return 0;
	return controlPointsList->Length();
}

void StepGeom_BSplineCurve::SetCurveForm(const StepGeom_BSplineCurveForm aCurveForm)
{
	curveForm = aCurveForm;
}

StepGeom_BSplineCurveForm StepGeom_BSplineCurve::CurveForm() const
{
	return curveForm;
}

void StepGeom_BSplineCurve::SetClosedCurve(const StepData_Logical aClosedCurve)
{
	closedCurve = aClosedCurve;
}

StepData_Logical StepGeom_BSplineCurve::ClosedCurve() const
{
	return closedCurve;
}

void StepGeom_BSplineCurve::SetSelfIntersect(const StepData_Logical aSelfIntersect)
{
	selfIntersect = aSelfIntersect;
}

StepData_Logical StepGeom_BSplineCurve::SelfIntersect() const
{
	return selfIntersect;
}
