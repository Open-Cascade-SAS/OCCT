// File:	StepBasic_EffectivityAssignment.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_EffectivityAssignment.ixx>

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
