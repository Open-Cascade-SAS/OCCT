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

#include <StepKinematics_UniversalPairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_UniversalPairWithRange, StepKinematics_UniversalPair)

//=================================================================================================

StepKinematics_UniversalPairWithRange::StepKinematics_UniversalPairWithRange()
{
  defLowerLimitFirstRotation  = false;
  defUpperLimitFirstRotation  = false;
  defLowerLimitSecondRotation = false;
  defUpperLimitSecondRotation = false;
}

//=================================================================================================

void StepKinematics_UniversalPairWithRange::Init(
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
  const bool                       hasUniversalPair_InputSkewAngle,
  const double                          theUniversalPair_InputSkewAngle,
  const bool                       hasLowerLimitFirstRotation,
  const double                          theLowerLimitFirstRotation,
  const bool                       hasUpperLimitFirstRotation,
  const double                          theUpperLimitFirstRotation,
  const bool                       hasLowerLimitSecondRotation,
  const double                          theLowerLimitSecondRotation,
  const bool                       hasUpperLimitSecondRotation,
  const double                          theUpperLimitSecondRotation)
{
  StepKinematics_UniversalPair::Init(theRepresentationItem_Name,
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
                                     theLowOrderKinematicPair_RZ,
                                     hasUniversalPair_InputSkewAngle,
                                     theUniversalPair_InputSkewAngle);

  defLowerLimitFirstRotation = hasLowerLimitFirstRotation;
  if (defLowerLimitFirstRotation)
  {
    myLowerLimitFirstRotation = theLowerLimitFirstRotation;
  }
  else
    myLowerLimitFirstRotation = 0;

  defUpperLimitFirstRotation = hasUpperLimitFirstRotation;
  if (defUpperLimitFirstRotation)
  {
    myUpperLimitFirstRotation = theUpperLimitFirstRotation;
  }
  else
    myUpperLimitFirstRotation = 0;

  defLowerLimitSecondRotation = hasLowerLimitSecondRotation;
  if (defLowerLimitSecondRotation)
  {
    myLowerLimitSecondRotation = theLowerLimitSecondRotation;
  }
  else
    myLowerLimitSecondRotation = 0;

  defUpperLimitSecondRotation = hasUpperLimitSecondRotation;
  if (defUpperLimitSecondRotation)
  {
    myUpperLimitSecondRotation = theUpperLimitSecondRotation;
  }
  else
    myUpperLimitSecondRotation = 0;
}

//=================================================================================================

double StepKinematics_UniversalPairWithRange::LowerLimitFirstRotation() const
{
  return myLowerLimitFirstRotation;
}

//=================================================================================================

void StepKinematics_UniversalPairWithRange::SetLowerLimitFirstRotation(
  const double theLowerLimitFirstRotation)
{
  myLowerLimitFirstRotation = theLowerLimitFirstRotation;
}

//=================================================================================================

bool StepKinematics_UniversalPairWithRange::HasLowerLimitFirstRotation() const
{
  return defLowerLimitFirstRotation;
}

//=================================================================================================

double StepKinematics_UniversalPairWithRange::UpperLimitFirstRotation() const
{
  return myUpperLimitFirstRotation;
}

//=================================================================================================

void StepKinematics_UniversalPairWithRange::SetUpperLimitFirstRotation(
  const double theUpperLimitFirstRotation)
{
  myUpperLimitFirstRotation = theUpperLimitFirstRotation;
}

//=================================================================================================

bool StepKinematics_UniversalPairWithRange::HasUpperLimitFirstRotation() const
{
  return defUpperLimitFirstRotation;
}

//=================================================================================================

double StepKinematics_UniversalPairWithRange::LowerLimitSecondRotation() const
{
  return myLowerLimitSecondRotation;
}

//=================================================================================================

void StepKinematics_UniversalPairWithRange::SetLowerLimitSecondRotation(
  const double theLowerLimitSecondRotation)
{
  myLowerLimitSecondRotation = theLowerLimitSecondRotation;
}

//=================================================================================================

bool StepKinematics_UniversalPairWithRange::HasLowerLimitSecondRotation() const
{
  return defLowerLimitSecondRotation;
}

//=================================================================================================

double StepKinematics_UniversalPairWithRange::UpperLimitSecondRotation() const
{
  return myUpperLimitSecondRotation;
}

//=================================================================================================

void StepKinematics_UniversalPairWithRange::SetUpperLimitSecondRotation(
  const double theUpperLimitSecondRotation)
{
  myUpperLimitSecondRotation = theUpperLimitSecondRotation;
}

//=================================================================================================

bool StepKinematics_UniversalPairWithRange::HasUpperLimitSecondRotation() const
{
  return defUpperLimitSecondRotation;
}
