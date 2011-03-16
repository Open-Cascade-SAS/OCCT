// File:	RWStepFEA_RWNodeRepresentation.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWNodeRepresentation.ixx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationItem.hxx>

//=======================================================================
//function : RWStepFEA_RWNodeRepresentation
//purpose  : 
//=======================================================================

RWStepFEA_RWNodeRepresentation::RWStepFEA_RWNodeRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                               const Standard_Integer num,
                                               Handle(Interface_Check)& ach,
                                               const Handle(StepFEA_NodeRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"node_representation") ) return;

  // Inherited fields of Representation

  Handle(TCollection_HAsciiString) aRepresentation_Name;
  data->ReadString (num, 1, "representation.name", ach, aRepresentation_Name);

  Handle(StepRepr_HArray1OfRepresentationItem) aRepresentation_Items;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "representation.items", ach, sub2) ) {
    Standard_Integer nb0 = data->NbParams(sub2);
    aRepresentation_Items = new StepRepr_HArray1OfRepresentationItem (1, nb0);
    Standard_Integer num2 = sub2;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepRepr_RepresentationItem) anIt0;
      data->ReadEntity (num2, i0, "representation_item", ach, STANDARD_TYPE(StepRepr_RepresentationItem), anIt0);
      aRepresentation_Items->SetValue(i0, anIt0);
    }
  }

  Handle(StepRepr_RepresentationContext) aRepresentation_ContextOfItems;
  data->ReadEntity (num, 3, "representation.context_of_items", ach, STANDARD_TYPE(StepRepr_RepresentationContext), aRepresentation_ContextOfItems);

  // Own fields of NodeRepresentation

  Handle(StepFEA_FeaModel) aModelRef;
  data->ReadEntity (num, 4, "model_ref", ach, STANDARD_TYPE(StepFEA_FeaModel), aModelRef);

  // Initialize entity
  ent->Init(aRepresentation_Name,
            aRepresentation_Items,
            aRepresentation_ContextOfItems,
            aModelRef);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeRepresentation::WriteStep (StepData_StepWriter& SW,
                                                const Handle(StepFEA_NodeRepresentation) &ent) const
{

  // Inherited fields of Representation

  SW.Send (ent->StepRepr_Representation::Name());

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->StepRepr_Representation::Items()->Length(); i1++ ) {
    Handle(StepRepr_RepresentationItem) Var0 = ent->StepRepr_Representation::Items()->Value(i1);
    SW.Send (Var0);
  }
  SW.CloseSub();

  SW.Send (ent->StepRepr_Representation::ContextOfItems());

  // Own fields of NodeRepresentation

  SW.Send (ent->ModelRef());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeRepresentation::Share (const Handle(StepFEA_NodeRepresentation) &ent,
                                            Interface_EntityIterator& iter) const
{

  // Inherited fields of Representation

  for (Standard_Integer i1=1; i1 <= ent->StepRepr_Representation::Items()->Length(); i1++ ) {
    Handle(StepRepr_RepresentationItem) Var0 = ent->StepRepr_Representation::Items()->Value(i1);
    iter.AddItem (Var0);
  }

  iter.AddItem (ent->StepRepr_Representation::ContextOfItems());

  // Own fields of NodeRepresentation

  iter.AddItem (ent->ModelRef());
}
