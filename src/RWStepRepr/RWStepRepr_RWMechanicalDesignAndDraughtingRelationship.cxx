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

#include <Standard_Type.hxx>
#include <Interface_EntityIterator.hxx>
#include "RWStepRepr_RWMechanicalDesignAndDraughtingRelationship.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_MechanicalDesignAndDraughtingRelationship.hxx>

RWStepRepr_RWMechanicalDesignAndDraughtingRelationship::
  RWStepRepr_RWMechanicalDesignAndDraughtingRelationship()
{
}

void RWStepRepr_RWMechanicalDesignAndDraughtingRelationship::ReadStep(
  const Handle(StepData_StepReaderData)&                            theData,
  const Standard_Integer                                            theNum,
  Handle(Interface_Check)&                                          theAch,
  const Handle(StepRepr_MechanicalDesignAndDraughtingRelationship)& theEnt) const
{
  // Number of Parameter Control
  if (!theData->CheckNbParams(theNum, 4, theAch, "mechanical_design_and_draughting_relationship"))
    return;

  // Inherited field : name
  Handle(TCollection_HAsciiString) aName;
  theData->ReadString(theNum, 1, "name", theAch, aName);

  // Inherited field : description
  Handle(TCollection_HAsciiString) aDescription;
  if (theData->IsParamDefined(theNum, 2))
  {
    theData->ReadString(theNum, 2, "description", theAch, aDescription);
  }
  // Inherited field : rep_1
  Handle(StepRepr_Representation) aRep1;
  theData->ReadEntity(theNum, 3, "rep_1", theAch, STANDARD_TYPE(StepRepr_Representation), aRep1);

  // Inherited field : rep_2
  Handle(StepRepr_Representation) aRep2;
  theData->ReadEntity(theNum, 4, "rep_2", theAch, STANDARD_TYPE(StepRepr_Representation), aRep2);

  // Initialisation of the read entity
  theEnt->Init(aName, aDescription, aRep1, aRep2);
}

void RWStepRepr_RWMechanicalDesignAndDraughtingRelationship::WriteStep(
  StepData_StepWriter&                                              theSW,
  const Handle(StepRepr_MechanicalDesignAndDraughtingRelationship)& theEnt) const
{
  // Inherited field : name
  theSW.Send(theEnt->Name());

  // Inherited field : description
  theSW.Send(theEnt->Description());

  // Inherited field : rep_1
  theSW.Send(theEnt->Rep1());

  // Inherited field : rep_2
  theSW.Send(theEnt->Rep2());
}

void RWStepRepr_RWMechanicalDesignAndDraughtingRelationship::Share(
  const Handle(StepRepr_MechanicalDesignAndDraughtingRelationship)& theEnt,
  Interface_EntityIterator&                                         theIter) const
{
  theIter.GetOneItem(theEnt->Rep1());
  theIter.GetOneItem(theEnt->Rep2());
}
