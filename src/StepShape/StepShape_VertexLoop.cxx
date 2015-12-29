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
#include <StepShape_Vertex.hxx>
#include <StepShape_VertexLoop.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_VertexLoop,StepShape_Loop)

StepShape_VertexLoop::StepShape_VertexLoop ()  {}

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
