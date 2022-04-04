// Created on : Thu Mar 24 18:30:12 2022 
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#include <RWStepVisual_RWTessellatedStructuredItem.hxx>
#include <StepVisual_TessellatedStructuredItem.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : RWStepVisual_RWTessellatedStructuredItem
//purpose  : 
//=======================================================================

RWStepVisual_RWTessellatedStructuredItem::RWStepVisual_RWTessellatedStructuredItem() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedStructuredItem::ReadStep(
  const Handle(StepData_StepReaderData)& theData,
  const Standard_Integer theNum,
  Handle(Interface_Check)& theCheck,
  const Handle(StepVisual_TessellatedStructuredItem)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 1, theCheck, "tessellated_structured_item"))
  {
    return;
  }

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedStructuredItem::WriteStep(
  StepData_StepWriter& theSW,
  const Handle(StepVisual_TessellatedStructuredItem)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedStructuredItem::Share(
  const Handle(StepVisual_TessellatedStructuredItem)&,
Interface_EntityIterator&) const
{
}
