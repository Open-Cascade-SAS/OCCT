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

#include <RWStepAP242_RWIdAttribute.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepAP242_IdAttribute.hxx>
#include <StepAP242_IdAttributeSelect.hxx>

RWStepAP242_RWIdAttribute::RWStepAP242_RWIdAttribute () {}

void RWStepAP242_RWIdAttribute::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepAP242_IdAttribute)& ent) const
{


  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num,2,ach,"id_attribute")) return;
  
  // --- own field : attribute_value ---

  Handle(TCollection_HAsciiString) anAttributeValue;
  data->ReadString (num,1,"attribute_value",ach,anAttributeValue);

  // --- own field : identified_item ---

  StepAP242_IdAttributeSelect anIdentifiedItem;
  data->ReadEntity (num,2,"identified_item",ach,anIdentifiedItem);

  //--- Initialisation of the read entity ---

  ent->Init(anAttributeValue, anIdentifiedItem);
}

void RWStepAP242_RWIdAttribute::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepAP242_IdAttribute)& ent) const
{

  // --- own field : attribute_value ---

  SW.Send(ent->AttributeValue());

  // --- own field : identified_item ---

  SW.Send(ent->IdentifiedItem().Value());
}

void RWStepAP242_RWIdAttribute::Share(const Handle(StepAP242_IdAttribute)& ent, Interface_EntityIterator& iter) const
{
	iter.GetOneItem(ent->IdentifiedItem().Value());
}

