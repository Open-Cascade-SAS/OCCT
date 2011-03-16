#include <StepGeom_PointReplica.ixx>


StepGeom_PointReplica::StepGeom_PointReplica ()  {}

void StepGeom_PointReplica::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_PointReplica::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Point)& aParentPt,
	const Handle(StepGeom_CartesianTransformationOperator)& aTransformation)
{
	// --- classe own fields ---
	parentPt = aParentPt;
	transformation = aTransformation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_PointReplica::SetParentPt(const Handle(StepGeom_Point)& aParentPt)
{
	parentPt = aParentPt;
}

Handle(StepGeom_Point) StepGeom_PointReplica::ParentPt() const
{
	return parentPt;
}

void StepGeom_PointReplica::SetTransformation(const Handle(StepGeom_CartesianTransformationOperator)& aTransformation)
{
	transformation = aTransformation;
}

Handle(StepGeom_CartesianTransformationOperator) StepGeom_PointReplica::Transformation() const
{
	return transformation;
}
