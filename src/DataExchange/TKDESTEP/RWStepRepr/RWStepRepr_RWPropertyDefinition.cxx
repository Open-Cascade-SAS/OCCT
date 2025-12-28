// Created on: 2000-07-03
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
#include "RWStepRepr_RWPropertyDefinition.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_PropertyDefinition.hxx>

//=================================================================================================

RWStepRepr_RWPropertyDefinition::RWStepRepr_RWPropertyDefinition() {}

//=================================================================================================

void RWStepRepr_RWPropertyDefinition::ReadStep(
  const occ::handle<StepData_StepReaderData>&     data,
  const int                                       num,
  occ::handle<Interface_Check>&                   ach,
  const occ::handle<StepRepr_PropertyDefinition>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 3, ach, "property_definition"))
    return;

  // Own fields of PropertyDefinition

  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);

  occ::handle<TCollection_HAsciiString> aDescription;
  bool                                  hasDescription = true;
  if (data->IsParamDefined(num, 2))
  {
    data->ReadString(num, 2, "description", ach, aDescription);
  }
  else
  {
    hasDescription = false;
  }

  StepRepr_CharacterizedDefinition aDefinition;
  data->ReadEntity(num, 3, "definition", ach, aDefinition);

  // Initialize entity
  ent->Init(aName, hasDescription, aDescription, aDefinition);
}

//=================================================================================================

void RWStepRepr_RWPropertyDefinition::WriteStep(
  StepData_StepWriter&                            SW,
  const occ::handle<StepRepr_PropertyDefinition>& ent) const
{

  // Own fields of PropertyDefinition

  SW.Send(ent->Name());

  if (ent->HasDescription())
  {
    SW.Send(ent->Description());
  }
  else
    SW.SendUndef();

  SW.Send(ent->Definition().Value());
}

//=================================================================================================

void RWStepRepr_RWPropertyDefinition::Share(const occ::handle<StepRepr_PropertyDefinition>& ent,
                                            Interface_EntityIterator& iter) const
{

  // Own fields of PropertyDefinition

  iter.AddItem(ent->Definition().Value());
}
