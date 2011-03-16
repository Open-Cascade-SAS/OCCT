// File:	RWStepFEA_RWFeaRepresentationItem.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaRepresentationItem.ixx>

//=======================================================================
//function : RWStepFEA_RWFeaRepresentationItem
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaRepresentationItem::RWStepFEA_RWFeaRepresentationItem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaRepresentationItem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                  const Standard_Integer num,
                                                  Handle(Interface_Check)& ach,
                                                  const Handle(StepFEA_FeaRepresentationItem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"fea_representation_item") ) return;

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

void RWStepFEA_RWFeaRepresentationItem::WriteStep (StepData_StepWriter& SW,
                                                   const Handle(StepFEA_FeaRepresentationItem) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaRepresentationItem::Share (const Handle(StepFEA_FeaRepresentationItem) &ent,
                                               Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem
}
