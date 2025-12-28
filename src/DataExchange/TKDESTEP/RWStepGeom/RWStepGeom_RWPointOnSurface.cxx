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
#include "RWStepGeom_RWPointOnSurface.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_PointOnSurface.hxx>
#include <StepGeom_Surface.hxx>

RWStepGeom_RWPointOnSurface::RWStepGeom_RWPointOnSurface() = default;

void RWStepGeom_RWPointOnSurface::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                           const int                                   num,
                                           occ::handle<Interface_Check>&               ach,
                                           const occ::handle<StepGeom_PointOnSurface>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "point_on_surface"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : basisSurface ---

  occ::handle<StepGeom_Surface> aBasisSurface;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "basis_surface", ach, STANDARD_TYPE(StepGeom_Surface), aBasisSurface);

  // --- own field : pointParameterU ---

  double aPointParameterU;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadReal(num, 3, "point_parameter_u", ach, aPointParameterU);

  // --- own field : pointParameterV ---

  double aPointParameterV;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadReal(num, 4, "point_parameter_v", ach, aPointParameterV);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aBasisSurface, aPointParameterU, aPointParameterV);
}

void RWStepGeom_RWPointOnSurface::WriteStep(StepData_StepWriter&                        SW,
                                            const occ::handle<StepGeom_PointOnSurface>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : basisSurface ---

  SW.Send(ent->BasisSurface());

  // --- own field : pointParameterU ---

  SW.Send(ent->PointParameterU());

  // --- own field : pointParameterV ---

  SW.Send(ent->PointParameterV());
}

void RWStepGeom_RWPointOnSurface::Share(const occ::handle<StepGeom_PointOnSurface>& ent,
                                        Interface_EntityIterator&                   iter) const
{

  iter.GetOneItem(ent->BasisSurface());
}
