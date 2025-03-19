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

#include "RWStepKinematics_RWGearPairValue.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_GearPairValue.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <Standard_Real.hxx>

//=================================================================================================

RWStepKinematics_RWGearPairValue::RWStepKinematics_RWGearPairValue() {}

//=================================================================================================

void RWStepKinematics_RWGearPairValue::ReadStep(
  const Handle(StepData_StepReaderData)&      theData,
  const Standard_Integer                      theNum,
  Handle(Interface_Check)&                    theArch,
  const Handle(StepKinematics_GearPairValue)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 3, theArch, "gear_pair_value"))
    return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);

  // Inherited fields of PairValue

  Handle(StepKinematics_KinematicPair) aPairValue_AppliesToPair;
  theData->ReadEntity(theNum,
                      2,
                      "pair_value.applies_to_pair",
                      theArch,
                      STANDARD_TYPE(StepKinematics_KinematicPair),
                      aPairValue_AppliesToPair);

  // Own fields of GearPairValue

  Standard_Real aActualRotation1;
  theData->ReadReal(theNum, 3, "actual_rotation1", theArch, aActualRotation1);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name, aPairValue_AppliesToPair, aActualRotation1);
}

//=================================================================================================

void RWStepKinematics_RWGearPairValue::WriteStep(
  StepData_StepWriter&                        theSW,
  const Handle(StepKinematics_GearPairValue)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of PairValue

  theSW.Send(theEnt->AppliesToPair());

  // Own fields of GearPairValue

  theSW.Send(theEnt->ActualRotation1());
}

//=================================================================================================

void RWStepKinematics_RWGearPairValue::Share(const Handle(StepKinematics_GearPairValue)& theEnt,
                                             Interface_EntityIterator&                   iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of PairValue

  iter.AddItem(theEnt->StepKinematics_PairValue::AppliesToPair());

  // Own fields of GearPairValue
}
