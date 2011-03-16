// File:	RWStepShape_RWConnectedEdgeSet.cxx
// Created:	Fri Dec 28 16:02:00 2001 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepShape_RWConnectedEdgeSet.ixx>
#include <StepShape_HArray1OfEdge.hxx>
#include <StepShape_Edge.hxx>

//=======================================================================
//function : RWStepShape_RWConnectedEdgeSet
//purpose  : 
//=======================================================================

RWStepShape_RWConnectedEdgeSet::RWStepShape_RWConnectedEdgeSet ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWConnectedEdgeSet::ReadStep (const Handle(StepData_StepReaderData)& data,
                                               const Standard_Integer num,
                                               Handle(Interface_Check)& ach,
                                               const Handle(StepShape_ConnectedEdgeSet) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"connected_edge_set") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of ConnectedEdgeSet

  Handle(StepShape_HArray1OfEdge) aCesEdges;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "ces_edges", ach, sub2) ) {
    Standard_Integer num2 = sub2;
    Standard_Integer nb0 = data->NbParams(num2);
    aCesEdges = new StepShape_HArray1OfEdge (1, nb0);
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepShape_Edge) anIt0;
      data->ReadEntity (num2, i0, "ces_edges", ach, STANDARD_TYPE(StepShape_Edge), anIt0);
      aCesEdges->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aCesEdges);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWConnectedEdgeSet::WriteStep (StepData_StepWriter& SW,
                                                const Handle(StepShape_ConnectedEdgeSet) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of ConnectedEdgeSet

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->CesEdges()->Length(); i1++ ) {
    Handle(StepShape_Edge) Var0 = ent->CesEdges()->Value(i1);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWConnectedEdgeSet::Share (const Handle(StepShape_ConnectedEdgeSet) &ent,
                                            Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of ConnectedEdgeSet

  for (Standard_Integer i1=1; i1 <= ent->CesEdges()->Length(); i1++ ) {
    Handle(StepShape_Edge) Var0 = ent->CesEdges()->Value(i1);
    iter.AddItem (Var0);
  }
}
