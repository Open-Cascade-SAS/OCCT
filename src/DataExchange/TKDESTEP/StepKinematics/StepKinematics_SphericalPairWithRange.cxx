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

#include <StepKinematics_SphericalPairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_SphericalPairWithRange, StepKinematics_SphericalPair)

//=================================================================================================

StepKinematics_SphericalPairWithRange::StepKinematics_SphericalPairWithRange()
{
  defLowerLimitYaw   = Standard_False;
  defUpperLimitYaw   = Standard_False;
  defLowerLimitPitch = Standard_False;
  defUpperLimitPitch = Standard_False;
  defLowerLimitRoll  = Standard_False;
  defUpperLimitRoll  = Standard_False;
}

//=================================================================================================

void StepKinematics_SphericalPairWithRange::Init(
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
  const Standard_Boolean                       hasLowerLimitYaw,
  const Standard_Real                          theLowerLimitYaw,
  const Standard_Boolean                       hasUpperLimitYaw,
  const Standard_Real                          theUpperLimitYaw,
  const Standard_Boolean                       hasLowerLimitPitch,
  const Standard_Real                          theLowerLimitPitch,
  const Standard_Boolean                       hasUpperLimitPitch,
  const Standard_Real                          theUpperLimitPitch,
  const Standard_Boolean                       hasLowerLimitRoll,
  const Standard_Real                          theLowerLimitRoll,
  const Standard_Boolean                       hasUpperLimitRoll,
  const Standard_Real                          theUpperLimitRoll)
{
  StepKinematics_SphericalPair::Init(theRepresentationItem_Name,
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

  defLowerLimitYaw = hasLowerLimitYaw;
  if (defLowerLimitYaw)
  {
    myLowerLimitYaw = theLowerLimitYaw;
  }
  else
    myLowerLimitYaw = 0;

  defUpperLimitYaw = hasUpperLimitYaw;
  if (defUpperLimitYaw)
  {
    myUpperLimitYaw = theUpperLimitYaw;
  }
  else
    myUpperLimitYaw = 0;

  defLowerLimitPitch = hasLowerLimitPitch;
  if (defLowerLimitPitch)
  {
    myLowerLimitPitch = theLowerLimitPitch;
  }
  else
    myLowerLimitPitch = 0;

  defUpperLimitPitch = hasUpperLimitPitch;
  if (defUpperLimitPitch)
  {
    myUpperLimitPitch = theUpperLimitPitch;
  }
  else
    myUpperLimitPitch = 0;

  defLowerLimitRoll = hasLowerLimitRoll;
  if (defLowerLimitRoll)
  {
    myLowerLimitRoll = theLowerLimitRoll;
  }
  else
    myLowerLimitRoll = 0;

  defUpperLimitRoll = hasUpperLimitRoll;
  if (defUpperLimitRoll)
  {
    myUpperLimitRoll = theUpperLimitRoll;
  }
  else
    myUpperLimitRoll = 0;
}

//=================================================================================================

Standard_Real StepKinematics_SphericalPairWithRange::LowerLimitYaw() const
{
  return myLowerLimitYaw;
}

//=================================================================================================

void StepKinematics_SphericalPairWithRange::SetLowerLimitYaw(const Standard_Real theLowerLimitYaw)
{
  myLowerLimitYaw = theLowerLimitYaw;
}

//=================================================================================================

Standard_Boolean StepKinematics_SphericalPairWithRange::HasLowerLimitYaw() const
{
  return defLowerLimitYaw;
}

//=================================================================================================

Standard_Real StepKinematics_SphericalPairWithRange::UpperLimitYaw() const
{
  return myUpperLimitYaw;
}

//=================================================================================================

void StepKinematics_SphericalPairWithRange::SetUpperLimitYaw(const Standard_Real theUpperLimitYaw)
{
  myUpperLimitYaw = theUpperLimitYaw;
}

//=================================================================================================

Standard_Boolean StepKinematics_SphericalPairWithRange::HasUpperLimitYaw() const
{
  return defUpperLimitYaw;
}

//=================================================================================================

Standard_Real StepKinematics_SphericalPairWithRange::LowerLimitPitch() const
{
  return myLowerLimitPitch;
}

//=================================================================================================

void StepKinematics_SphericalPairWithRange::SetLowerLimitPitch(
  const Standard_Real theLowerLimitPitch)
{
  myLowerLimitPitch = theLowerLimitPitch;
}

//=================================================================================================

Standard_Boolean StepKinematics_SphericalPairWithRange::HasLowerLimitPitch() const
{
  return defLowerLimitPitch;
}

//=================================================================================================

Standard_Real StepKinematics_SphericalPairWithRange::UpperLimitPitch() const
{
  return myUpperLimitPitch;
}

//=================================================================================================

void StepKinematics_SphericalPairWithRange::SetUpperLimitPitch(
  const Standard_Real theUpperLimitPitch)
{
  myUpperLimitPitch = theUpperLimitPitch;
}

//=================================================================================================

Standard_Boolean StepKinematics_SphericalPairWithRange::HasUpperLimitPitch() const
{
  return defUpperLimitPitch;
}

//=================================================================================================

Standard_Real StepKinematics_SphericalPairWithRange::LowerLimitRoll() const
{
  return myLowerLimitRoll;
}

//=================================================================================================

void StepKinematics_SphericalPairWithRange::SetLowerLimitRoll(const Standard_Real theLowerLimitRoll)
{
  myLowerLimitRoll = theLowerLimitRoll;
}

//=================================================================================================

Standard_Boolean StepKinematics_SphericalPairWithRange::HasLowerLimitRoll() const
{
  return defLowerLimitRoll;
}

//=================================================================================================

Standard_Real StepKinematics_SphericalPairWithRange::UpperLimitRoll() const
{
  return myUpperLimitRoll;
}

//=================================================================================================

void StepKinematics_SphericalPairWithRange::SetUpperLimitRoll(const Standard_Real theUpperLimitRoll)
{
  myUpperLimitRoll = theUpperLimitRoll;
}

//=================================================================================================

Standard_Boolean StepKinematics_SphericalPairWithRange::HasUpperLimitRoll() const
{
  return defUpperLimitRoll;
}
