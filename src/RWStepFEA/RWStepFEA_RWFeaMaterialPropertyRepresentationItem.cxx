// File:	RWStepFEA_RWFeaMaterialPropertyRepresentationItem.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaMaterialPropertyRepresentationItem.ixx>

//=======================================================================
//function : RWStepFEA_RWFeaMaterialPropertyRepresentationItem
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaMaterialPropertyRepresentationItem::RWStepFEA_RWFeaMaterialPropertyRepresentationItem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaMaterialPropertyRepresentationItem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                  const Standard_Integer num,
                                                                  Handle(Interface_Check)& ach,
                                                                  const Handle(StepFEA_FeaMaterialPropertyRepresentationItem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"fea_material_property_representation_item") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Initialize entity
  ent->Init(aRepresentationItem_Name);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaMaterialPropertyRepresentationItem::WriteStep (StepData_StepWriter& SW,
                                                                   const Handle(StepFEA_FeaMaterialPropertyRepresentationItem) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaMaterialPropertyRepresentationItem::Share (const Handle(StepFEA_FeaMaterialPropertyRepresentationItem) &ent,
                                                               Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem
}
