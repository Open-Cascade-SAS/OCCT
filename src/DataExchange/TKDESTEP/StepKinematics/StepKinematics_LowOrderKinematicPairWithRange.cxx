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

#include <StepKinematics_LowOrderKinematicPairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_LowOrderKinematicPairWithRange,
                           StepKinematics_LowOrderKinematicPair)

//=================================================================================================

StepKinematics_LowOrderKinematicPairWithRange::StepKinematics_LowOrderKinematicPairWithRange()
{
  defLowerLimitActualRotationX    = false;
  defUpperLimitActualRotationX    = false;
  defLowerLimitActualRotationY    = false;
  defUpperLimitActualRotationY    = false;
  defLowerLimitActualRotationZ    = false;
  defUpperLimitActualRotationZ    = false;
  defLowerLimitActualTranslationX = false;
  defUpperLimitActualTranslationX = false;
  defLowerLimitActualTranslationY = false;
  defUpperLimitActualTranslationY = false;
  defLowerLimitActualTranslationZ = false;
  defUpperLimitActualTranslationZ = false;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::Init(
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
  const bool                       hasLowerLimitActualRotationX,
  const double                          theLowerLimitActualRotationX,
  const bool                       hasUpperLimitActualRotationX,
  const double                          theUpperLimitActualRotationX,
  const bool                       hasLowerLimitActualRotationY,
  const double                          theLowerLimitActualRotationY,
  const bool                       hasUpperLimitActualRotationY,
  const double                          theUpperLimitActualRotationY,
  const bool                       hasLowerLimitActualRotationZ,
  const double                          theLowerLimitActualRotationZ,
  const bool                       hasUpperLimitActualRotationZ,
  const double                          theUpperLimitActualRotationZ,
  const bool                       hasLowerLimitActualTranslationX,
  const double                          theLowerLimitActualTranslationX,
  const bool                       hasUpperLimitActualTranslationX,
  const double                          theUpperLimitActualTranslationX,
  const bool                       hasLowerLimitActualTranslationY,
  const double                          theLowerLimitActualTranslationY,
  const bool                       hasUpperLimitActualTranslationY,
  const double                          theUpperLimitActualTranslationY,
  const bool                       hasLowerLimitActualTranslationZ,
  const double                          theLowerLimitActualTranslationZ,
  const bool                       hasUpperLimitActualTranslationZ,
  const double                          theUpperLimitActualTranslationZ)
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

  defLowerLimitActualRotationX = hasLowerLimitActualRotationX;
  if (defLowerLimitActualRotationX)
  {
    myLowerLimitActualRotationX = theLowerLimitActualRotationX;
  }
  else
    myLowerLimitActualRotationX = 0;

  defUpperLimitActualRotationX = hasUpperLimitActualRotationX;
  if (defUpperLimitActualRotationX)
  {
    myUpperLimitActualRotationX = theUpperLimitActualRotationX;
  }
  else
    myUpperLimitActualRotationX = 0;

  defLowerLimitActualRotationY = hasLowerLimitActualRotationY;
  if (defLowerLimitActualRotationY)
  {
    myLowerLimitActualRotationY = theLowerLimitActualRotationY;
  }
  else
    myLowerLimitActualRotationY = 0;

  defUpperLimitActualRotationY = hasUpperLimitActualRotationY;
  if (defUpperLimitActualRotationY)
  {
    myUpperLimitActualRotationY = theUpperLimitActualRotationY;
  }
  else
    myUpperLimitActualRotationY = 0;

  defLowerLimitActualRotationZ = hasLowerLimitActualRotationZ;
  if (defLowerLimitActualRotationZ)
  {
    myLowerLimitActualRotationZ = theLowerLimitActualRotationZ;
  }
  else
    myLowerLimitActualRotationZ = 0;

  defUpperLimitActualRotationZ = hasUpperLimitActualRotationZ;
  if (defUpperLimitActualRotationZ)
  {
    myUpperLimitActualRotationZ = theUpperLimitActualRotationZ;
  }
  else
    myUpperLimitActualRotationZ = 0;

  defLowerLimitActualTranslationX = hasLowerLimitActualTranslationX;
  if (defLowerLimitActualTranslationX)
  {
    myLowerLimitActualTranslationX = theLowerLimitActualTranslationX;
  }
  else
    myLowerLimitActualTranslationX = 0;

  defUpperLimitActualTranslationX = hasUpperLimitActualTranslationX;
  if (defUpperLimitActualTranslationX)
  {
    myUpperLimitActualTranslationX = theUpperLimitActualTranslationX;
  }
  else
    myUpperLimitActualTranslationX = 0;

  defLowerLimitActualTranslationY = hasLowerLimitActualTranslationY;
  if (defLowerLimitActualTranslationY)
  {
    myLowerLimitActualTranslationY = theLowerLimitActualTranslationY;
  }
  else
    myLowerLimitActualTranslationY = 0;

  defUpperLimitActualTranslationY = hasUpperLimitActualTranslationY;
  if (defUpperLimitActualTranslationY)
  {
    myUpperLimitActualTranslationY = theUpperLimitActualTranslationY;
  }
  else
    myUpperLimitActualTranslationY = 0;

  defLowerLimitActualTranslationZ = hasLowerLimitActualTranslationZ;
  if (defLowerLimitActualTranslationZ)
  {
    myLowerLimitActualTranslationZ = theLowerLimitActualTranslationZ;
  }
  else
    myLowerLimitActualTranslationZ = 0;

  defUpperLimitActualTranslationZ = hasUpperLimitActualTranslationZ;
  if (defUpperLimitActualTranslationZ)
  {
    myUpperLimitActualTranslationZ = theUpperLimitActualTranslationZ;
  }
  else
    myUpperLimitActualTranslationZ = 0;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualRotationX() const
{
  return myLowerLimitActualRotationX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualRotationX(
  const double theLowerLimitActualRotationX)
{
  myLowerLimitActualRotationX = theLowerLimitActualRotationX;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualRotationX() const
{
  return defLowerLimitActualRotationX;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualRotationX() const
{
  return myUpperLimitActualRotationX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualRotationX(
  const double theUpperLimitActualRotationX)
{
  myUpperLimitActualRotationX = theUpperLimitActualRotationX;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualRotationX() const
{
  return defUpperLimitActualRotationX;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualRotationY() const
{
  return myLowerLimitActualRotationY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualRotationY(
  const double theLowerLimitActualRotationY)
{
  myLowerLimitActualRotationY = theLowerLimitActualRotationY;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualRotationY() const
{
  return defLowerLimitActualRotationY;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualRotationY() const
{
  return myUpperLimitActualRotationY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualRotationY(
  const double theUpperLimitActualRotationY)
{
  myUpperLimitActualRotationY = theUpperLimitActualRotationY;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualRotationY() const
{
  return defUpperLimitActualRotationY;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualRotationZ() const
{
  return myLowerLimitActualRotationZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualRotationZ(
  const double theLowerLimitActualRotationZ)
{
  myLowerLimitActualRotationZ = theLowerLimitActualRotationZ;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualRotationZ() const
{
  return defLowerLimitActualRotationZ;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualRotationZ() const
{
  return myUpperLimitActualRotationZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualRotationZ(
  const double theUpperLimitActualRotationZ)
{
  myUpperLimitActualRotationZ = theUpperLimitActualRotationZ;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualRotationZ() const
{
  return defUpperLimitActualRotationZ;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualTranslationX() const
{
  return myLowerLimitActualTranslationX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualTranslationX(
  const double theLowerLimitActualTranslationX)
{
  myLowerLimitActualTranslationX = theLowerLimitActualTranslationX;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualTranslationX()
  const
{
  return defLowerLimitActualTranslationX;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualTranslationX() const
{
  return myUpperLimitActualTranslationX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualTranslationX(
  const double theUpperLimitActualTranslationX)
{
  myUpperLimitActualTranslationX = theUpperLimitActualTranslationX;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualTranslationX()
  const
{
  return defUpperLimitActualTranslationX;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualTranslationY() const
{
  return myLowerLimitActualTranslationY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualTranslationY(
  const double theLowerLimitActualTranslationY)
{
  myLowerLimitActualTranslationY = theLowerLimitActualTranslationY;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualTranslationY()
  const
{
  return defLowerLimitActualTranslationY;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualTranslationY() const
{
  return myUpperLimitActualTranslationY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualTranslationY(
  const double theUpperLimitActualTranslationY)
{
  myUpperLimitActualTranslationY = theUpperLimitActualTranslationY;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualTranslationY()
  const
{
  return defUpperLimitActualTranslationY;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualTranslationZ() const
{
  return myLowerLimitActualTranslationZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualTranslationZ(
  const double theLowerLimitActualTranslationZ)
{
  myLowerLimitActualTranslationZ = theLowerLimitActualTranslationZ;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualTranslationZ()
  const
{
  return defLowerLimitActualTranslationZ;
}

//=================================================================================================

double StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualTranslationZ() const
{
  return myUpperLimitActualTranslationZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualTranslationZ(
  const double theUpperLimitActualTranslationZ)
{
  myUpperLimitActualTranslationZ = theUpperLimitActualTranslationZ;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualTranslationZ()
  const
{
  return defUpperLimitActualTranslationZ;
}
