// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepAP214_RWRepItemGroup.hxx>
#include <StepAP214_RepItemGroup.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
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

void RWStepAP214_RWRepItemGroup::Share (const Handle(StepAP214_RepItemGroup)&,
                                        Interface_EntityIterator&) const
{
  // Inherited fields of Group
  // Inherited fields of RepresentationItem
}
