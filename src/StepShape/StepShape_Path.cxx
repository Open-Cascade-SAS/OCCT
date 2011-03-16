#include <StepShape_Path.ixx>


StepShape_Path::StepShape_Path ()  {}

void StepShape_Path::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_Path::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList)
{
	// --- classe own fields ---
	edgeList = aEdgeList;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_Path::SetEdgeList(const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList)
{
	edgeList = aEdgeList;
}

Handle(StepShape_HArray1OfOrientedEdge) StepShape_Path::EdgeList() const
{
	return edgeList;
}

Handle(StepShape_OrientedEdge) StepShape_Path::EdgeListValue(const Standard_Integer num) const
{
	return edgeList->Value(num);
}

Standard_Integer StepShape_Path::NbEdgeList () const
{
	if (edgeList.IsNull()) return 0;
	return edgeList->Length();
}
