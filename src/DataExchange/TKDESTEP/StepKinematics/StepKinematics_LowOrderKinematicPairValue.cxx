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

#include <StepKinematics_LowOrderKinematicPairValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_LowOrderKinematicPairValue, StepKinematics_PairValue)

//=================================================================================================

StepKinematics_LowOrderKinematicPairValue::StepKinematics_LowOrderKinematicPairValue() {}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairValue::Init(
  const Handle(TCollection_HAsciiString)&     theRepresentationItem_Name,
  const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
  const Standard_Real                         theActualTranslationX,
  const Standard_Real                         theActualTranslationY,
  const Standard_Real                         theActualTranslationZ,
  const Standard_Real                         theActualRotationX,
  const Standard_Real                         theActualRotationY,
  const Standard_Real                         theActualRotationZ)
{
  StepKinematics_PairValue::Init(theRepresentationItem_Name, thePairValue_AppliesToPair);

  myActualTranslationX = theActualTranslationX;

  myActualTranslationY = theActualTranslationY;

  myActualTranslationZ = theActualTranslationZ;

  myActualRotationX = theActualRotationX;

  myActualRotationY = theActualRotationY;

  myActualRotationZ = theActualRotationZ;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairValue::ActualTranslationX() const
{
  return myActualTranslationX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairValue::SetActualTranslationX(
  const Standard_Real theActualTranslationX)
{
  myActualTranslationX = theActualTranslationX;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairValue::ActualTranslationY() const
{
  return myActualTranslationY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairValue::SetActualTranslationY(
  const Standard_Real theActualTranslationY)
{
  myActualTranslationY = theActualTranslationY;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairValue::ActualTranslationZ() const
{
  return myActualTranslationZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairValue::SetActualTranslationZ(
  const Standard_Real theActualTranslationZ)
{
  myActualTranslationZ = theActualTranslationZ;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairValue::ActualRotationX() const
{
  return myActualRotationX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairValue::SetActualRotationX(
  const Standard_Real theActualRotationX)
{
  myActualRotationX = theActualRotationX;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairValue::ActualRotationY() const
{
  return myActualRotationY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairValue::SetActualRotationY(
  const Standard_Real theActualRotationY)
{
  myActualRotationY = theActualRotationY;
}

//=================================================================================================

Standard_Real StepKinematics_LowOrderKinematicPairValue::ActualRotationZ() const
{
  return myActualRotationZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPairValue::SetActualRotationZ(
  const Standard_Real theActualRotationZ)
{
  myActualRotationZ = theActualRotationZ;
}
