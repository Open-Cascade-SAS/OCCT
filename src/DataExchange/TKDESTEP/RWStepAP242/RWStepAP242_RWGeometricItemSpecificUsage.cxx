// Created on: 2015-07-07
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

#include "RWStepAP242_RWGeometricItemSpecificUsage.pxx"
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepAP242_GeometricItemSpecificUsage.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepRepr_Representation.hxx>

RWStepAP242_RWGeometricItemSpecificUsage::RWStepAP242_RWGeometricItemSpecificUsage() {}

void RWStepAP242_RWGeometricItemSpecificUsage::ReadStep(
  const occ::handle<StepData_StepReaderData>&              data,
  const int                                                num,
  occ::handle<Interface_Check>&                            ach,
  const occ::handle<StepAP242_GeometricItemSpecificUsage>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 5, ach, "geometric_item_specific_usage"))
    return;

  // Inherited fields of ItemIdentifiedRepresentationUsage

  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "item_identified_representation_usage.name", ach, aName);

  occ::handle<TCollection_HAsciiString> aDescription;
  if (data->IsParamDefined(num, 2))
  {
    data->ReadString(num, 2, "item_identified_representation_usage.description", ach, aDescription);
  }

  StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
  data->ReadEntity(num, 3, "item_identified_representation_usage.definition", ach, aDefinition);

  occ::handle<StepRepr_Representation> aRepresentation;
  data->ReadEntity(num,
                   4,
                   "item_identified_representation_usage.used_representation",
                   ach,
                   STANDARD_TYPE(StepRepr_Representation),
                   aRepresentation);

  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anItems;
  occ::handle<StepRepr_RepresentationItem>                                   anEnt;
  int                                                                        nbSub;
  Interface_ParamType aType = data->ParamType(num, 5);
  if (aType == Interface_ParamIdent)
  {
    data->ReadEntity(num,
                     5,
                     "item_identified_representation_usage.identified_item",
                     ach,
                     STANDARD_TYPE(StepRepr_RepresentationItem),
                     anEnt);
    anItems = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
    anItems->SetValue(1, anEnt);
  }
  else if (data->ReadSubList(num,
                             5,
                             "item_identified_representation_usage.identified_item",
                             ach,
                             nbSub))
  {
    int nbElements = data->NbParams(nbSub);
    anItems = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, nbElements);
    for (int i = 1; i <= nbElements; i++)
    {
      if (data->ReadEntity(nbSub,
                           i,
                           "representation_item",
                           ach,
                           STANDARD_TYPE(StepRepr_RepresentationItem),
                           anEnt))
        anItems->SetValue(i, anEnt);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aDescription, aDefinition, aRepresentation, anItems);
}

void RWStepAP242_RWGeometricItemSpecificUsage::WriteStep(
  StepData_StepWriter&                                     SW,
  const occ::handle<StepAP242_GeometricItemSpecificUsage>& ent) const
{
  // Inherited fields of ItemIdentifiedRepresentationUsage

  SW.Send(ent->Name());

  SW.Send(ent->Description());

  SW.Send(ent->Definition().Value());

  SW.Send(ent->UsedRepresentation());

  if (ent->NbIdentifiedItem() == 1)
    SW.Send(ent->IdentifiedItemValue(1));
  else
  {
    SW.OpenSub();
    for (int i = 1; i <= ent->NbIdentifiedItem(); i++)
    {
      SW.Send(ent->IdentifiedItemValue(i));
    }
    SW.CloseSub();
  }
}

void RWStepAP242_RWGeometricItemSpecificUsage::Share(
  const occ::handle<StepAP242_GeometricItemSpecificUsage>& ent,
  Interface_EntityIterator&                                iter) const
{
  // Inherited fields of ItemIdentifiedRepresentationUsage

  iter.AddItem(ent->Definition().Value());
  int i, nb = ent->NbIdentifiedItem();
  for (i = 1; i <= nb; i++)
    iter.AddItem(ent->IdentifiedItemValue(i));
}
