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
#include "RWStepShape_RWOrientedOpenShell.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_OrientedOpenShell.hxx>

RWStepShape_RWOrientedOpenShell::RWStepShape_RWOrientedOpenShell() = default;

void RWStepShape_RWOrientedOpenShell::ReadStep(
  const occ::handle<StepData_StepReaderData>&     data,
  const int                                       num,
  occ::handle<Interface_Check>&                   ach,
  const occ::handle<StepShape_OrientedOpenShell>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "oriented_open_shell"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : cfsFaces ---
  // --- this field is redefined ---
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->CheckDerived(num, 2, "cfs_faces", ach, false);

  // --- own field : openShellElement ---

  occ::handle<StepShape_OpenShell> aOpenShellElement;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num,
                   3,
                   "open_shell_element",
                   ach,
                   STANDARD_TYPE(StepShape_OpenShell),
                   aOpenShellElement);

  // --- own field : orientation ---

  bool aOrientation;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadBoolean(num, 4, "orientation", ach, aOrientation);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aOpenShellElement, aOrientation);
}

void RWStepShape_RWOrientedOpenShell::WriteStep(
  StepData_StepWriter&                            SW,
  const occ::handle<StepShape_OrientedOpenShell>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field cfsFaces ---

  SW.SendDerived();

  // --- own field : openShellElement ---

  SW.Send(ent->OpenShellElement());

  // --- own field : orientation ---

  SW.SendBoolean(ent->Orientation());
}

void RWStepShape_RWOrientedOpenShell::Share(const occ::handle<StepShape_OrientedOpenShell>& ent,
                                            Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->OpenShellElement());
}
