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
#include <StepShape_Vertex.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_Edge,StepShape_TopologicalRepresentationItem)

StepShape_Edge::StepShape_Edge ()  {}

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
