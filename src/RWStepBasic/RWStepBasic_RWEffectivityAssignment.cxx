// File:	RWStepBasic_RWEffectivityAssignment.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWEffectivityAssignment.ixx>

//=======================================================================
//function : RWStepBasic_RWEffectivityAssignment
//purpose  : 
//=======================================================================

RWStepBasic_RWEffectivityAssignment::RWStepBasic_RWEffectivityAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWEffectivityAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepBasic_EffectivityAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"effectivity_assignment") ) return;

  // Own fields of EffectivityAssignment

  Handle(StepBasic_Effectivity) aAssignedEffectivity;
  data->ReadEntity (num, 1, "assigned_effectivity", ach, STANDARD_TYPE(StepBasic_Effectivity), aAssignedEffectivity);

  // Initialize entity
  ent->Init(aAssignedEffectivity);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWEffectivityAssignment::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepBasic_EffectivityAssignment) &ent) const
{

  // Own fields of EffectivityAssignment

  SW.Send (ent->AssignedEffectivity());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWEffectivityAssignment::Share (const Handle(StepBasic_EffectivityAssignment) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Own fields of EffectivityAssignment

  iter.AddItem (ent->AssignedEffectivity());
}
