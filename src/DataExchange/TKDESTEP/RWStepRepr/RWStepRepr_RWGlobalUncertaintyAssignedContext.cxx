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
#include <Interface_EntityIterator.hxx>
#include "RWStepRepr_RWGlobalUncertaintyAssignedContext.pxx"
#include <StepBasic_UncertaintyMeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>

RWStepRepr_RWGlobalUncertaintyAssignedContext::RWStepRepr_RWGlobalUncertaintyAssignedContext() = default;

void RWStepRepr_RWGlobalUncertaintyAssignedContext::ReadStep(
  const occ::handle<StepData_StepReaderData>&                   data,
  const int                                                     num,
  occ::handle<Interface_Check>&                                 ach,
  const occ::handle<StepRepr_GlobalUncertaintyAssignedContext>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "global_uncertainty_assigned_context"))
    return;

  // --- inherited field : contextIdentifier ---

  occ::handle<TCollection_HAsciiString> aContextIdentifier;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "context_identifier", ach, aContextIdentifier);

  // --- inherited field : contextType ---

  occ::handle<TCollection_HAsciiString> aContextType;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadString(num, 2, "context_type", ach, aContextType);

  // --- own field : uncertainty ---

  occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>> aUncertainty;
  occ::handle<StepBasic_UncertaintyMeasureWithUnit>                                   anent3;
  int                                                                                 nsub3;
  if (data->ReadSubList(num, 3, "uncertainty", ach, nsub3))
  {
    int nb3 = data->NbParams(nsub3);
    aUncertainty =
      new NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>(1, nb3);
    for (int i3 = 1; i3 <= nb3; i3++)
    {
      // szv#4:S4163:12Mar99 `bool stat3 =` not needed
      if (data->ReadEntity(nsub3,
                           i3,
                           "uncertainty_measure_with_unit",
                           ach,
                           STANDARD_TYPE(StepBasic_UncertaintyMeasureWithUnit),
                           anent3))
        aUncertainty->SetValue(i3, anent3);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aContextIdentifier, aContextType, aUncertainty);
}

void RWStepRepr_RWGlobalUncertaintyAssignedContext::WriteStep(
  StepData_StepWriter&                                          SW,
  const occ::handle<StepRepr_GlobalUncertaintyAssignedContext>& ent) const
{

  // --- inherited field contextIdentifier ---

  SW.Send(ent->ContextIdentifier());

  // --- inherited field contextType ---

  SW.Send(ent->ContextType());

  // --- own field : uncertainty ---

  SW.OpenSub();
  for (int i3 = 1; i3 <= ent->NbUncertainty(); i3++)
  {
    SW.Send(ent->UncertaintyValue(i3));
  }
  SW.CloseSub();
}

void RWStepRepr_RWGlobalUncertaintyAssignedContext::Share(
  const occ::handle<StepRepr_GlobalUncertaintyAssignedContext>& ent,
  Interface_EntityIterator&                                     iter) const
{

  int nbElem1 = ent->NbUncertainty();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->UncertaintyValue(is1));
  }
}
