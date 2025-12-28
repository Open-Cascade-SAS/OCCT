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

#include <StepKinematics_RackAndPinionPairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_RackAndPinionPairWithRange,
                           StepKinematics_RackAndPinionPair)

//=================================================================================================

StepKinematics_RackAndPinionPairWithRange::StepKinematics_RackAndPinionPairWithRange()
{
  defLowerLimitRackDisplacement = false;
  defUpperLimitRackDisplacement = false;
}

//=================================================================================================

void StepKinematics_RackAndPinionPairWithRange::Init(
  const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
  const bool                                        hasItemDefinedTransformation_Description,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
  const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
  const double                                      theRackAndPinionPair_PinionRadius,
  const bool                                        hasLowerLimitRackDisplacement,
  const double                                      theLowerLimitRackDisplacement,
  const bool                                        hasUpperLimitRackDisplacement,
  const double                                      theUpperLimitRackDisplacement)
{
  StepKinematics_RackAndPinionPair::Init(theRepresentationItem_Name,
                                         theItemDefinedTransformation_Name,
                                         hasItemDefinedTransformation_Description,
                                         theItemDefinedTransformation_Description,
                                         theItemDefinedTransformation_TransformItem1,
                                         theItemDefinedTransformation_TransformItem2,
                                         theKinematicPair_Joint,
                                         theRackAndPinionPair_PinionRadius);

  defLowerLimitRackDisplacement = hasLowerLimitRackDisplacement;
  if (defLowerLimitRackDisplacement)
  {
    myLowerLimitRackDisplacement = theLowerLimitRackDisplacement;
  }
  else
    myLowerLimitRackDisplacement = 0;

  defUpperLimitRackDisplacement = hasUpperLimitRackDisplacement;
  if (defUpperLimitRackDisplacement)
  {
    myUpperLimitRackDisplacement = theUpperLimitRackDisplacement;
  }
  else
    myUpperLimitRackDisplacement = 0;
}

//=================================================================================================

double StepKinematics_RackAndPinionPairWithRange::LowerLimitRackDisplacement() const
{
  return myLowerLimitRackDisplacement;
}

//=================================================================================================

void StepKinematics_RackAndPinionPairWithRange::SetLowerLimitRackDisplacement(
  const double theLowerLimitRackDisplacement)
{
  myLowerLimitRackDisplacement = theLowerLimitRackDisplacement;
}

//=================================================================================================

bool StepKinematics_RackAndPinionPairWithRange::HasLowerLimitRackDisplacement() const
{
  return defLowerLimitRackDisplacement;
}

//=================================================================================================

double StepKinematics_RackAndPinionPairWithRange::UpperLimitRackDisplacement() const
{
  return myUpperLimitRackDisplacement;
}

//=================================================================================================

void StepKinematics_RackAndPinionPairWithRange::SetUpperLimitRackDisplacement(
  const double theUpperLimitRackDisplacement)
{
  myUpperLimitRackDisplacement = theUpperLimitRackDisplacement;
}

//=================================================================================================

bool StepKinematics_RackAndPinionPairWithRange::HasUpperLimitRackDisplacement() const
{
  return defUpperLimitRackDisplacement;
}
