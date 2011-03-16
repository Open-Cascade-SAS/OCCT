// File:	RWStepFEA_RWNodeSet.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWNodeSet.ixx>
#include <StepFEA_HArray1OfNodeRepresentation.hxx>
#include <StepFEA_NodeRepresentation.hxx>

//=======================================================================
//function : RWStepFEA_RWNodeSet
//purpose  : 
//=======================================================================

RWStepFEA_RWNodeSet::RWStepFEA_RWNodeSet ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeSet::ReadStep (const Handle(StepData_StepReaderData)& data,
                                    const Standard_Integer num,
                                    Handle(Interface_Check)& ach,
                                    const Handle(StepFEA_NodeSet) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"node_set") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of NodeSet

  Handle(StepFEA_HArray1OfNodeRepresentation) aNodes;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "nodes", ach, sub2) ) {
    Standard_Integer nb0 = data->NbParams(sub2);
    aNodes = new StepFEA_HArray1OfNodeRepresentation (1, nb0);
    Standard_Integer num2 = sub2;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepFEA_NodeRepresentation) anIt0;
      data->ReadEntity (num2, i0, "node_representation", ach, STANDARD_TYPE(StepFEA_NodeRepresentation), anIt0);
      aNodes->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aNodes);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeSet::WriteStep (StepData_StepWriter& SW,
                                     const Handle(StepFEA_NodeSet) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of NodeSet

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->Nodes()->Length(); i1++ ) {
    Handle(StepFEA_NodeRepresentation) Var0 = ent->Nodes()->Value(i1);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeSet::Share (const Handle(StepFEA_NodeSet) &ent,
                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of NodeSet

  for (Standard_Integer i1=1; i1 <= ent->Nodes()->Length(); i1++ ) {
    Handle(StepFEA_NodeRepresentation) Var0 = ent->Nodes()->Value(i1);
    iter.AddItem (Var0);
  }
}
