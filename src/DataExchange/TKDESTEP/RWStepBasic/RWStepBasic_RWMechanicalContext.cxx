// Created on: 1996-07-24
// Created by: Frederic MAUPAS
// Copyright (c) 1996-1999 Matra Datavision
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
#include "RWStepBasic_RWMechanicalContext.pxx"
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_MechanicalContext.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWMechanicalContext::RWStepBasic_RWMechanicalContext() {}

void RWStepBasic_RWMechanicalContext::ReadStep(
  const occ::handle<StepData_StepReaderData>&     data,
  const int                                       num,
  occ::handle<Interface_Check>&                   ach,
  const occ::handle<StepBasic_MechanicalContext>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "mechanical_context"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : frameOfReference ---

  occ::handle<StepBasic_ApplicationContext> aFrameOfReference;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num,
                   2,
                   "frame_of_reference",
                   ach,
                   STANDARD_TYPE(StepBasic_ApplicationContext),
                   aFrameOfReference);

  // --- own field : disciplineType ---

  occ::handle<TCollection_HAsciiString> aDisciplineType;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadString(num, 3, "discipline_type", ach, aDisciplineType);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aFrameOfReference, aDisciplineType);
}

void RWStepBasic_RWMechanicalContext::WriteStep(
  StepData_StepWriter&                            SW,
  const occ::handle<StepBasic_MechanicalContext>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field frameOfReference ---

  SW.Send(ent->FrameOfReference());

  // --- own field : disciplineType ---

  SW.Send(ent->DisciplineType());
}

void RWStepBasic_RWMechanicalContext::Share(const occ::handle<StepBasic_MechanicalContext>& ent,
                                            Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->FrameOfReference());
}
