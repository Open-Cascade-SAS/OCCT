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

#include <StepKinematics_GearPairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_GearPairWithRange, StepKinematics_GearPair)

//=================================================================================================

StepKinematics_GearPairWithRange::StepKinematics_GearPairWithRange()
{
  defLowerLimitActualRotation1 = Standard_False;
  defUpperLimitActualRotation1 = Standard_False;
}

//=================================================================================================

void StepKinematics_GearPairWithRange::Init(
  const Handle(TCollection_HAsciiString)&      theRepresentationItem_Name,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Name,
  const Standard_Boolean                       hasItemDefinedTransformation_Description,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Description,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem1,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem2,
  const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
  const Standard_Real                          theGearPair_RadiusFirstLink,
  const Standard_Real                          theGearPair_RadiusSecondLink,
  const Standard_Real                          theGearPair_Bevel,
  const Standard_Real                          theGearPair_HelicalAngle,
  const Standard_Real                          theGearPair_GearRatio,
  const Standard_Boolean                       hasLowerLimitActualRotation1,
  const Standard_Real                          theLowerLimitActualRotation1,
  const Standard_Boolean                       hasUpperLimitActualRotation1,
  const Standard_Real                          theUpperLimitActualRotation1)
{
  StepKinematics_GearPair::Init(theRepresentationItem_Name,
                                theItemDefinedTransformation_Name,
                                hasItemDefinedTransformation_Description,
                                theItemDefinedTransformation_Description,
                                theItemDefinedTransformation_TransformItem1,
                                theItemDefinedTransformation_TransformItem2,
                                theKinematicPair_Joint,
                                theGearPair_RadiusFirstLink,
                                theGearPair_RadiusSecondLink,
                                theGearPair_Bevel,
                                theGearPair_HelicalAngle,
                                theGearPair_GearRatio);

  defLowerLimitActualRotation1 = hasLowerLimitActualRotation1;
  if (defLowerLimitActualRotation1)
  {
    myLowerLimitActualRotation1 = theLowerLimitActualRotation1;
  }
  else
    myLowerLimitActualRotation1 = 0;

  defUpperLimitActualRotation1 = hasUpperLimitActualRotation1;
  if (defUpperLimitActualRotation1)
  {
    myUpperLimitActualRotation1 = theUpperLimitActualRotation1;
  }
  else
    myUpperLimitActualRotation1 = 0;
}

//=================================================================================================

Standard_Real StepKinematics_GearPairWithRange::LowerLimitActualRotation1() const
{
  return myLowerLimitActualRotation1;
}

//=================================================================================================

void StepKinematics_GearPairWithRange::SetLowerLimitActualRotation1(
  const Standard_Real theLowerLimitActualRotation1)
{
  myLowerLimitActualRotation1 = theLowerLimitActualRotation1;
}

//=================================================================================================

Standard_Boolean StepKinematics_GearPairWithRange::HasLowerLimitActualRotation1() const
{
  return defLowerLimitActualRotation1;
}

//=================================================================================================

Standard_Real StepKinematics_GearPairWithRange::UpperLimitActualRotation1() const
{
  return myUpperLimitActualRotation1;
}

//=================================================================================================

void StepKinematics_GearPairWithRange::SetUpperLimitActualRotation1(
  const Standard_Real theUpperLimitActualRotation1)
{
  myUpperLimitActualRotation1 = theUpperLimitActualRotation1;
}

//=================================================================================================

Standard_Boolean StepKinematics_GearPairWithRange::HasUpperLimitActualRotation1() const
{
  return defUpperLimitActualRotation1;
}
