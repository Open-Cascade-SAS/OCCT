// Created on: 1999-10-12
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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
#include "RWStepBasic_RWConversionBasedUnitAndVolumeUnit.pxx"
#include <StepBasic_ConversionBasedUnitAndVolumeUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWConversionBasedUnitAndVolumeUnit::RWStepBasic_RWConversionBasedUnitAndVolumeUnit() {}

void RWStepBasic_RWConversionBasedUnitAndVolumeUnit::ReadStep(
  const Handle(StepData_StepReaderData)&                    data,
  const Standard_Integer                                    num0,
  Handle(Interface_Check)&                                  ach,
  const Handle(StepBasic_ConversionBasedUnitAndVolumeUnit)& ent) const
{
  Standard_Integer num = num0;
  // --- Instance of plex component ConversionBasedUnit ---
  if (!data->CheckNbParams(num, 2, ach, "conversion_based_unit"))
    return;
  Handle(TCollection_HAsciiString) aName;
  data->ReadString(num, 1, "name", ach, aName);

  // --- field : conversionFactor ---
  Handle(StepBasic_MeasureWithUnit) aConversionFactor;
  data->ReadEntity(num,
                   2,
                   "conversion_factor",
                   ach,
                   STANDARD_TYPE(StepBasic_MeasureWithUnit),
                   aConversionFactor);

  num = data->NextForComplex(num);
  if (!data->CheckNbParams(num, 1, ach, "named_unit"))
    return;
  Handle(StepBasic_DimensionalExponents) aDimensions;
  data->ReadEntity(num,
                   1,
                   "dimensions",
                   ach,
                   STANDARD_TYPE(StepBasic_DimensionalExponents),
                   aDimensions);

  data->NamedForComplex("VOLUME_UNIT", "VLMUNT", num0, num, ach);
  if (!data->CheckNbParams(num, 0, ach, "volume_unit"))
    return;

  ent->Init(aDimensions, aName, aConversionFactor);
}

void RWStepBasic_RWConversionBasedUnitAndVolumeUnit::WriteStep(
  StepData_StepWriter&                                      SW,
  const Handle(StepBasic_ConversionBasedUnitAndVolumeUnit)& ent) const
{
  SW.StartEntity("CONVERSION_BASED_UNIT");
  SW.Send(ent->Name());
  SW.Send(ent->ConversionFactor());
  SW.StartEntity("NAMED_UNIT");
  SW.Send(ent->Dimensions());
  SW.StartEntity("VOLUME_UNIT");
}

void RWStepBasic_RWConversionBasedUnitAndVolumeUnit::Share(
  const Handle(StepBasic_ConversionBasedUnitAndVolumeUnit)& ent,
  Interface_EntityIterator&                                 iter) const
{
  iter.GetOneItem(ent->Dimensions());
  iter.GetOneItem(ent->ConversionFactor());
}
