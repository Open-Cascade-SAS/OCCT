#include <StepShape_VertexLoop.ixx>


StepShape_VertexLoop::StepShape_VertexLoop ()  {}

void StepShape_VertexLoop::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_VertexLoop::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_Vertex)& aLoopVertex)
{
	// --- classe own fields ---
	loopVertex = aLoopVertex;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_VertexLoop::SetLoopVertex(const Handle(StepShape_Vertex)& aLoopVertex)
{
	loopVertex = aLoopVertex;
}

Handle(StepShape_Vertex) StepShape_VertexLoop::LoopVertex() const
{
	return loopVertex;
}
