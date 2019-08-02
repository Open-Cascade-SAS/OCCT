// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Standard_Type.hxx>
#include <StepShape_Edge.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_Vertex.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_OrientedEdge,StepShape_Edge)

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
	std::cout << "Field is redefined, SetUp Forbidden" << std::endl;
}

Handle(StepShape_Vertex) StepShape_OrientedEdge::EdgeStart() const
{
	// WARNING : the field is redefined.
	// method body is not yet automaticly wrote
  if (edgeElement.IsNull())
    return NULL;
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
	std::cout << "Field is redefined, SetUp Forbidden" << std::endl;
}

Handle(StepShape_Vertex) StepShape_OrientedEdge::EdgeEnd() const
{
	// WARNING : the field is redefined.
	// method body is not yet automaticly wrote
  if (edgeElement.IsNull())
    return NULL;
  if (Orientation()) {
    return edgeElement->EdgeEnd();
  }
  else {
    return edgeElement->EdgeStart();
  }
}
