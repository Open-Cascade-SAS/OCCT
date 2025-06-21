// Copyright (c) 2024 OPEN CASCADE SAS
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

#include "RWStepRepr_RWRealRepresentationItem.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RealRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

RWStepRepr_RWRealRepresentationItem::RWStepRepr_RWRealRepresentationItem() {}

void RWStepRepr_RWRealRepresentationItem::ReadStep(
  const Handle(StepData_StepReaderData)&         theData,
  const Standard_Integer                         theNum,
  Handle(Interface_Check)&                       theAch,
  const Handle(StepRepr_RealRepresentationItem)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 2, theAch, "real_representation_item"))
    return;

  // --- inherited field : name ---
  Handle(TCollection_HAsciiString) aName;
  theData->ReadString(theNum, 1, "name", theAch, aName);

  // --- own field : value ---
  Standard_Real aValue;
  theData->ReadReal(theNum, 2, "value", theAch, aValue);

  //--- Initialisation of the read entity ---
  theEnt->Init(aName, aValue);
}

void RWStepRepr_RWRealRepresentationItem::WriteStep(
  StepData_StepWriter&                           theSW,
  const Handle(StepRepr_RealRepresentationItem)& theEnt) const
{
  theSW.Send(theEnt->Name());
  theSW.Send(theEnt->Value());
}
