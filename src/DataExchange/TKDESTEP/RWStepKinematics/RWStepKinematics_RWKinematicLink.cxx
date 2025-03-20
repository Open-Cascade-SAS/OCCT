// Created on : Sat May 02 12:41:15 2020
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include "RWStepKinematics_RWKinematicLink.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_KinematicLink.hxx>
#include <TCollection_HAsciiString.hxx>

//=================================================================================================

RWStepKinematics_RWKinematicLink::RWStepKinematics_RWKinematicLink() {}

//=================================================================================================

void RWStepKinematics_RWKinematicLink::ReadStep(
  const Handle(StepData_StepReaderData)&      theData,
  const Standard_Integer                      theNum,
  Handle(Interface_Check)&                    theArch,
  const Handle(StepKinematics_KinematicLink)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 1, theArch, "kinematic_link"))
    return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name);
}

//=================================================================================================

void RWStepKinematics_RWKinematicLink::WriteStep(
  StepData_StepWriter&                        theSW,
  const Handle(StepKinematics_KinematicLink)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());
}

//=================================================================================================

void RWStepKinematics_RWKinematicLink::Share(const Handle(StepKinematics_KinematicLink)& /*theEnt*/,
                                             Interface_EntityIterator& /*iter*/) const
{

  // Inherited fields of RepresentationItem
}
