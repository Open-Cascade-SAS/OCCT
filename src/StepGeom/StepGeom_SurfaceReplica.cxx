#include <StepGeom_SurfaceReplica.ixx>


StepGeom_SurfaceReplica::StepGeom_SurfaceReplica ()  {}

void StepGeom_SurfaceReplica::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_SurfaceReplica::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aParentSurface,
	const Handle(StepGeom_CartesianTransformationOperator3d)& aTransformation)
{
	// --- classe own fields ---
	parentSurface = aParentSurface;
	transformation = aTransformation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_SurfaceReplica::SetParentSurface(const Handle(StepGeom_Surface)& aParentSurface)
{
	parentSurface = aParentSurface;
}

Handle(StepGeom_Surface) StepGeom_SurfaceReplica::ParentSurface() const
{
	return parentSurface;
}

void StepGeom_SurfaceReplica::SetTransformation(const Handle(StepGeom_CartesianTransformationOperator3d)& aTransformation)
{
	transformation = aTransformation;
}

Handle(StepGeom_CartesianTransformationOperator3d) StepGeom_SurfaceReplica::Transformation() const
{
	return transformation;
}
