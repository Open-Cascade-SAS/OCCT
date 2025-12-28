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
#include "RWStepBasic_RWProductDefinitionEffectivity.pxx"
#include <StepBasic_ProductDefinitionEffectivity.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>

RWStepBasic_RWProductDefinitionEffectivity::RWStepBasic_RWProductDefinitionEffectivity() = default;

void RWStepBasic_RWProductDefinitionEffectivity::ReadStep(
  const occ::handle<StepData_StepReaderData>&                data,
  const int                                                  num,
  occ::handle<Interface_Check>&                              ach,
  const occ::handle<StepBasic_ProductDefinitionEffectivity>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "product_definition_effectivity"))
    return;

  // --- inherited field : product_data_type ---

  occ::handle<TCollection_HAsciiString> aId;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "id", ach, aId);

  // --- own field : kind ---

  occ::handle<StepBasic_ProductDefinitionRelationship> aUsage;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num,
                   2,
                   "kind",
                   ach,
                   STANDARD_TYPE(StepBasic_ProductDefinitionRelationship),
                   aUsage);

  //--- Initialisation of the read entity ---

  ent->Init(aId, aUsage);
}

void RWStepBasic_RWProductDefinitionEffectivity::WriteStep(
  StepData_StepWriter&                                       SW,
  const occ::handle<StepBasic_ProductDefinitionEffectivity>& ent) const
{

  // --- own field : id ---

  SW.Send(ent->Id());
  SW.Send(ent->Usage());
}

void RWStepBasic_RWProductDefinitionEffectivity::Share(
  const occ::handle<StepBasic_ProductDefinitionEffectivity>& ent,
  Interface_EntityIterator&                                  iter) const
{
  iter.AddItem(ent->Usage());
}
