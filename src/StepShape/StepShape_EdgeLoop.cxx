#include <StepShape_EdgeLoop.ixx>


StepShape_EdgeLoop::StepShape_EdgeLoop ()  {}

void StepShape_EdgeLoop::Init(const Handle(TCollection_HAsciiString)& aName)
{
  StepRepr_RepresentationItem::Init(aName);
}

void StepShape_EdgeLoop::Init
(const Handle(TCollection_HAsciiString)& aName,
 const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList)
{
  // --- class inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
  // --- class own fields ---
  edgeList = aEdgeList;
}


void StepShape_EdgeLoop::SetEdgeList(const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList)
{
	edgeList = aEdgeList;
}

Handle(StepShape_HArray1OfOrientedEdge) StepShape_EdgeLoop::EdgeList() const
{
	return edgeList;
}

Handle(StepShape_OrientedEdge) StepShape_EdgeLoop::EdgeListValue(const Standard_Integer num) const
{
	return edgeList->Value(num);
}

Standard_Integer StepShape_EdgeLoop::NbEdgeList () const
{
	if (edgeList.IsNull()) return 0;
	return edgeList->Length();
}
