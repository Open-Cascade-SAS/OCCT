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

#include <StepKinematics_PrismaticPairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_PrismaticPairWithRange, StepKinematics_PrismaticPair)

//=================================================================================================

StepKinematics_PrismaticPairWithRange::StepKinematics_PrismaticPairWithRange()
{
  defLowerLimitActualTranslation = Standard_False;
  defUpperLimitActualTranslation = Standard_False;
}

//=================================================================================================

void StepKinematics_PrismaticPairWithRange::Init(
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
  const Standard_Boolean                       hasLowerLimitActualTranslation,
  const Standard_Real                          theLowerLimitActualTranslation,
  const Standard_Boolean                       hasUpperLimitActualTranslation,
  const Standard_Real                          theUpperLimitActualTranslation)
{
  StepKinematics_PrismaticPair::Init(theRepresentationItem_Name,
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

  defLowerLimitActualTranslation = hasLowerLimitActualTranslation;
  if (defLowerLimitActualTranslation)
  {
    myLowerLimitActualTranslation = theLowerLimitActualTranslation;
  }
  else
    myLowerLimitActualTranslation = 0;

  defUpperLimitActualTranslation = hasUpperLimitActualTranslation;
  if (defUpperLimitActualTranslation)
  {
    myUpperLimitActualTranslation = theUpperLimitActualTranslation;
  }
  else
    myUpperLimitActualTranslation = 0;
}

//=================================================================================================

Standard_Real StepKinematics_PrismaticPairWithRange::LowerLimitActualTranslation() const
{
  return myLowerLimitActualTranslation;
}

//=================================================================================================

void StepKinematics_PrismaticPairWithRange::SetLowerLimitActualTranslation(
  const Standard_Real theLowerLimitActualTranslation)
{
  myLowerLimitActualTranslation = theLowerLimitActualTranslation;
}

//=================================================================================================

Standard_Boolean StepKinematics_PrismaticPairWithRange::HasLowerLimitActualTranslation() const
{
  return defLowerLimitActualTranslation;
}

//=================================================================================================

Standard_Real StepKinematics_PrismaticPairWithRange::UpperLimitActualTranslation() const
{
  return myUpperLimitActualTranslation;
}

//=================================================================================================

void StepKinematics_PrismaticPairWithRange::SetUpperLimitActualTranslation(
  const Standard_Real theUpperLimitActualTranslation)
{
  myUpperLimitActualTranslation = theUpperLimitActualTranslation;
}

//=================================================================================================

Standard_Boolean StepKinematics_PrismaticPairWithRange::HasUpperLimitActualTranslation() const
{
  return defUpperLimitActualTranslation;
}
