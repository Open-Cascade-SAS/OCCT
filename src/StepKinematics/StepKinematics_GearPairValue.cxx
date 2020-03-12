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

#include <StepKinematics_GearPairValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_GearPairValue, StepKinematics_PairValue)

//=======================================================================
//function : StepKinematics_GearPairValue
//purpose  :
//=======================================================================
StepKinematics_GearPairValue::StepKinematics_GearPairValue ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_GearPairValue::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                         const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
                                         const Standard_Real theActualRotation1)
{
  StepKinematics_PairValue::Init(theRepresentationItem_Name,
                                 thePairValue_AppliesToPair);

  myActualRotation1 = theActualRotation1;
}

//=======================================================================
//function : ActualRotation1
//purpose  :
//=======================================================================
Standard_Real StepKinematics_GearPairValue::ActualRotation1 () const
{
  return myActualRotation1;
}

//=======================================================================
//function : SetActualRotation1
//purpose  :
//=======================================================================
void StepKinematics_GearPairValue::SetActualRotation1 (const Standard_Real theActualRotation1)
{
  myActualRotation1 = theActualRotation1;
}
