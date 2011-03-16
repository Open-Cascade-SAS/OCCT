#include <StepShape_VertexPoint.ixx>


StepShape_VertexPoint::StepShape_VertexPoint ()  {}

void StepShape_VertexPoint::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_VertexPoint::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Point)& aVertexGeometry)
{
	// --- classe own fields ---
	vertexGeometry = aVertexGeometry;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_VertexPoint::SetVertexGeometry(const Handle(StepGeom_Point)& aVertexGeometry)
{
	vertexGeometry = aVertexGeometry;
}

Handle(StepGeom_Point) StepShape_VertexPoint::VertexGeometry() const
{
	return vertexGeometry;
}
