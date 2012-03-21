// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepShape_OrientedEdge.ixx>


StepShape_OrientedEdge::StepShape_OrientedEdge ()  {}


void StepShape_OrientedEdge::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_Edge)& aEdgeElement,
	const Standard_Boolean aOrientation)
{
	// --- classe own fields ---
	edgeElement = aEdgeElement;
	orientation = aOrientation;
	// --- classe inherited fields ---
	Handle(StepShape_Vertex) aEdgeStart;
	aEdgeStart.Nullify();
	Handle(StepShape_Vertex) aEdgeEnd;
	aEdgeEnd.Nullify();
	StepShape_Edge::Init(aName, aEdgeStart, aEdgeEnd);
}


void StepShape_OrientedEdge::SetEdgeElement(const Handle(StepShape_Edge)& aEdgeElement)
{
	edgeElement = aEdgeElement;
}

Handle(StepShape_Edge) StepShape_OrientedEdge::EdgeElement() const
{
	return edgeElement;
}

void StepShape_OrientedEdge::SetOrientation(const Standard_Boolean aOrientation)
{
	orientation = aOrientation;
}

Standard_Boolean StepShape_OrientedEdge::Orientation() const
{
	return orientation;
}

void StepShape_OrientedEdge::SetEdgeStart(const Handle(StepShape_Vertex)& /*aEdgeStart*/)
{
	// WARNING : the field is redefined.
	// field set up forbidden.
	cout << "Field is redefined, SetUp Forbidden" << endl;
}

Handle(StepShape_Vertex) StepShape_OrientedEdge::EdgeStart() const
{
	// WARNING : the field is redefined.
	// method body is not yet automaticly wrote
  if (Orientation()) {
    return edgeElement->EdgeStart();
  }
  else {
    return edgeElement->EdgeEnd();
  }
}

void StepShape_OrientedEdge::SetEdgeEnd(const Handle(StepShape_Vertex)& /*aEdgeEnd*/)
{
	// WARNING : the field is redefined.
	// field set up forbidden.
	cout << "Field is redefined, SetUp Forbidden" << endl;
}

Handle(StepShape_Vertex) StepShape_OrientedEdge::EdgeEnd() const
{
	// WARNING : the field is redefined.
	// method body is not yet automaticly wrote

  if (Orientation()) {
    return edgeElement->EdgeEnd();
  }
  else {
    return edgeElement->EdgeStart();
  }
}
