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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWEffectivityAssignment.hxx>
#include <StepBasic_Effectivity.hxx>
#include <StepBasic_EffectivityAssignment.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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
