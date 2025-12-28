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

// gka 05.03.99 S4134 upgrade from CD to DIS

#include <Interface_EntityIterator.hxx>
#include "RWStepRepr_RWAssemblyComponentUsageSubstitute.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <StepRepr_AssemblyComponentUsageSubstitute.hxx>

RWStepRepr_RWAssemblyComponentUsageSubstitute::RWStepRepr_RWAssemblyComponentUsageSubstitute() =
  default;

void RWStepRepr_RWAssemblyComponentUsageSubstitute::ReadStep(
  const occ::handle<StepData_StepReaderData>&                   data,
  const int                                                     num,
  occ::handle<Interface_Check>&                                 ach,
  const occ::handle<StepRepr_AssemblyComponentUsageSubstitute>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "assembly_component_usage_substitute"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : definition ---

  occ::handle<TCollection_HAsciiString> aDef;
  if (data->IsParamDefined(num, 2))
  { // gka 05.03.99 S4134 upgrade from CD to DIS
    // szv#4:S4163:12Mar99 `bool stat2 =` not needed
    data->ReadString(num, 2, "definition", ach, aDef);
  }
  // --- own field : base ---

  occ::handle<StepRepr_AssemblyComponentUsage> aBase;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num, 3, "base", ach, STANDARD_TYPE(StepRepr_AssemblyComponentUsage), aBase);

  // --- own field : substitute ---

  occ::handle<StepRepr_AssemblyComponentUsage> aSubs;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data
    ->ReadEntity(num, 4, "substitute", ach, STANDARD_TYPE(StepRepr_AssemblyComponentUsage), aSubs);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aDef, aBase, aSubs);
}

void RWStepRepr_RWAssemblyComponentUsageSubstitute::WriteStep(
  StepData_StepWriter&                                          SW,
  const occ::handle<StepRepr_AssemblyComponentUsageSubstitute>& ent) const
{

  SW.Send(ent->Name());
  SW.Send(ent->Definition());
  SW.Send(ent->Base());
  SW.Send(ent->Substitute());
}

void RWStepRepr_RWAssemblyComponentUsageSubstitute::Share(
  const occ::handle<StepRepr_AssemblyComponentUsageSubstitute>& ent,
  Interface_EntityIterator&                                     iter) const
{

  iter.GetOneItem(ent->Base());

  iter.GetOneItem(ent->Substitute());
}
