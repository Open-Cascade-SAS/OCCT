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

#include <Interface_EntityIterator.hxx>
#include "RWStepAP214_RWRepItemGroup.pxx"
#include <StepAP214_RepItemGroup.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationItem.hxx>

//=================================================================================================

RWStepAP214_RWRepItemGroup::RWStepAP214_RWRepItemGroup() = default;

//=================================================================================================

void RWStepAP214_RWRepItemGroup::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                          const int                                   num,
                                          occ::handle<Interface_Check>&               ach,
                                          const occ::handle<StepAP214_RepItemGroup>&  ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 3, ach, "rep_item_group"))
    return;

  // Inherited fields of Group

  occ::handle<TCollection_HAsciiString> aGroup_Name;
  data->ReadString(num, 1, "group.name", ach, aGroup_Name);

  occ::handle<TCollection_HAsciiString> aGroup_Description;
  bool                                  hasGroup_Description = true;
  if (data->IsParamDefined(num, 2))
  {
    data->ReadString(num, 2, "group.description", ach, aGroup_Description);
  }
  else
  {
    hasGroup_Description = false;
  }

  // Inherited fields of RepresentationItem

  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  data->ReadString(num, 3, "representation_item.name", ach, aRepresentationItem_Name);

  // Initialize entity
  ent->Init(aGroup_Name, hasGroup_Description, aGroup_Description, aRepresentationItem_Name);
}

//=================================================================================================

void RWStepAP214_RWRepItemGroup::WriteStep(StepData_StepWriter&                       SW,
                                           const occ::handle<StepAP214_RepItemGroup>& ent) const
{

  // Inherited fields of Group

  SW.Send(ent->StepBasic_Group::Name());

  if (ent->StepBasic_Group::HasDescription())
  {
    SW.Send(ent->StepBasic_Group::Description());
  }
  else
    SW.SendUndef();

  // Inherited fields of RepresentationItem

  SW.Send(ent->RepresentationItem()->Name());
}

//=================================================================================================

void RWStepAP214_RWRepItemGroup::Share(const occ::handle<StepAP214_RepItemGroup>&,
                                       Interface_EntityIterator&) const
{
  // Inherited fields of Group
  // Inherited fields of RepresentationItem
}
