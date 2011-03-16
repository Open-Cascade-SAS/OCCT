// File:	RWStepFEA_RWFeaMoistureAbsorption.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaMoistureAbsorption.ixx>

//=======================================================================
//function : RWStepFEA_RWFeaMoistureAbsorption
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaMoistureAbsorption::RWStepFEA_RWFeaMoistureAbsorption ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaMoistureAbsorption::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                  const Standard_Integer num,
                                                  Handle(Interface_Check)& ach,
                                                  const Handle(StepFEA_FeaMoistureAbsorption) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"fea_moisture_absorption") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of FeaMoistureAbsorption

  StepFEA_SymmetricTensor23d aFeaConstants;
  data->ReadEntity (num, 2, "fea_constants", ach, aFeaConstants);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aFeaConstants);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaMoistureAbsorption::WriteStep (StepData_StepWriter& SW,
                                                   const Handle(StepFEA_FeaMoistureAbsorption) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of FeaMoistureAbsorption

  SW.Send (ent->FeaConstants().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaMoistureAbsorption::Share (const Handle(StepFEA_FeaMoistureAbsorption) &ent,
                                               Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of FeaMoistureAbsorption

  iter.AddItem (ent->FeaConstants().Value());
}
