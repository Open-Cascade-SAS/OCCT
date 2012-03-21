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

#include <RWStepShape_RWSubedge.ixx>

//=======================================================================
//function : RWStepShape_RWSubedge
//purpose  : 
//=======================================================================

RWStepShape_RWSubedge::RWStepShape_RWSubedge ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWSubedge::ReadStep (const Handle(StepData_StepReaderData)& data,
                                      const Standard_Integer num,
                                      Handle(Interface_Check)& ach,
                                      const Handle(StepShape_Subedge) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"subedge") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of Edge

  Handle(StepShape_Vertex) aEdge_EdgeStart;
  data->ReadEntity (num, 2, "edge.edge_start", ach, STANDARD_TYPE(StepShape_Vertex), aEdge_EdgeStart);

  Handle(StepShape_Vertex) aEdge_EdgeEnd;
  data->ReadEntity (num, 3, "edge.edge_end", ach, STANDARD_TYPE(StepShape_Vertex), aEdge_EdgeEnd);

  // Own fields of Subedge

  Handle(StepShape_Edge) aParentEdge;
  data->ReadEntity (num, 4, "parent_edge", ach, STANDARD_TYPE(StepShape_Edge), aParentEdge);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aEdge_EdgeStart,
            aEdge_EdgeEnd,
            aParentEdge);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWSubedge::WriteStep (StepData_StepWriter& SW,
                                       const Handle(StepShape_Subedge) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Inherited fields of Edge

  SW.Send (ent->StepShape_Edge::EdgeStart());

  SW.Send (ent->StepShape_Edge::EdgeEnd());

  // Own fields of Subedge

  SW.Send (ent->ParentEdge());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWSubedge::Share (const Handle(StepShape_Subedge) &ent,
                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of Edge

  iter.AddItem (ent->StepShape_Edge::EdgeStart());

  iter.AddItem (ent->StepShape_Edge::EdgeEnd());

  // Own fields of Subedge

  iter.AddItem (ent->ParentEdge());
}
