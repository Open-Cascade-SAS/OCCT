// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include "RWStepElement_RWElementMaterial.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_ElementMaterial.hxx>
#include <StepRepr_MaterialPropertyRepresentation.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

RWStepElement_RWElementMaterial::RWStepElement_RWElementMaterial() = default;

//=================================================================================================

void RWStepElement_RWElementMaterial::ReadStep(
  const occ::handle<StepData_StepReaderData>&     data,
  const int                                       num,
  occ::handle<Interface_Check>&                   ach,
  const occ::handle<StepElement_ElementMaterial>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 3, ach, "element_material"))
    return;

  // Own fields of ElementMaterial

  occ::handle<TCollection_HAsciiString> aMaterialId;
  data->ReadString(num, 1, "material_id", ach, aMaterialId);

  occ::handle<TCollection_HAsciiString> aDescription;
  data->ReadString(num, 2, "description", ach, aDescription);

  occ::handle<NCollection_HArray1<occ::handle<StepRepr_MaterialPropertyRepresentation>>>
      aProperties;
  int sub3 = 0;
  if (data->ReadSubList(num, 3, "properties", ach, sub3))
  {
    int nb0 = data->NbParams(sub3);
    aProperties =
      new NCollection_HArray1<occ::handle<StepRepr_MaterialPropertyRepresentation>>(1, nb0);
    int num2 = sub3;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      occ::handle<StepRepr_MaterialPropertyRepresentation> anIt0;
      data->ReadEntity(num2,
                       i0,
                       "material_property_representation",
                       ach,
                       STANDARD_TYPE(StepRepr_MaterialPropertyRepresentation),
                       anIt0);
      aProperties->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aMaterialId, aDescription, aProperties);
}

//=================================================================================================

void RWStepElement_RWElementMaterial::WriteStep(
  StepData_StepWriter&                            SW,
  const occ::handle<StepElement_ElementMaterial>& ent) const
{

  // Own fields of ElementMaterial

  SW.Send(ent->MaterialId());

  SW.Send(ent->Description());

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->Properties()->Length(); i2++)
  {
    occ::handle<StepRepr_MaterialPropertyRepresentation> Var0 = ent->Properties()->Value(i2);
    SW.Send(Var0);
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepElement_RWElementMaterial::Share(const occ::handle<StepElement_ElementMaterial>& ent,
                                            Interface_EntityIterator& iter) const
{

  // Own fields of ElementMaterial

  for (int i1 = 1; i1 <= ent->Properties()->Length(); i1++)
  {
    occ::handle<StepRepr_MaterialPropertyRepresentation> Var0 = ent->Properties()->Value(i1);
    iter.AddItem(Var0);
  }
}
