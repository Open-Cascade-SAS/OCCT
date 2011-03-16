#include <StepGeom_CurveReplica.ixx>


StepGeom_CurveReplica::StepGeom_CurveReplica ()  {}

void StepGeom_CurveReplica::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_CurveReplica::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aParentCurve,
	const Handle(StepGeom_CartesianTransformationOperator)& aTransformation)
{
	// --- classe own fields ---
	parentCurve = aParentCurve;
	transformation = aTransformation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_CurveReplica::SetParentCurve(const Handle(StepGeom_Curve)& aParentCurve)
{
	parentCurve = aParentCurve;
}

Handle(StepGeom_Curve) StepGeom_CurveReplica::ParentCurve() const
{
	return parentCurve;
}

void StepGeom_CurveReplica::SetTransformation(const Handle(StepGeom_CartesianTransformationOperator)& aTransformation)
{
	transformation = aTransformation;
}

Handle(StepGeom_CartesianTransformationOperator) StepGeom_CurveReplica::Transformation() const
{
	return transformation;
}
