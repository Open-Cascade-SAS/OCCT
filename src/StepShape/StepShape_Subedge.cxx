// File:	StepShape_Subedge.cxx
// Created:	Fri Jan  4 17:42:45 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepShape_Subedge.ixx>

//=======================================================================
//function : StepShape_Subedge
//purpose  : 
//=======================================================================

StepShape_Subedge::StepShape_Subedge ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_Subedge::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                              const Handle(StepShape_Vertex) &aEdge_EdgeStart,
                              const Handle(StepShape_Vertex) &aEdge_EdgeEnd,
                              const Handle(StepShape_Edge) &aParentEdge)
{
  StepShape_Edge::Init(aRepresentationItem_Name,
                       aEdge_EdgeStart,
                       aEdge_EdgeEnd);

  theParentEdge = aParentEdge;
}

//=======================================================================
//function : ParentEdge
//purpose  : 
//=======================================================================

Handle(StepShape_Edge) StepShape_Subedge::ParentEdge () const
{
  return theParentEdge;
}

//=======================================================================
//function : SetParentEdge
//purpose  : 
//=======================================================================

void StepShape_Subedge::SetParentEdge (const Handle(StepShape_Edge) &aParentEdge)
{
  theParentEdge = aParentEdge;
}
