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

#include <StepKinematics_CylindricalPairValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_CylindricalPairValue, StepKinematics_PairValue)

//=================================================================================================

StepKinematics_CylindricalPairValue::StepKinematics_CylindricalPairValue() {}

//=================================================================================================

void StepKinematics_CylindricalPairValue::Init(
  const Handle(TCollection_HAsciiString)&     theRepresentationItem_Name,
  const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
  const Standard_Real                         theActualTranslation,
  const Standard_Real                         theActualRotation)
{
  StepKinematics_PairValue::Init(theRepresentationItem_Name, thePairValue_AppliesToPair);

  myActualTranslation = theActualTranslation;

  myActualRotation = theActualRotation;
}

//=================================================================================================

Standard_Real StepKinematics_CylindricalPairValue::ActualTranslation() const
{
  return myActualTranslation;
}

//=================================================================================================

void StepKinematics_CylindricalPairValue::SetActualTranslation(
  const Standard_Real theActualTranslation)
{
  myActualTranslation = theActualTranslation;
}

//=================================================================================================

Standard_Real StepKinematics_CylindricalPairValue::ActualRotation() const
{
  return myActualRotation;
}

//=================================================================================================

void StepKinematics_CylindricalPairValue::SetActualRotation(const Standard_Real theActualRotation)
{
  myActualRotation = theActualRotation;
}
