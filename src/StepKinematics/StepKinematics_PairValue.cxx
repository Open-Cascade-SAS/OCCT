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

#include <StepKinematics_PairValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_PairValue, StepGeom_GeometricRepresentationItem)

//=================================================================================================

StepKinematics_PairValue::StepKinematics_PairValue() {}

//=================================================================================================

void StepKinematics_PairValue::Init(
  const Handle(TCollection_HAsciiString)&     theRepresentationItem_Name,
  const Handle(StepKinematics_KinematicPair)& theAppliesToPair)
{
  StepGeom_GeometricRepresentationItem::Init(theRepresentationItem_Name);

  myAppliesToPair = theAppliesToPair;
}

//=================================================================================================

Handle(StepKinematics_KinematicPair) StepKinematics_PairValue::AppliesToPair() const
{
  return myAppliesToPair;
}

//=================================================================================================

void StepKinematics_PairValue::SetAppliesToPair(
  const Handle(StepKinematics_KinematicPair)& theAppliesToPair)
{
  myAppliesToPair = theAppliesToPair;
}
