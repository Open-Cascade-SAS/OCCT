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
#include "RWStepGeom_RWSurfaceOfRevolution.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_SurfaceOfRevolution.hxx>

RWStepGeom_RWSurfaceOfRevolution::RWStepGeom_RWSurfaceOfRevolution() = default;

void RWStepGeom_RWSurfaceOfRevolution::ReadStep(
  const occ::handle<StepData_StepReaderData>&      data,
  const int                                        num,
  occ::handle<Interface_Check>&                    ach,
  const occ::handle<StepGeom_SurfaceOfRevolution>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "surface_of_revolution"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- inherited field : sweptCurve ---

  occ::handle<StepGeom_Curve> aSweptCurve;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "swept_curve", ach, STANDARD_TYPE(StepGeom_Curve), aSweptCurve);

  // --- own field : axisPosition ---

  occ::handle<StepGeom_Axis1Placement> aAxisPosition;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num,
                   3,
                   "axis_position",
                   ach,
                   STANDARD_TYPE(StepGeom_Axis1Placement),
                   aAxisPosition);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aSweptCurve, aAxisPosition);
}

void RWStepGeom_RWSurfaceOfRevolution::WriteStep(
  StepData_StepWriter&                             SW,
  const occ::handle<StepGeom_SurfaceOfRevolution>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- inherited field sweptCurve ---

  SW.Send(ent->SweptCurve());

  // --- own field : axisPosition ---

  SW.Send(ent->AxisPosition());
}

void RWStepGeom_RWSurfaceOfRevolution::Share(const occ::handle<StepGeom_SurfaceOfRevolution>& ent,
                                             Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->SweptCurve());

  iter.GetOneItem(ent->AxisPosition());
}
