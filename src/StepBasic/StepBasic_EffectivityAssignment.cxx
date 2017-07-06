// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Type.hxx>
#include <StepBasic_Effectivity.hxx>
#include <StepBasic_EffectivityAssignment.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_EffectivityAssignment,Standard_Transient)

//=======================================================================
//function : StepBasic_EffectivityAssignment
//purpose  : 
//=======================================================================
StepBasic_EffectivityAssignment::StepBasic_EffectivityAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_EffectivityAssignment::Init (const Handle(StepBasic_Effectivity) &aAssignedEffectivity)
{

  theAssignedEffectivity = aAssignedEffectivity;
}

//=======================================================================
//function : AssignedEffectivity
//purpose  : 
//=======================================================================

Handle(StepBasic_Effectivity) StepBasic_EffectivityAssignment::AssignedEffectivity () const
{
  return theAssignedEffectivity;
}

//=======================================================================
//function : SetAssignedEffectivity
//purpose  : 
//=======================================================================

void StepBasic_EffectivityAssignment::SetAssignedEffectivity (const Handle(StepBasic_Effectivity) &aAssignedEffectivity)
{
  theAssignedEffectivity = aAssignedEffectivity;
}
