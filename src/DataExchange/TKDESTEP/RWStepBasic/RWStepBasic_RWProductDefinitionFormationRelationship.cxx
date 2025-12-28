// Created on: 2002-12-15
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

#include <Interface_EntityIterator.hxx>
#include "RWStepBasic_RWProductDefinitionFormationRelationship.pxx"
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionFormationRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=================================================================================================

RWStepBasic_RWProductDefinitionFormationRelationship::
  RWStepBasic_RWProductDefinitionFormationRelationship()
= default;

//=================================================================================================

void RWStepBasic_RWProductDefinitionFormationRelationship::ReadStep(
  const occ::handle<StepData_StepReaderData>&                          data,
  const int                                                            num,
  occ::handle<Interface_Check>&                                        ach,
  const occ::handle<StepBasic_ProductDefinitionFormationRelationship>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 5, ach, "product_definition_formation_relationship"))
    return;

  // Own fields of ProductDefinitionFormationRelationship

  occ::handle<TCollection_HAsciiString> aId;
  data->ReadString(num, 1, "id", ach, aId);

  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 2, "name", ach, aName);

  occ::handle<TCollection_HAsciiString> aDescription;
  data->ReadString(num, 3, "description", ach, aDescription);

  occ::handle<StepBasic_ProductDefinitionFormation> aRelatingProductDefinitionFormation;
  data->ReadEntity(num,
                   4,
                   "relating_product_definition_formation",
                   ach,
                   STANDARD_TYPE(StepBasic_ProductDefinitionFormation),
                   aRelatingProductDefinitionFormation);

  occ::handle<StepBasic_ProductDefinitionFormation> aRelatedProductDefinitionFormation;
  data->ReadEntity(num,
                   5,
                   "related_product_definition_formation",
                   ach,
                   STANDARD_TYPE(StepBasic_ProductDefinitionFormation),
                   aRelatedProductDefinitionFormation);

  // Initialize entity
  ent->Init(aId,
            aName,
            aDescription,
            aRelatingProductDefinitionFormation,
            aRelatedProductDefinitionFormation);
}

//=================================================================================================

void RWStepBasic_RWProductDefinitionFormationRelationship::WriteStep(
  StepData_StepWriter&                                                 SW,
  const occ::handle<StepBasic_ProductDefinitionFormationRelationship>& ent) const
{

  // Own fields of ProductDefinitionFormationRelationship

  SW.Send(ent->Id());

  SW.Send(ent->Name());

  SW.Send(ent->Description());

  SW.Send(ent->RelatingProductDefinitionFormation());

  SW.Send(ent->RelatedProductDefinitionFormation());
}

//=================================================================================================

void RWStepBasic_RWProductDefinitionFormationRelationship::Share(
  const occ::handle<StepBasic_ProductDefinitionFormationRelationship>& ent,
  Interface_EntityIterator&                                            iter) const
{

  // Own fields of ProductDefinitionFormationRelationship

  iter.AddItem(ent->RelatingProductDefinitionFormation());

  iter.AddItem(ent->RelatedProductDefinitionFormation());
}
