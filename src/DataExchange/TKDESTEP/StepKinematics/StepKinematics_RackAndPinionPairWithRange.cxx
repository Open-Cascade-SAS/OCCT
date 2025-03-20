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
  defLowerLimitRackDisplacement = Standard_False;
  defUpperLimitRackDisplacement = Standard_False;
}

//=================================================================================================

void StepKinematics_RackAndPinionPairWithRange::Init(
  const Handle(TCollection_HAsciiString)&      theRepresentationItem_Name,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Name,
  const Standard_Boolean                       hasItemDefinedTransformation_Description,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Description,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem1,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem2,
  const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
  const Standard_Real                          theRackAndPinionPair_PinionRadius,
  const Standard_Boolean                       hasLowerLimitRackDisplacement,
  const Standard_Real                          theLowerLimitRackDisplacement,
  const Standard_Boolean                       hasUpperLimitRackDisplacement,
  const Standard_Real                          theUpperLimitRackDisplacement)
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

Standard_Real StepKinematics_RackAndPinionPairWithRange::LowerLimitRackDisplacement() const
{
  return myLowerLimitRackDisplacement;
}

//=================================================================================================

void StepKinematics_RackAndPinionPairWithRange::SetLowerLimitRackDisplacement(
  const Standard_Real theLowerLimitRackDisplacement)
{
  myLowerLimitRackDisplacement = theLowerLimitRackDisplacement;
}

//=================================================================================================

Standard_Boolean StepKinematics_RackAndPinionPairWithRange::HasLowerLimitRackDisplacement() const
{
  return defLowerLimitRackDisplacement;
}

//=================================================================================================

Standard_Real StepKinematics_RackAndPinionPairWithRange::UpperLimitRackDisplacement() const
{
  return myUpperLimitRackDisplacement;
}

//=================================================================================================

void StepKinematics_RackAndPinionPairWithRange::SetUpperLimitRackDisplacement(
  const Standard_Real theUpperLimitRackDisplacement)
{
  myUpperLimitRackDisplacement = theUpperLimitRackDisplacement;
}

//=================================================================================================

Standard_Boolean StepKinematics_RackAndPinionPairWithRange::HasUpperLimitRackDisplacement() const
{
  return defUpperLimitRackDisplacement;
}
