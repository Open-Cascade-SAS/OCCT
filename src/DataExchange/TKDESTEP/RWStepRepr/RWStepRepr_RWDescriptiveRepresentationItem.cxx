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

#include <Interface_Check.hxx>
#include "RWStepRepr_RWDescriptiveRepresentationItem.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

RWStepRepr_RWDescriptiveRepresentationItem::RWStepRepr_RWDescriptiveRepresentationItem() = default;

void RWStepRepr_RWDescriptiveRepresentationItem::ReadStep(
  const occ::handle<StepData_StepReaderData>&                data,
  const int                                                  num,
  occ::handle<Interface_Check>&                              ach,
  const occ::handle<StepRepr_DescriptiveRepresentationItem>& ent) const
{

  // --- Number of Parameter Control ---

  // if (!data->CheckNbParams(num,2,ach,"descriptive_representation_item")) return;
  // for the case when description is absent
  if (data->NbParams(num) != 2 && data->NbParams(num) != 1)
  {
    occ::handle<TCollection_HAsciiString> errmess = new TCollection_HAsciiString(
      "Count of Parameters is not 1 or 2 for descriptive_representation_item");
    occ::handle<TCollection_HAsciiString> errmesso =
      new TCollection_HAsciiString("Count of Parameters is not %d or %d for %s");
    ach->AddFail(errmess->ToCString(), errmesso->ToCString());
    return;
  }

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : description ---

  occ::handle<TCollection_HAsciiString> aDescription;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadString(num, 2, "description", ach, aDescription);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aDescription);
}

void RWStepRepr_RWDescriptiveRepresentationItem::WriteStep(
  StepData_StepWriter&                                       SW,
  const occ::handle<StepRepr_DescriptiveRepresentationItem>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : description ---

  SW.Send(ent->Description());
}
