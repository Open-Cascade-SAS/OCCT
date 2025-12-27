// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Interface_EntityIterator.hxx>
#include "RWStepVisual_RWTemplateInstance.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepVisual_TemplateInstance.hxx>

RWStepVisual_RWTemplateInstance::RWStepVisual_RWTemplateInstance() {}

void RWStepVisual_RWTemplateInstance::ReadStep(const occ::handle<StepData_StepReaderData>&     data,
                                               const int                     num,
                                               occ::handle<Interface_Check>&                   ach,
                                               const occ::handle<StepVisual_TemplateInstance>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "template_instance"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : mappingSource ---

  occ::handle<StepRepr_RepresentationMap> aMappingSource;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num,
                   2,
                   "mapping_source",
                   ach,
                   STANDARD_TYPE(StepRepr_RepresentationMap),
                   aMappingSource);

  // --- inherited field : mappingTarget ---

  occ::handle<StepRepr_RepresentationItem> aMappingTarget;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num,
                   3,
                   "mapping_target",
                   ach,
                   STANDARD_TYPE(StepRepr_RepresentationItem),
                   aMappingTarget);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aMappingSource, aMappingTarget);
}

void RWStepVisual_RWTemplateInstance::WriteStep(
  StepData_StepWriter&                       SW,
  const occ::handle<StepVisual_TemplateInstance>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field mappingSource ---

  SW.Send(ent->MappingSource());

  // --- inherited field mappingTarget ---

  SW.Send(ent->MappingTarget());
}

void RWStepVisual_RWTemplateInstance::Share(const occ::handle<StepVisual_TemplateInstance>& ent,
                                            Interface_EntityIterator&                  iter) const
{

  iter.GetOneItem(ent->MappingSource());

  iter.GetOneItem(ent->MappingTarget());
}
