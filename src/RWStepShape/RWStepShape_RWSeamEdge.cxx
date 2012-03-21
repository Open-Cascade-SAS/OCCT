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

#include <RWStepShape_RWSeamEdge.ixx>

//=======================================================================
//function : RWStepShape_RWSeamEdge
//purpose  : 
//=======================================================================

RWStepShape_RWSeamEdge::RWStepShape_RWSeamEdge ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWSeamEdge::ReadStep (const Handle(StepData_StepReaderData)& data,
                                       const Standard_Integer num,
                                       Handle(Interface_Check)& ach,
                                       const Handle(StepShape_SeamEdge) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,6,ach,"seam_edge") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of Edge

  Handle(StepShape_Vertex) aEdge_EdgeStart;
  data->CheckDerived(num, 2, "edge.edge_start", ach,Standard_False);

  Handle(StepShape_Vertex) aEdge_EdgeEnd;
  data->CheckDerived (num, 3, "edge.edge_end", ach, Standard_False);

  // Inherited fields of OrientedEdge

  Handle(StepShape_Edge) aOrientedEdge_EdgeElement;
  data->ReadEntity (num, 4, "oriented_edge.edge_element", ach, STANDARD_TYPE(StepShape_Edge), aOrientedEdge_EdgeElement);

  Standard_Boolean aOrientedEdge_Orientation;
  data->ReadBoolean (num, 5, "oriented_edge.orientation", ach, aOrientedEdge_Orientation);

  // Own fields of SeamEdge

  Handle(StepGeom_Pcurve) aPcurveReference;
  data->ReadEntity (num, 6, "pcurve_reference", ach, STANDARD_TYPE(StepGeom_Pcurve), aPcurveReference);

  // Initialize entity
 
  ent->Init(aRepresentationItem_Name,
            aOrientedEdge_EdgeElement,
            aOrientedEdge_Orientation,
            aPcurveReference);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWSeamEdge::WriteStep (StepData_StepWriter& SW,
                                        const Handle(StepShape_SeamEdge) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Inherited fields of Edge

  SW.Send (ent->StepShape_Edge::EdgeStart());

  SW.Send (ent->StepShape_Edge::EdgeEnd());

  // Inherited fields of OrientedEdge

  SW.Send (ent->StepShape_OrientedEdge::EdgeElement());

  SW.SendBoolean (ent->StepShape_OrientedEdge::Orientation());

  // Own fields of SeamEdge

  SW.Send (ent->PcurveReference());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWSeamEdge::Share (const Handle(StepShape_SeamEdge) &ent,
                                    Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of Edge

  iter.AddItem (ent->StepShape_Edge::EdgeStart());

  iter.AddItem (ent->StepShape_Edge::EdgeEnd());

  // Inherited fields of OrientedEdge

  iter.AddItem (ent->StepShape_OrientedEdge::EdgeElement());

  // Own fields of SeamEdge

  iter.AddItem (ent->PcurveReference());
}
