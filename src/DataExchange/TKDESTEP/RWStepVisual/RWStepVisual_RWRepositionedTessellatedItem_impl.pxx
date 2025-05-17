// Copyright (c) 2022 OPEN CASCADE SAS
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

#include "RWStepVisual_RWRepositionedTessellatedItem.pxx"

#include <Interface_Check.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_RepositionedTessellatedItem.hxx>
#include <StepGeom_Axis2Placement3d.hxx>

//=================================================================================================

void RWStepVisual_RWRepositionedTessellatedItem::ReadStep(
  const Handle(StepData_StepReaderData)&                theData,
  const Standard_Integer                                theNum,
  Handle(Interface_Check)&                              theAch,
  const Handle(StepVisual_RepositionedTessellatedItem)& theEnt) const
{
  // --- Number of Parameter Control ---
  if (!theData->CheckNbParams(theNum, 2, theAch, "tessellated_item"))
    return;

  // --- inherited field : name ---
  Handle(TCollection_HAsciiString) aName;
  theData->ReadString(theNum, 1, "name", theAch, aName);
  // --- inherited field : location ---
  Handle(StepGeom_Axis2Placement3d) aLocation;
  theData->ReadEntity(theNum,
                      2,
                      "location",
                      theAch,
                      STANDARD_TYPE(StepGeom_Axis2Placement3d),
                      aLocation);

  //--- Initialisation of the read entity ---
  theEnt->Init(aName, aLocation);
}

//=================================================================================================

void RWStepVisual_RWRepositionedTessellatedItem::WriteStep(
  StepData_StepWriter&                                  theSW,
  const Handle(StepVisual_RepositionedTessellatedItem)& theEnt) const
{
  // --- inherited field name ---
  theSW.Send(theEnt->Name());
  theSW.Send(theEnt->Location());
}
