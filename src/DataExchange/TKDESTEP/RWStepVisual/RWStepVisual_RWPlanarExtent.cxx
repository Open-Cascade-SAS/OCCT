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

#include "RWStepVisual_RWPlanarExtent.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_PlanarExtent.hxx>

RWStepVisual_RWPlanarExtent::RWStepVisual_RWPlanarExtent() {}

void RWStepVisual_RWPlanarExtent::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                           const int                 num,
                                           occ::handle<Interface_Check>&               ach,
                                           const occ::handle<StepVisual_PlanarExtent>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "planar_extent"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : sizeInX ---

  double aSizeInX;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadReal(num, 2, "size_in_x", ach, aSizeInX);

  // --- own field : sizeInY ---

  double aSizeInY;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadReal(num, 3, "size_in_y", ach, aSizeInY);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aSizeInX, aSizeInY);
}

void RWStepVisual_RWPlanarExtent::WriteStep(StepData_StepWriter&                   SW,
                                            const occ::handle<StepVisual_PlanarExtent>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : sizeInX ---

  SW.Send(ent->SizeInX());

  // --- own field : sizeInY ---

  SW.Send(ent->SizeInY());
}
