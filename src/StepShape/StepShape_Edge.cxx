#include <StepShape_Edge.ixx>


StepShape_Edge::StepShape_Edge ()  {}

void StepShape_Edge::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_Edge::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_Vertex)& aEdgeStart,
	const Handle(StepShape_Vertex)& aEdgeEnd)
{
	// --- classe own fields ---
	edgeStart = aEdgeStart;
	edgeEnd = aEdgeEnd;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_Edge::SetEdgeStart(const Handle(StepShape_Vertex)& aEdgeStart)
{
	edgeStart = aEdgeStart;
}

Handle(StepShape_Vertex) StepShape_Edge::EdgeStart() const
{
	return edgeStart;
}

void StepShape_Edge::SetEdgeEnd(const Handle(StepShape_Vertex)& aEdgeEnd)
{
	edgeEnd = aEdgeEnd;
}

Handle(StepShape_Vertex) StepShape_Edge::EdgeEnd() const
{
	return edgeEnd;
}
