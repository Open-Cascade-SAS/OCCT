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
  defLowerLimitActualRotationX    = Standard_False;
  defUpperLimitActualRotationX    = Standard_False;
  defLowerLimitActualRotationY    = Standard_False;
  defUpperLimitActualRotationY    = Standard_False;
  defLowerLimitActualRotationZ    = Standard_False;
  defUpperLimitActualRotationZ    = Standard_False;
  defLowerLimitActualTranslationX = Standard_False;
  defUpperLimitActualTranslationX = Standard_False;
  defLowerLimitActualTranslationY = Standard_False;
  defUpperLimitActualTranslationY = Standard_False;
  defLowerLimitActualTranslationZ = Standard_False;
  defUpperLimitActualTranslationZ = Standard_False;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::Init(
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
  const Standard_Boolean                       hasLowerLimitActualRotationX,
  const Standard_Real                          theLowerLimitActualRotationX,
  const Standard_Boolean                       hasUpperLimitActualRotationX,
  const Standard_Real                          theUpperLimitActualRotationX,
  const Standard_Boolean                       hasLowerLimitActualRotationY,
  const Standard_Real                          theLowerLimitActualRotationY,
  const Standard_Boolean                       hasUpperLimitActualRotationY,
  const Standard_Real                          theUpperLimitActualRotationY,
  const Standard_Boolean                       hasLowerLimitActualRotationZ,
  const Standard_Real                          theLowerLimitActualRotationZ,
  const Standard_Boolean                       hasUpperLimitActualRotationZ,
  const Standard_Real                          theUpperLimitActualRotationZ,
  const Standard_Boolean                       hasLowerLimitActualTranslationX,
  const Standard_Real                          theLowerLimitActualTranslationX,
  const Standard_Boolean                       hasUpperLimitActualTranslationX,
  const Standard_Real                          theUpperLimitActualTranslationX,
  const Standard_Boolean                       hasLowerLimitActualTranslationY,
  const Standard_Real                          theLowerLimitActualTranslationY,
  const Standard_Boolean                       hasUpperLimitActualTranslationY,
  const Standard_Real                          theUpperLimitActualTranslationY,
  const Standard_Boolean                       hasLowerLimitActualTranslationZ,
  const Standard_Real                          theLowerLimitActualTranslationZ,
  const Standard_Boolean                       hasUpperLimitActualTranslationZ,
  const Standard_Real                          theUpperLimitActualTranslationZ)
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

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualRotationX() const
{
  return myLowerLimitActualRotationX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualRotationX(
  const Standard_Real theLowerLimitActualRotationX)
{
  myLowerLimitActualRotationX = theLowerLimitActualRotationX;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualRotationX() const
{
  return defLowerLimitActualRotationX;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualRotationX() const
{
  return myUpperLimitActualRotationX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualRotationX(
  const Standard_Real theUpperLimitActualRotationX)
{
  myUpperLimitActualRotationX = theUpperLimitActualRotationX;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualRotationX() const
{
  return defUpperLimitActualRotationX;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualRotationY() const
{
  return myLowerLimitActualRotationY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualRotationY(
  const Standard_Real theLowerLimitActualRotationY)
{
  myLowerLimitActualRotationY = theLowerLimitActualRotationY;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualRotationY() const
{
  return defLowerLimitActualRotationY;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualRotationY() const
{
  return myUpperLimitActualRotationY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualRotationY(
  const Standard_Real theUpperLimitActualRotationY)
{
  myUpperLimitActualRotationY = theUpperLimitActualRotationY;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualRotationY() const
{
  return defUpperLimitActualRotationY;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualRotationZ() const
{
  return myLowerLimitActualRotationZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualRotationZ(
  const Standard_Real theLowerLimitActualRotationZ)
{
  myLowerLimitActualRotationZ = theLowerLimitActualRotationZ;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualRotationZ() const
{
  return defLowerLimitActualRotationZ;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualRotationZ() const
{
  return myUpperLimitActualRotationZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualRotationZ(
  const Standard_Real theUpperLimitActualRotationZ)
{
  myUpperLimitActualRotationZ = theUpperLimitActualRotationZ;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualRotationZ() const
{
  return defUpperLimitActualRotationZ;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualTranslationX() const
{
  return myLowerLimitActualTranslationX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualTranslationX(
  const Standard_Real theLowerLimitActualTranslationX)
{
  myLowerLimitActualTranslationX = theLowerLimitActualTranslationX;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualTranslationX()
  const
{
  return defLowerLimitActualTranslationX;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualTranslationX() const
{
  return myUpperLimitActualTranslationX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualTranslationX(
  const Standard_Real theUpperLimitActualTranslationX)
{
  myUpperLimitActualTranslationX = theUpperLimitActualTranslationX;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualTranslationX()
  const
{
  return defUpperLimitActualTranslationX;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualTranslationY() const
{
  return myLowerLimitActualTranslationY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualTranslationY(
  const Standard_Real theLowerLimitActualTranslationY)
{
  myLowerLimitActualTranslationY = theLowerLimitActualTranslationY;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualTranslationY()
  const
{
  return defLowerLimitActualTranslationY;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualTranslationY() const
{
  return myUpperLimitActualTranslationY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualTranslationY(
  const Standard_Real theUpperLimitActualTranslationY)
{
  myUpperLimitActualTranslationY = theUpperLimitActualTranslationY;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualTranslationY()
  const
{
  return defUpperLimitActualTranslationY;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualTranslationZ() const
{
  return myLowerLimitActualTranslationZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualTranslationZ(
  const Standard_Real theLowerLimitActualTranslationZ)
{
  myLowerLimitActualTranslationZ = theLowerLimitActualTranslationZ;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualTranslationZ()
  const
{
  return defLowerLimitActualTranslationZ;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualTranslationZ() const
{
  return myUpperLimitActualTranslationZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualTranslationZ(
  const Standard_Real theUpperLimitActualTranslationZ)
{
  myUpperLimitActualTranslationZ = theUpperLimitActualTranslationZ;
}

//=================================================================================================

Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualTranslationZ()
  const
{
  return defUpperLimitActualTranslationZ;
}
