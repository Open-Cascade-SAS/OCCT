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
#include "RWStepBasic_RWSolidAngleUnit.pxx"
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_SolidAngleUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWSolidAngleUnit::RWStepBasic_RWSolidAngleUnit() {}

void RWStepBasic_RWSolidAngleUnit::ReadStep(const Handle(StepData_StepReaderData)&  data,
                                            const Standard_Integer                  num,
                                            Handle(Interface_Check)&                ach,
                                            const Handle(StepBasic_SolidAngleUnit)& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 1, ach, "solid_angle_unit"))
    return;

  // --- inherited field : dimensions ---

  Handle(StepBasic_DimensionalExponents) aDimensions;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadEntity(num,
                   1,
                   "dimensions",
                   ach,
                   STANDARD_TYPE(StepBasic_DimensionalExponents),
                   aDimensions);

  //--- Initialisation of the read entity ---

  ent->Init(aDimensions);
}

void RWStepBasic_RWSolidAngleUnit::WriteStep(StepData_StepWriter&                    SW,
                                             const Handle(StepBasic_SolidAngleUnit)& ent) const
{

  // --- inherited field dimensions ---

  SW.Send(ent->Dimensions());
}

void RWStepBasic_RWSolidAngleUnit::Share(const Handle(StepBasic_SolidAngleUnit)& ent,
                                         Interface_EntityIterator&               iter) const
{

  iter.GetOneItem(ent->Dimensions());
}
