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
#include "RWStepGeom_RWCylindricalSurface.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_CylindricalSurface.hxx>

RWStepGeom_RWCylindricalSurface::RWStepGeom_RWCylindricalSurface() = default;

void RWStepGeom_RWCylindricalSurface::ReadStep(
  const occ::handle<StepData_StepReaderData>&     data,
  const int                                       num,
  occ::handle<Interface_Check>&                   ach,
  const occ::handle<StepGeom_CylindricalSurface>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "cylindrical_surface"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : position ---

  occ::handle<StepGeom_Axis2Placement3d> aPosition;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "position", ach, STANDARD_TYPE(StepGeom_Axis2Placement3d), aPosition);

  // --- own field : radius ---

  double aRadius;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadReal(num, 3, "radius", ach, aRadius);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aPosition, aRadius);
}

void RWStepGeom_RWCylindricalSurface::WriteStep(
  StepData_StepWriter&                            SW,
  const occ::handle<StepGeom_CylindricalSurface>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field position ---

  SW.Send(ent->Position());

  // --- own field : radius ---

  SW.Send(ent->Radius());
}

void RWStepGeom_RWCylindricalSurface::Share(const occ::handle<StepGeom_CylindricalSurface>& ent,
                                            Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->Position());
}
