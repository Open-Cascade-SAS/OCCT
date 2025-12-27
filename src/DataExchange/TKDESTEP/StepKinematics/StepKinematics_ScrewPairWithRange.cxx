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

#include <StepKinematics_ScrewPairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_ScrewPairWithRange, StepKinematics_ScrewPair)

//=================================================================================================

StepKinematics_ScrewPairWithRange::StepKinematics_ScrewPairWithRange()
{
  defLowerLimitActualRotation = false;
  defUpperLimitActualRotation = false;
}

//=================================================================================================

void StepKinematics_ScrewPairWithRange::Init(
  const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
  const bool                       hasItemDefinedTransformation_Description,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
  const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
  const double                          theScrewPair_Pitch,
  const bool                       hasLowerLimitActualRotation,
  const double                          theLowerLimitActualRotation,
  const bool                       hasUpperLimitActualRotation,
  const double                          theUpperLimitActualRotation)
{
  StepKinematics_ScrewPair::Init(theRepresentationItem_Name,
                                 theItemDefinedTransformation_Name,
                                 hasItemDefinedTransformation_Description,
                                 theItemDefinedTransformation_Description,
                                 theItemDefinedTransformation_TransformItem1,
                                 theItemDefinedTransformation_TransformItem2,
                                 theKinematicPair_Joint,
                                 theScrewPair_Pitch);

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

double StepKinematics_ScrewPairWithRange::LowerLimitActualRotation() const
{
  return myLowerLimitActualRotation;
}

//=================================================================================================

void StepKinematics_ScrewPairWithRange::SetLowerLimitActualRotation(
  const double theLowerLimitActualRotation)
{
  myLowerLimitActualRotation = theLowerLimitActualRotation;
}

//=================================================================================================

bool StepKinematics_ScrewPairWithRange::HasLowerLimitActualRotation() const
{
  return defLowerLimitActualRotation;
}

//=================================================================================================

double StepKinematics_ScrewPairWithRange::UpperLimitActualRotation() const
{
  return myUpperLimitActualRotation;
}

//=================================================================================================

void StepKinematics_ScrewPairWithRange::SetUpperLimitActualRotation(
  const double theUpperLimitActualRotation)
{
  myUpperLimitActualRotation = theUpperLimitActualRotation;
}

//=================================================================================================

bool StepKinematics_ScrewPairWithRange::HasUpperLimitActualRotation() const
{
  return defUpperLimitActualRotation;
}
