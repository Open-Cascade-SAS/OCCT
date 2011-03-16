#include <StepShape_EdgeCurve.ixx>


StepShape_EdgeCurve::StepShape_EdgeCurve ()  {}

void StepShape_EdgeCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_Vertex)& aEdgeStart,
	const Handle(StepShape_Vertex)& aEdgeEnd)
{

	StepShape_Edge::Init(aName, aEdgeStart, aEdgeEnd);
}

void StepShape_EdgeCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_Vertex)& aEdgeStart,
	const Handle(StepShape_Vertex)& aEdgeEnd,
	const Handle(StepGeom_Curve)& aEdgeGeometry,
	const Standard_Boolean aSameSense)
{
	// --- classe own fields ---
	edgeGeometry = aEdgeGeometry;
	sameSense = aSameSense;
	// --- classe inherited fields ---
	StepShape_Edge::Init(aName, aEdgeStart, aEdgeEnd);
}


void StepShape_EdgeCurve::SetEdgeGeometry(const Handle(StepGeom_Curve)& aEdgeGeometry)
{
	edgeGeometry = aEdgeGeometry;
}

Handle(StepGeom_Curve) StepShape_EdgeCurve::EdgeGeometry() const
{
	return edgeGeometry;
}

void StepShape_EdgeCurve::SetSameSense(const Standard_Boolean aSameSense)
{
	sameSense = aSameSense;
}

Standard_Boolean StepShape_EdgeCurve::SameSense() const
{
	return sameSense;
}
