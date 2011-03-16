// File:	RWStepBasic_RWExternallyDefinedItem.cxx
// Created:	Wed May 10 15:09:07 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWExternallyDefinedItem.ixx>

//=======================================================================
//function : RWStepBasic_RWExternallyDefinedItem
//purpose  : 
//=======================================================================

RWStepBasic_RWExternallyDefinedItem::RWStepBasic_RWExternallyDefinedItem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternallyDefinedItem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepBasic_ExternallyDefinedItem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"externally_defined_item") ) return;

  // Own fields of ExternallyDefinedItem

  StepBasic_SourceItem aItemId;
  data->ReadEntity (num, 1, "item_id", ach, aItemId);

  Handle(StepBasic_ExternalSource) aSource;
  data->ReadEntity (num, 2, "source", ach, STANDARD_TYPE(StepBasic_ExternalSource), aSource);

  // Initialize entity
  ent->Init(aItemId,
            aSource);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternallyDefinedItem::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepBasic_ExternallyDefinedItem) &ent) const
{

  // Own fields of ExternallyDefinedItem

  SW.Send (ent->ItemId().Value());

  SW.Send (ent->Source());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternallyDefinedItem::Share (const Handle(StepBasic_ExternallyDefinedItem) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Own fields of ExternallyDefinedItem

  iter.AddItem (ent->ItemId().Value());

  iter.AddItem (ent->Source());
}
