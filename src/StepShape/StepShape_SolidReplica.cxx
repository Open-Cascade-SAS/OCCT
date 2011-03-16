#include <StepShape_SolidReplica.ixx>


StepShape_SolidReplica::StepShape_SolidReplica ()  {}

void StepShape_SolidReplica::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_SolidReplica::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_SolidModel)& aParentSolid,
	const Handle(StepGeom_CartesianTransformationOperator3d)& aTransformation)
{
	// --- classe own fields ---
	parentSolid = aParentSolid;
	transformation = aTransformation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_SolidReplica::SetParentSolid(const Handle(StepShape_SolidModel)& aParentSolid)
{
	parentSolid = aParentSolid;
}

Handle(StepShape_SolidModel) StepShape_SolidReplica::ParentSolid() const
{
	return parentSolid;
}

void StepShape_SolidReplica::SetTransformation(const Handle(StepGeom_CartesianTransformationOperator3d)& aTransformation)
{
	transformation = aTransformation;
}

Handle(StepGeom_CartesianTransformationOperator3d) StepShape_SolidReplica::Transformation() const
{
	return transformation;
}
