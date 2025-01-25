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

#include <StepKinematics_PlanarPairValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_PlanarPairValue, StepKinematics_PairValue)

//=================================================================================================

StepKinematics_PlanarPairValue::StepKinematics_PlanarPairValue() {}

//=================================================================================================

void StepKinematics_PlanarPairValue::Init(
  const Handle(TCollection_HAsciiString)&     theRepresentationItem_Name,
  const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
  const Standard_Real                         theActualRotation,
  const Standard_Real                         theActualTranslationX,
  const Standard_Real                         theActualTranslationY)
{
  StepKinematics_PairValue::Init(theRepresentationItem_Name, thePairValue_AppliesToPair);

  myActualRotation = theActualRotation;

  myActualTranslationX = theActualTranslationX;

  myActualTranslationY = theActualTranslationY;
}

//=================================================================================================

Standard_Real StepKinematics_PlanarPairValue::ActualRotation() const
{
  return myActualRotation;
}

//=================================================================================================

void StepKinematics_PlanarPairValue::SetActualRotation(const Standard_Real theActualRotation)
{
  myActualRotation = theActualRotation;
}

//=================================================================================================

Standard_Real StepKinematics_PlanarPairValue::ActualTranslationX() const
{
  return myActualTranslationX;
}

//=================================================================================================

void StepKinematics_PlanarPairValue::SetActualTranslationX(
  const Standard_Real theActualTranslationX)
{
  myActualTranslationX = theActualTranslationX;
}

//=================================================================================================

Standard_Real StepKinematics_PlanarPairValue::ActualTranslationY() const
{
  return myActualTranslationY;
}

//=================================================================================================

void StepKinematics_PlanarPairValue::SetActualTranslationY(
  const Standard_Real theActualTranslationY)
{
  myActualTranslationY = theActualTranslationY;
}
