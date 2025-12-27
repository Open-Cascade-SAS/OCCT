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
  defLowerLimitActualRotation1 = false;
  defUpperLimitActualRotation1 = false;
}

//=================================================================================================

void StepKinematics_GearPairWithRange::Init(
  const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
  const bool                       hasItemDefinedTransformation_Description,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
  const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
  const double                          theGearPair_RadiusFirstLink,
  const double                          theGearPair_RadiusSecondLink,
  const double                          theGearPair_Bevel,
  const double                          theGearPair_HelicalAngle,
  const double                          theGearPair_GearRatio,
  const bool                       hasLowerLimitActualRotation1,
  const double                          theLowerLimitActualRotation1,
  const bool                       hasUpperLimitActualRotation1,
  const double                          theUpperLimitActualRotation1)
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

double StepKinematics_GearPairWithRange::LowerLimitActualRotation1() const
{
  return myLowerLimitActualRotation1;
}

//=================================================================================================

void StepKinematics_GearPairWithRange::SetLowerLimitActualRotation1(
  const double theLowerLimitActualRotation1)
{
  myLowerLimitActualRotation1 = theLowerLimitActualRotation1;
}

//=================================================================================================

bool StepKinematics_GearPairWithRange::HasLowerLimitActualRotation1() const
{
  return defLowerLimitActualRotation1;
}

//=================================================================================================

double StepKinematics_GearPairWithRange::UpperLimitActualRotation1() const
{
  return myUpperLimitActualRotation1;
}

//=================================================================================================

void StepKinematics_GearPairWithRange::SetUpperLimitActualRotation1(
  const double theUpperLimitActualRotation1)
{
  myUpperLimitActualRotation1 = theUpperLimitActualRotation1;
}

//=================================================================================================

bool StepKinematics_GearPairWithRange::HasUpperLimitActualRotation1() const
{
  return defUpperLimitActualRotation1;
}
