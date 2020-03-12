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

#include <StepKinematics_RollingSurfacePairValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_RollingSurfacePairValue, StepKinematics_PairValue)

//=======================================================================
//function : StepKinematics_RollingSurfacePairValue
//purpose  :
//=======================================================================
StepKinematics_RollingSurfacePairValue::StepKinematics_RollingSurfacePairValue ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_RollingSurfacePairValue::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                                   const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
                                                   const Handle(StepGeom_PointOnSurface)& theActualPointOnSurface,
                                                   const Standard_Real theActualRotation)
{
  StepKinematics_PairValue::Init(theRepresentationItem_Name,
                                 thePairValue_AppliesToPair);

  myActualPointOnSurface = theActualPointOnSurface;

  myActualRotation = theActualRotation;
}

//=======================================================================
//function : ActualPointOnSurface
//purpose  :
//=======================================================================
Handle(StepGeom_PointOnSurface) StepKinematics_RollingSurfacePairValue::ActualPointOnSurface () const
{
  return myActualPointOnSurface;
}

//=======================================================================
//function : SetActualPointOnSurface
//purpose  :
//=======================================================================
void StepKinematics_RollingSurfacePairValue::SetActualPointOnSurface (const Handle(StepGeom_PointOnSurface)& theActualPointOnSurface)
{
  myActualPointOnSurface = theActualPointOnSurface;
}

//=======================================================================
//function : ActualRotation
//purpose  :
//=======================================================================
Standard_Real StepKinematics_RollingSurfacePairValue::ActualRotation () const
{
  return myActualRotation;
}

//=======================================================================
//function : SetActualRotation
//purpose  :
//=======================================================================
void StepKinematics_RollingSurfacePairValue::SetActualRotation (const Standard_Real theActualRotation)
{
  myActualRotation = theActualRotation;
}
