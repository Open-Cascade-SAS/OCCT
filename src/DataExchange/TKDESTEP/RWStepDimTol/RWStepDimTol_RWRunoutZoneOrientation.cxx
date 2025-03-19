// Created on: 2015-07-13
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include "RWStepDimTol_RWRunoutZoneOrientation.pxx"

#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_RunoutZoneOrientation.hxx>

//=================================================================================================

RWStepDimTol_RWRunoutZoneOrientation::RWStepDimTol_RWRunoutZoneOrientation() {}

//=================================================================================================

void RWStepDimTol_RWRunoutZoneOrientation::ReadStep(
  const Handle(StepData_StepReaderData)&          data,
  const Standard_Integer                          num,
  Handle(Interface_Check)&                        ach,
  const Handle(StepDimTol_RunoutZoneOrientation)& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 1, ach, "runout_zone_orientation"))
    return;

  // Own fields of RunoutZoneOrientation

  Handle(StepBasic_PlaneAngleMeasureWithUnit) anAngle;
  data
    ->ReadEntity(num, 1, "angle", ach, STANDARD_TYPE(StepBasic_PlaneAngleMeasureWithUnit), anAngle);

  // Initialize entity
  ent->Init(anAngle);
}

//=================================================================================================

void RWStepDimTol_RWRunoutZoneOrientation::WriteStep(
  StepData_StepWriter&                            SW,
  const Handle(StepDimTol_RunoutZoneOrientation)& ent) const
{
  SW.Send(ent->Angle());
}
