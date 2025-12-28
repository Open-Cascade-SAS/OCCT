// Created on: 2020-06-18
// Created by: PASUKHIN DMITRY
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <StepKinematics_MechanismStateRepresentation.hxx>
#include <StepKinematics_MechanismRepresentation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_MechanismStateRepresentation, StepRepr_Representation)

StepKinematics_MechanismStateRepresentation::StepKinematics_MechanismStateRepresentation() {}

void StepKinematics_MechanismStateRepresentation::Init(
  const occ::handle<TCollection_HAsciiString>&                                      theName,
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>& theItems,
  const occ::handle<StepRepr_RepresentationContext>&         theContextOfItems,
  const occ::handle<StepKinematics_MechanismRepresentation>& theMechanism)
{
  StepRepr_Representation::Init(theName, theItems, theContextOfItems);
  myRepresentedMechanism = theMechanism;
}

void StepKinematics_MechanismStateRepresentation::SetMechanism(
  const occ::handle<StepKinematics_MechanismRepresentation>& theMechanism)
{
  myRepresentedMechanism = theMechanism;
}

occ::handle<StepKinematics_MechanismRepresentation> StepKinematics_MechanismStateRepresentation::
  Mechanism() const
{
  return myRepresentedMechanism;
}