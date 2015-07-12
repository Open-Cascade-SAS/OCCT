// Created on: 2002-12-12
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
#include <RWStepFEA_RWCurveElementInterval.hxx>
#include <StepBasic_EulerAngles.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_CurveElementInterval.hxx>
#include <StepFEA_CurveElementLocation.hxx>

//=======================================================================
//function : RWStepFEA_RWCurveElementInterval
//purpose  : 
//=======================================================================
RWStepFEA_RWCurveElementInterval::RWStepFEA_RWCurveElementInterval ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementInterval::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                 const Standard_Integer num,
                                                 Handle(Interface_Check)& ach,
                                                 const Handle(StepFEA_CurveElementInterval) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"curve_element_interval") ) return;

  // Own fields of CurveElementInterval

  Handle(StepFEA_CurveElementLocation) aFinishPosition;
  data->ReadEntity (num, 1, "finish_position", ach, STANDARD_TYPE(StepFEA_CurveElementLocation), aFinishPosition);

  Handle(StepBasic_EulerAngles) aEuAngles;
  data->ReadEntity (num, 2, "eu_angles", ach, STANDARD_TYPE(StepBasic_EulerAngles), aEuAngles);

  // Initialize entity
  ent->Init(aFinishPosition,
            aEuAngles);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementInterval::WriteStep (StepData_StepWriter& SW,
                                                  const Handle(StepFEA_CurveElementInterval) &ent) const
{

  // Own fields of CurveElementInterval

  SW.Send (ent->FinishPosition());

  SW.Send (ent->EuAngles());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementInterval::Share (const Handle(StepFEA_CurveElementInterval) &ent,
                                              Interface_EntityIterator& iter) const
{

  // Own fields of CurveElementInterval

  iter.AddItem (ent->FinishPosition());

  iter.AddItem (ent->EuAngles());
}
