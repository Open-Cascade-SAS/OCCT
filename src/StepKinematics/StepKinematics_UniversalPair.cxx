// Created on : Sat May 02 12:41:16 2020
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

#include <StepKinematics_UniversalPair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_UniversalPair, StepKinematics_LowOrderKinematicPair)

//=================================================================================================

StepKinematics_UniversalPair::StepKinematics_UniversalPair()
{
  defInputSkewAngle = Standard_False;
}

//=================================================================================================

void StepKinematics_UniversalPair::Init(
  const Handle(TCollection_HAsciiString)&      theRepresentationItem_Name,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Name,
  const Standard_Boolean                       hasItemDefinedTransformation_Description,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Description,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem1,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem2,
  const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
  const Standard_Boolean                       theLowOrderKinematicPair_TX,
  const Standard_Boolean                       theLowOrderKinematicPair_TY,
  const Standard_Boolean                       theLowOrderKinematicPair_TZ,
  const Standard_Boolean                       theLowOrderKinematicPair_RX,
  const Standard_Boolean                       theLowOrderKinematicPair_RY,
  const Standard_Boolean                       theLowOrderKinematicPair_RZ,
  const Standard_Boolean                       hasInputSkewAngle,
  const Standard_Real                          theInputSkewAngle)
{
  StepKinematics_LowOrderKinematicPair::Init(theRepresentationItem_Name,
                                             theItemDefinedTransformation_Name,
                                             hasItemDefinedTransformation_Description,
                                             theItemDefinedTransformation_Description,
                                             theItemDefinedTransformation_TransformItem1,
                                             theItemDefinedTransformation_TransformItem2,
                                             theKinematicPair_Joint,
                                             theLowOrderKinematicPair_TX,
                                             theLowOrderKinematicPair_TY,
                                             theLowOrderKinematicPair_TZ,
                                             theLowOrderKinematicPair_RX,
                                             theLowOrderKinematicPair_RY,
                                             theLowOrderKinematicPair_RZ);

  defInputSkewAngle = hasInputSkewAngle;
  if (defInputSkewAngle)
  {
    myInputSkewAngle = theInputSkewAngle;
  }
  else
    myInputSkewAngle = 0;
}

//=================================================================================================

Standard_Real StepKinematics_UniversalPair::InputSkewAngle() const
{
  return myInputSkewAngle;
}

//=================================================================================================

void StepKinematics_UniversalPair::SetInputSkewAngle(const Standard_Real theInputSkewAngle)
{
  myInputSkewAngle = theInputSkewAngle;
}

//=================================================================================================

Standard_Boolean StepKinematics_UniversalPair::HasInputSkewAngle() const
{
  return defInputSkewAngle;
}
