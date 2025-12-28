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
#include "RWStepShape_RWRevolvedAreaSolid.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepShape_RevolvedAreaSolid.hxx>

RWStepShape_RWRevolvedAreaSolid::RWStepShape_RWRevolvedAreaSolid() {}

void RWStepShape_RWRevolvedAreaSolid::ReadStep(
  const occ::handle<StepData_StepReaderData>&     data,
  const int                                       num,
  occ::handle<Interface_Check>&                   ach,
  const occ::handle<StepShape_RevolvedAreaSolid>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "revolved_area_solid"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : sweptArea ---

  occ::handle<StepGeom_CurveBoundedSurface> aSweptArea;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num,
                   2,
                   "swept_area",
                   ach,
                   STANDARD_TYPE(StepGeom_CurveBoundedSurface),
                   aSweptArea);

  // --- own field : axis ---

  occ::handle<StepGeom_Axis1Placement> aAxis;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num, 3, "axis", ach, STANDARD_TYPE(StepGeom_Axis1Placement), aAxis);

  // --- own field : angle ---

  double aAngle;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadReal(num, 4, "angle", ach, aAngle);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aSweptArea, aAxis, aAngle);
}

void RWStepShape_RWRevolvedAreaSolid::WriteStep(
  StepData_StepWriter&                            SW,
  const occ::handle<StepShape_RevolvedAreaSolid>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field sweptArea ---

  SW.Send(ent->SweptArea());

  // --- own field : axis ---

  SW.Send(ent->Axis());

  // --- own field : angle ---

  SW.Send(ent->Angle());
}

void RWStepShape_RWRevolvedAreaSolid::Share(const occ::handle<StepShape_RevolvedAreaSolid>& ent,
                                            Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->SweptArea());

  iter.GetOneItem(ent->Axis());
}
