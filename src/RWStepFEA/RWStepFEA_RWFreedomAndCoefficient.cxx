// Created on: 2002-12-14
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepFEA_RWFreedomAndCoefficient.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_FreedomAndCoefficient.hxx>

//=======================================================================
//function : RWStepFEA_RWFreedomAndCoefficient
//purpose  : 
//=======================================================================
RWStepFEA_RWFreedomAndCoefficient::RWStepFEA_RWFreedomAndCoefficient ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFreedomAndCoefficient::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                  const Standard_Integer num,
                                                  Handle(Interface_Check)& ach,
                                                  const Handle(StepFEA_FreedomAndCoefficient) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"freedom_and_coefficient") ) return;

  // Own fields of FreedomAndCoefficient

  StepFEA_DegreeOfFreedom aFreedom;
  data->ReadEntity (num, 1, "freedom", ach, aFreedom);

  StepElement_MeasureOrUnspecifiedValue aA;
  data->ReadEntity (num, 2, "a", ach, aA);

  // Initialize entity
  ent->Init(aFreedom,
            aA);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFreedomAndCoefficient::WriteStep (StepData_StepWriter& SW,
                                                   const Handle(StepFEA_FreedomAndCoefficient) &ent) const
{

  // Own fields of FreedomAndCoefficient

  SW.Send (ent->Freedom().Value());

  SW.Send (ent->A().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFreedomAndCoefficient::Share (const Handle(StepFEA_FreedomAndCoefficient) &ent,
                                               Interface_EntityIterator& iter) const
{

  // Own fields of FreedomAndCoefficient

  iter.AddItem (ent->Freedom().Value());

  iter.AddItem (ent->A().Value());
}
