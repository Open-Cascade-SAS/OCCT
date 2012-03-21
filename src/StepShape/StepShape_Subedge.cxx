// Created on: 2002-01-04
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

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
