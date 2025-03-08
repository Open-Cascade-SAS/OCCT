// Created on: 2020-05-26
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

#include <StepKinematics_ActuatedKinPairAndOrderKinPair.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepKinematics_ActuatedKinematicPair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_ActuatedKinPairAndOrderKinPair,
                           StepKinematics_KinematicPair)

//=================================================================================================

StepKinematics_ActuatedKinPairAndOrderKinPair::StepKinematics_ActuatedKinPairAndOrderKinPair() {}

//=================================================================================================

void StepKinematics_ActuatedKinPairAndOrderKinPair::Init(
  const Handle(TCollection_HAsciiString)&             theRepresentationItem_Name,
  const Handle(TCollection_HAsciiString)&             theItemDefinedTransformation_Name,
  const Standard_Boolean                              hasItemDefinedTransformation_Description,
  const Handle(TCollection_HAsciiString)&             theItemDefinedTransformation_Description,
  const Handle(StepRepr_RepresentationItem)&          theItemDefinedTransformation_TransformItem1,
  const Handle(StepRepr_RepresentationItem)&          theItemDefinedTransformation_TransformItem2,
  const Handle(StepKinematics_KinematicJoint)&        theJoint,
  const Handle(StepKinematics_ActuatedKinematicPair)& theActuatedKinematicPair,
  const Handle(StepKinematics_KinematicPair)&         theOrderKinematicPair)
{
  StepKinematics_KinematicPair::Init(theRepresentationItem_Name,
                                     theItemDefinedTransformation_Name,
                                     hasItemDefinedTransformation_Description,
                                     theItemDefinedTransformation_Description,
                                     theItemDefinedTransformation_TransformItem1,
                                     theItemDefinedTransformation_TransformItem2,
                                     theJoint);
  SetActuatedKinematicPair(theActuatedKinematicPair);
  SetOrderKinematicPair(theOrderKinematicPair);
}
