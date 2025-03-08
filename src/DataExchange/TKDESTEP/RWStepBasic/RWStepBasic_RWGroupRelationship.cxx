// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_EntityIterator.hxx>
#include "RWStepBasic_RWGroupRelationship.pxx"
#include <StepBasic_Group.hxx>
#include <StepBasic_GroupRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=================================================================================================

RWStepBasic_RWGroupRelationship::RWStepBasic_RWGroupRelationship() {}

//=================================================================================================

void RWStepBasic_RWGroupRelationship::ReadStep(const Handle(StepData_StepReaderData)&     data,
                                               const Standard_Integer                     num,
                                               Handle(Interface_Check)&                   ach,
                                               const Handle(StepBasic_GroupRelationship)& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 4, ach, "group_relationship"))
    return;

  // Own fields of GroupRelationship

  Handle(TCollection_HAsciiString) aName;
  data->ReadString(num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean                 hasDescription = Standard_True;
  if (data->IsParamDefined(num, 2))
  {
    data->ReadString(num, 2, "description", ach, aDescription);
  }
  else
  {
    hasDescription = Standard_False;
  }

  Handle(StepBasic_Group) aRelatingGroup;
  data->ReadEntity(num, 3, "relating_group", ach, STANDARD_TYPE(StepBasic_Group), aRelatingGroup);

  Handle(StepBasic_Group) aRelatedGroup;
  data->ReadEntity(num, 4, "related_group", ach, STANDARD_TYPE(StepBasic_Group), aRelatedGroup);

  // Initialize entity
  ent->Init(aName, hasDescription, aDescription, aRelatingGroup, aRelatedGroup);
}

//=================================================================================================

void RWStepBasic_RWGroupRelationship::WriteStep(
  StepData_StepWriter&                       SW,
  const Handle(StepBasic_GroupRelationship)& ent) const
{

  // Own fields of GroupRelationship

  SW.Send(ent->Name());

  if (ent->HasDescription())
  {
    SW.Send(ent->Description());
  }
  else
    SW.SendUndef();

  SW.Send(ent->RelatingGroup());

  SW.Send(ent->RelatedGroup());
}

//=================================================================================================

void RWStepBasic_RWGroupRelationship::Share(const Handle(StepBasic_GroupRelationship)& ent,
                                            Interface_EntityIterator&                  iter) const
{

  // Own fields of GroupRelationship

  iter.AddItem(ent->RelatingGroup());

  iter.AddItem(ent->RelatedGroup());
}
