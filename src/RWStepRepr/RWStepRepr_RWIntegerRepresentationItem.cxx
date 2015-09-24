// Created on: 2015-09-03
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


#include <Interface_Check.hxx>
#include <RWStepRepr_RWIntegerRepresentationItem.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_IntegerRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

RWStepRepr_RWIntegerRepresentationItem::RWStepRepr_RWIntegerRepresentationItem () {}

void RWStepRepr_RWIntegerRepresentationItem::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepRepr_IntegerRepresentationItem)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"integer_representation_item") ) return;

  // --- inherited field : name ---
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num,1,"name",ach,aName);

  // --- own field : value ---
  Standard_Integer aValue;
  data->ReadInteger (num,2,"value",ach,aValue);

  //--- Initialisation of the read entity ---
  ent->Init(aName, aValue);
}


void RWStepRepr_RWIntegerRepresentationItem::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepRepr_IntegerRepresentationItem)& ent) const
{
  SW.Send(ent->Name());
  SW.Send(ent->Value());
}
