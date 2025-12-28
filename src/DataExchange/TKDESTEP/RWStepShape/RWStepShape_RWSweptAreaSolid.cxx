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
#include "RWStepShape_RWSweptAreaSolid.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepShape_SweptAreaSolid.hxx>

RWStepShape_RWSweptAreaSolid::RWStepShape_RWSweptAreaSolid() {}

void RWStepShape_RWSweptAreaSolid::ReadStep(const occ::handle<StepData_StepReaderData>&  data,
                                            const int                  num,
                                            occ::handle<Interface_Check>&                ach,
                                            const occ::handle<StepShape_SweptAreaSolid>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "swept_area_solid"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : sweptArea ---

  occ::handle<StepGeom_CurveBoundedSurface> aSweptArea;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num,
                   2,
                   "swept_area",
                   ach,
                   STANDARD_TYPE(StepGeom_CurveBoundedSurface),
                   aSweptArea);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aSweptArea);
}

void RWStepShape_RWSweptAreaSolid::WriteStep(StepData_StepWriter&                    SW,
                                             const occ::handle<StepShape_SweptAreaSolid>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : sweptArea ---

  SW.Send(ent->SweptArea());
}

void RWStepShape_RWSweptAreaSolid::Share(const occ::handle<StepShape_SweptAreaSolid>& ent,
                                         Interface_EntityIterator&               iter) const
{

  iter.GetOneItem(ent->SweptArea());
}
