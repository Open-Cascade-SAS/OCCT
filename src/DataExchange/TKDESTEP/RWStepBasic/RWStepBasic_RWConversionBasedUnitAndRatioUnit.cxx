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
#include "RWStepBasic_RWConversionBasedUnitAndRatioUnit.pxx"
#include <StepBasic_ConversionBasedUnitAndRatioUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_RatioUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWConversionBasedUnitAndRatioUnit::RWStepBasic_RWConversionBasedUnitAndRatioUnit() {}

void RWStepBasic_RWConversionBasedUnitAndRatioUnit::ReadStep(
  const Handle(StepData_StepReaderData)&                   data,
  const Standard_Integer                                   num0,
  Handle(Interface_Check)&                                 ach,
  const Handle(StepBasic_ConversionBasedUnitAndRatioUnit)& ent) const
{

  Standard_Integer num = num0;

  // --- Instance of plex component ConversionBasedUnit ---

  if (!data->CheckNbParams(num, 2, ach, "conversion_based_unit"))
    return;

  // --- field : name ---

  Handle(TCollection_HAsciiString) aName;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- field : conversionFactor ---

  Handle(StepBasic_MeasureWithUnit) aConversionFactor;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadEntity(num,
                   2,
                   "conversion_factor",
                   ach,
                   STANDARD_TYPE(StepBasic_MeasureWithUnit),
                   aConversionFactor);

  num = data->NextForComplex(num);

  // --- Instance of common supertype NamedUnit ---

  if (!data->CheckNbParams(num, 1, ach, "named_unit"))
    return;

  // --- field : dimensions ---

  Handle(StepBasic_DimensionalExponents) aDimensions;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
  data->ReadEntity(num,
                   1,
                   "dimensions",
                   ach,
                   STANDARD_TYPE(StepBasic_DimensionalExponents),
                   aDimensions);

  num = data->NextForComplex(num);

  // --- Instance of plex component RatioUnit ---

  if (!data->CheckNbParams(num, 0, ach, "ratio_unit"))
    return;

  //--- Initialisation of the red entity ---

  ent->Init(aDimensions, aName, aConversionFactor);
}

void RWStepBasic_RWConversionBasedUnitAndRatioUnit::WriteStep(
  StepData_StepWriter&                                     SW,
  const Handle(StepBasic_ConversionBasedUnitAndRatioUnit)& ent) const
{

  // --- Instance of plex component ConversionBasedUnit ---

  SW.StartEntity("CONVERSION_BASED_UNIT");
  // --- field : name ---

  SW.Send(ent->Name());
  // --- field : conversionFactor ---

  SW.Send(ent->ConversionFactor());

  // --- Instance of plex component RatioUnit ---

  SW.StartEntity("RATIO_UNIT");

  // --- Instance of common supertype NamedUnit ---

  SW.StartEntity("NAMED_UNIT");
  // --- field : dimensions ---

  SW.Send(ent->Dimensions());
}

void RWStepBasic_RWConversionBasedUnitAndRatioUnit::Share(
  const Handle(StepBasic_ConversionBasedUnitAndRatioUnit)& ent,
  Interface_EntityIterator&                                iter) const
{

  iter.GetOneItem(ent->Dimensions());

  iter.GetOneItem(ent->ConversionFactor());
}
