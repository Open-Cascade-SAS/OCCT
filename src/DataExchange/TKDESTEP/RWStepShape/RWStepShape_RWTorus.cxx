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
#include "RWStepShape_RWTorus.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepShape_Torus.hxx>

RWStepShape_RWTorus::RWStepShape_RWTorus() = default;

void RWStepShape_RWTorus::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                   const int                                   num,
                                   occ::handle<Interface_Check>&               ach,
                                   const occ::handle<StepShape_Torus>&         ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "torus"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : position ---

  occ::handle<StepGeom_Axis1Placement> aPosition;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "position", ach, STANDARD_TYPE(StepGeom_Axis1Placement), aPosition);

  // --- own field : majorRadius ---

  double aMajorRadius;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadReal(num, 3, "major_radius", ach, aMajorRadius);

  // --- own field : minorRadius ---

  double aMinorRadius;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadReal(num, 4, "minor_radius", ach, aMinorRadius);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aPosition, aMajorRadius, aMinorRadius);
}

void RWStepShape_RWTorus::WriteStep(StepData_StepWriter&                SW,
                                    const occ::handle<StepShape_Torus>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : position ---

  SW.Send(ent->Position());

  // --- own field : majorRadius ---

  SW.Send(ent->MajorRadius());

  // --- own field : minorRadius ---

  SW.Send(ent->MinorRadius());
}

void RWStepShape_RWTorus::Share(const occ::handle<StepShape_Torus>& ent,
                                Interface_EntityIterator&           iter) const
{

  iter.GetOneItem(ent->Position());
}
