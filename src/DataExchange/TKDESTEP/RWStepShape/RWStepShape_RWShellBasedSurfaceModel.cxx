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
#include "RWStepShape_RWShellBasedSurfaceModel.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_Shell.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>

RWStepShape_RWShellBasedSurfaceModel::RWStepShape_RWShellBasedSurfaceModel() {}

void RWStepShape_RWShellBasedSurfaceModel::ReadStep(
  const occ::handle<StepData_StepReaderData>&          data,
  const int                                            num,
  occ::handle<Interface_Check>&                        ach,
  const occ::handle<StepShape_ShellBasedSurfaceModel>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "shell_based_surface_model"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : sbsmBoundary ---

  occ::handle<NCollection_HArray1<StepShape_Shell>> aSbsmBoundary;
  StepShape_Shell                                   aSbsmBoundaryItem;
  int                                               nsub2;
  if (data->ReadSubList(num, 2, "sbsm_boundary", ach, nsub2))
  {
    int nb2       = data->NbParams(nsub2);
    aSbsmBoundary = new NCollection_HArray1<StepShape_Shell>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `bool stat2 =` not needed
      if (data->ReadEntity(nsub2, i2, "sbsm_boundary", ach, aSbsmBoundaryItem))
        aSbsmBoundary->SetValue(i2, aSbsmBoundaryItem);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aSbsmBoundary);
}

void RWStepShape_RWShellBasedSurfaceModel::WriteStep(
  StepData_StepWriter&                                 SW,
  const occ::handle<StepShape_ShellBasedSurfaceModel>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : sbsmBoundary ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbSbsmBoundary(); i2++)
  {
    SW.Send(ent->SbsmBoundaryValue(i2).Value());
  }
  SW.CloseSub();
}

void RWStepShape_RWShellBasedSurfaceModel::Share(
  const occ::handle<StepShape_ShellBasedSurfaceModel>& ent,
  Interface_EntityIterator&                            iter) const
{

  int nbElem1 = ent->NbSbsmBoundary();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->SbsmBoundaryValue(is1).Value());
  }
}
