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
#include "RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource.pxx"
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinitionFormationWithSpecifiedSource.hxx>
#include <StepBasic_Source.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_AsciiString.hxx>

#include "RWStepBasic_RWSource.pxx"

RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource::
  RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource()
= default;

void RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource::ReadStep(
  const occ::handle<StepData_StepReaderData>&                                 data,
  const int                                                                   num,
  occ::handle<Interface_Check>&                                               ach,
  const occ::handle<StepBasic_ProductDefinitionFormationWithSpecifiedSource>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "product_definition_formation_with_specified_source"))
    return;

  // --- inherited field : id ---

  occ::handle<TCollection_HAsciiString> aId;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "id", ach, aId);

  // --- inherited field : description ---

  occ::handle<TCollection_HAsciiString> aDescription;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadString(num, 2, "description", ach, aDescription);

  // --- inherited field : ofProduct ---

  occ::handle<StepBasic_Product> aOfProduct;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadEntity(num, 3, "of_product", ach, STANDARD_TYPE(StepBasic_Product), aOfProduct);

  // --- own field : makeOrBuy ---

  StepBasic_Source aMakeOrBuy = StepBasic_sNotKnown;
  if (data->ParamType(num, 4) == Interface_ParamEnum)
  {
    const char* text = data->ParamCValue(num, 4);
    if (!RWStepBasic_RWSource::ConvertToEnum(text, aMakeOrBuy))
    {
      ach->AddFail("Enumeration source has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #4 (make_or_buy) is not an enumeration");

  //--- Initialisation of the read entity ---

  ent->Init(aId, aDescription, aOfProduct, aMakeOrBuy);
}

void RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource::WriteStep(
  StepData_StepWriter&                                                        SW,
  const occ::handle<StepBasic_ProductDefinitionFormationWithSpecifiedSource>& ent) const
{

  // --- inherited field id ---

  SW.Send(ent->Id());

  // --- inherited field description ---

  SW.Send(ent->Description());

  // --- inherited field ofProduct ---

  SW.Send(ent->OfProduct());

  // --- own field : makeOrBuy ---

  SW.SendEnum(RWStepBasic_RWSource::ConvertToString(ent->MakeOrBuy()));
}

void RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource::Share(
  const occ::handle<StepBasic_ProductDefinitionFormationWithSpecifiedSource>& ent,
  Interface_EntityIterator&                                                   iter) const
{

  iter.GetOneItem(ent->OfProduct());
}
