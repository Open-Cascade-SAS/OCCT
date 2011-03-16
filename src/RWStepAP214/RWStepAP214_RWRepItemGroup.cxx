// File:	RWStepAP214_RWRepItemGroup.cxx
// Created:	Wed May 10 15:09:08 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepAP214_RWRepItemGroup.ixx>
#include <StepRepr_RepresentationItem.hxx>

//=======================================================================
//function : RWStepAP214_RWRepItemGroup
//purpose  : 
//=======================================================================

RWStepAP214_RWRepItemGroup::RWStepAP214_RWRepItemGroup ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWRepItemGroup::ReadStep (const Handle(StepData_StepReaderData)& data,
                                           const Standard_Integer num,
                                           Handle(Interface_Check)& ach,
                                           const Handle(StepAP214_RepItemGroup) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"rep_item_group") ) return;

  // Inherited fields of Group

  Handle(TCollection_HAsciiString) aGroup_Name;
  data->ReadString (num, 1, "group.name", ach, aGroup_Name);

  Handle(TCollection_HAsciiString) aGroup_Description;
  Standard_Boolean hasGroup_Description = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "group.description", ach, aGroup_Description);
  }
  else {
    hasGroup_Description = Standard_False;
  }

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 3, "representation_item.name", ach, aRepresentationItem_Name);

  // Initialize entity
  ent->Init(aGroup_Name,
            hasGroup_Description,
            aGroup_Description,
            aRepresentationItem_Name);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWRepItemGroup::WriteStep (StepData_StepWriter& SW,
                                            const Handle(StepAP214_RepItemGroup) &ent) const
{

  // Inherited fields of Group

  SW.Send (ent->StepBasic_Group::Name());

  if ( ent->StepBasic_Group::HasDescription() ) {
    SW.Send (ent->StepBasic_Group::Description());
  }
  else SW.SendUndef();

  // Inherited fields of RepresentationItem

  SW.Send (ent->RepresentationItem()->Name());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepAP214_RWRepItemGroup::Share (const Handle(StepAP214_RepItemGroup) &ent,
                                        Interface_EntityIterator& iter) const
{

  // Inherited fields of Group

  // Inherited fields of RepresentationItem
}
