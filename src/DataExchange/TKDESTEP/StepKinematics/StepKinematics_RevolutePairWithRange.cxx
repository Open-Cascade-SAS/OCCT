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

#include <StepKinematics_RevolutePairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_RevolutePairWithRange, StepKinematics_RevolutePair)

//=================================================================================================

StepKinematics_RevolutePairWithRange::StepKinematics_RevolutePairWithRange()
{
  defLowerLimitActualRotation = false;
  defUpperLimitActualRotation = false;
}

//=================================================================================================

void StepKinematics_RevolutePairWithRange::Init(
  const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
  const bool                       hasItemDefinedTransformation_Description,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
  const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
  const bool                       theLowOrderKinematicPair_TX,
  const bool                       theLowOrderKinematicPair_TY,
  const bool                       theLowOrderKinematicPair_TZ,
  const bool                       theLowOrderKinematicPair_RX,
  const bool                       theLowOrderKinematicPair_RY,
  const bool                       theLowOrderKinematicPair_RZ,
  const bool                       hasLowerLimitActualRotation,
  const double                          theLowerLimitActualRotation,
  const bool                       hasUpperLimitActualRotation,
  const double                          theUpperLimitActualRotation)
{
  StepKinematics_RevolutePair::Init(theRepresentationItem_Name,
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

  defLowerLimitActualRotation = hasLowerLimitActualRotation;
  if (defLowerLimitActualRotation)
  {
    myLowerLimitActualRotation = theLowerLimitActualRotation;
  }
  else
    myLowerLimitActualRotation = 0;

  defUpperLimitActualRotation = hasUpperLimitActualRotation;
  if (defUpperLimitActualRotation)
  {
    myUpperLimitActualRotation = theUpperLimitActualRotation;
  }
  else
    myUpperLimitActualRotation = 0;
}

//=================================================================================================

double StepKinematics_RevolutePairWithRange::LowerLimitActualRotation() const
{
  return myLowerLimitActualRotation;
}

//=================================================================================================

void StepKinematics_RevolutePairWithRange::SetLowerLimitActualRotation(
  const double theLowerLimitActualRotation)
{
  myLowerLimitActualRotation = theLowerLimitActualRotation;
}

//=================================================================================================

bool StepKinematics_RevolutePairWithRange::HasLowerLimitActualRotation() const
{
  return defLowerLimitActualRotation;
}

//=================================================================================================

double StepKinematics_RevolutePairWithRange::UpperLimitActualRotation() const
{
  return myUpperLimitActualRotation;
}

//=================================================================================================

void StepKinematics_RevolutePairWithRange::SetUpperLimitActualRotation(
  const double theUpperLimitActualRotation)
{
  myUpperLimitActualRotation = theUpperLimitActualRotation;
}

//=================================================================================================

bool StepKinematics_RevolutePairWithRange::HasUpperLimitActualRotation() const
{
  return defUpperLimitActualRotation;
}
