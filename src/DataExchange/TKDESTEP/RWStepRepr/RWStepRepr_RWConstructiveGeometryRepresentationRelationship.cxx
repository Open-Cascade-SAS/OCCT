// Created on: 2016-04-26
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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
#include "RWStepRepr_RWConstructiveGeometryRepresentationRelationship.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ConstructiveGeometryRepresentation.hxx>
#include <StepRepr_ConstructiveGeometryRepresentationRelationship.hxx>

RWStepRepr_RWConstructiveGeometryRepresentationRelationship::
  RWStepRepr_RWConstructiveGeometryRepresentationRelationship()
= default;

void RWStepRepr_RWConstructiveGeometryRepresentationRelationship::ReadStep(
  const occ::handle<StepData_StepReaderData>&                                 data,
  const int                                                                   num,
  occ::handle<Interface_Check>&                                               ach,
  const occ::handle<StepRepr_ConstructiveGeometryRepresentationRelationship>& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 4, ach, "constructive_geometry_representation_relationship"))
    return;

  // Inherited field : name
  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);

  // Inherited field : description
  occ::handle<TCollection_HAsciiString> aDescription;
  if (data->IsParamDefined(num, 2))
  {
    data->ReadString(num, 2, "description", ach, aDescription);
  }
  // Inherited field : rep_1
  occ::handle<StepRepr_Representation> aRep1;
  data->ReadEntity(num, 3, "rep_1", ach, STANDARD_TYPE(StepRepr_Representation), aRep1);

  // Inherited field : rep_2
  occ::handle<StepRepr_Representation> aRep2;
  data->ReadEntity(num, 4, "rep_2", ach, STANDARD_TYPE(StepRepr_Representation), aRep2);

  // Initialisation of the read entity
  ent->Init(aName, aDescription, aRep1, aRep2);
}

void RWStepRepr_RWConstructiveGeometryRepresentationRelationship::WriteStep(
  StepData_StepWriter&                                                        SW,
  const occ::handle<StepRepr_ConstructiveGeometryRepresentationRelationship>& ent) const
{
  // Inherited field : name
  SW.Send(ent->Name());

  // Inherited field : description
  SW.Send(ent->Description());

  // Inherited field : rep_1
  SW.Send(ent->Rep1());

  // Inherited field : rep_2
  SW.Send(ent->Rep2());
}

void RWStepRepr_RWConstructiveGeometryRepresentationRelationship::Share(
  const occ::handle<StepRepr_ConstructiveGeometryRepresentationRelationship>& ent,
  Interface_EntityIterator&                                                   iter) const
{
  iter.GetOneItem(ent->Rep1());
  iter.GetOneItem(ent->Rep2());
}
