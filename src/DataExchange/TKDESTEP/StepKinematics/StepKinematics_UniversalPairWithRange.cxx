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
  defLowerLimitFirstRotation  = Standard_False;
  defUpperLimitFirstRotation  = Standard_False;
  defLowerLimitSecondRotation = Standard_False;
  defUpperLimitSecondRotation = Standard_False;
}

//=================================================================================================

void StepKinematics_UniversalPairWithRange::Init(
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
  const Standard_Boolean                       hasUniversalPair_InputSkewAngle,
  const Standard_Real                          theUniversalPair_InputSkewAngle,
  const Standard_Boolean                       hasLowerLimitFirstRotation,
  const Standard_Real                          theLowerLimitFirstRotation,
  const Standard_Boolean                       hasUpperLimitFirstRotation,
  const Standard_Real                          theUpperLimitFirstRotation,
  const Standard_Boolean                       hasLowerLimitSecondRotation,
  const Standard_Real                          theLowerLimitSecondRotation,
  const Standard_Boolean                       hasUpperLimitSecondRotation,
  const Standard_Real                          theUpperLimitSecondRotation)
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

Standard_Real StepKinematics_UniversalPairWithRange::LowerLimitFirstRotation() const
{
  return myLowerLimitFirstRotation;
}

//=================================================================================================

void StepKinematics_UniversalPairWithRange::SetLowerLimitFirstRotation(
  const Standard_Real theLowerLimitFirstRotation)
{
  myLowerLimitFirstRotation = theLowerLimitFirstRotation;
}

//=================================================================================================

Standard_Boolean StepKinematics_UniversalPairWithRange::HasLowerLimitFirstRotation() const
{
  return defLowerLimitFirstRotation;
}

//=================================================================================================

Standard_Real StepKinematics_UniversalPairWithRange::UpperLimitFirstRotation() const
{
  return myUpperLimitFirstRotation;
}

//=================================================================================================

void StepKinematics_UniversalPairWithRange::SetUpperLimitFirstRotation(
  const Standard_Real theUpperLimitFirstRotation)
{
  myUpperLimitFirstRotation = theUpperLimitFirstRotation;
}

//=================================================================================================

Standard_Boolean StepKinematics_UniversalPairWithRange::HasUpperLimitFirstRotation() const
{
  return defUpperLimitFirstRotation;
}

//=================================================================================================

Standard_Real StepKinematics_UniversalPairWithRange::LowerLimitSecondRotation() const
{
  return myLowerLimitSecondRotation;
}

//=================================================================================================

void StepKinematics_UniversalPairWithRange::SetLowerLimitSecondRotation(
  const Standard_Real theLowerLimitSecondRotation)
{
  myLowerLimitSecondRotation = theLowerLimitSecondRotation;
}

//=================================================================================================

Standard_Boolean StepKinematics_UniversalPairWithRange::HasLowerLimitSecondRotation() const
{
  return defLowerLimitSecondRotation;
}

//=================================================================================================

Standard_Real StepKinematics_UniversalPairWithRange::UpperLimitSecondRotation() const
{
  return myUpperLimitSecondRotation;
}

//=================================================================================================

void StepKinematics_UniversalPairWithRange::SetUpperLimitSecondRotation(
  const Standard_Real theUpperLimitSecondRotation)
{
  myUpperLimitSecondRotation = theUpperLimitSecondRotation;
}

//=================================================================================================

Standard_Boolean StepKinematics_UniversalPairWithRange::HasUpperLimitSecondRotation() const
{
  return defUpperLimitSecondRotation;
}
