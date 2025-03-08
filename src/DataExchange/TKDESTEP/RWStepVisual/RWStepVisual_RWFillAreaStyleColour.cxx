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
#include "RWStepVisual_RWFillAreaStyleColour.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_Colour.hxx>
#include <StepVisual_FillAreaStyleColour.hxx>

RWStepVisual_RWFillAreaStyleColour::RWStepVisual_RWFillAreaStyleColour() {}

void RWStepVisual_RWFillAreaStyleColour::ReadStep(
  const Handle(StepData_StepReaderData)&        data,
  const Standard_Integer                        num,
  Handle(Interface_Check)&                      ach,
  const Handle(StepVisual_FillAreaStyleColour)& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "fill_area_style_colour"))
    return;

  // --- own field : name ---

  Handle(TCollection_HAsciiString) aName;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : fillColour ---

  Handle(StepVisual_Colour) aFillColour;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadEntity(num, 2, "fill_colour", ach, STANDARD_TYPE(StepVisual_Colour), aFillColour);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aFillColour);
}

void RWStepVisual_RWFillAreaStyleColour::WriteStep(
  StepData_StepWriter&                          SW,
  const Handle(StepVisual_FillAreaStyleColour)& ent) const
{

  // --- own field : name ---

  SW.Send(ent->Name());

  // --- own field : fillColour ---

  SW.Send(ent->FillColour());
}

void RWStepVisual_RWFillAreaStyleColour::Share(const Handle(StepVisual_FillAreaStyleColour)& ent,
                                               Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->FillColour());
}
