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
#include <RWStepFEA_RWCurveElementIntervalConstant.hxx>
#include <StepBasic_EulerAngles.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_CurveElementSectionDefinition.hxx>
#include <StepFEA_CurveElementIntervalConstant.hxx>
#include <StepFEA_CurveElementLocation.hxx>

//=======================================================================
//function : RWStepFEA_RWCurveElementIntervalConstant
//purpose  : 
//=======================================================================
RWStepFEA_RWCurveElementIntervalConstant::RWStepFEA_RWCurveElementIntervalConstant ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementIntervalConstant::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                         const Standard_Integer num,
                                                         Handle(Interface_Check)& ach,
                                                         const Handle(StepFEA_CurveElementIntervalConstant) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"curve_element_interval_constant") ) return;

  // Inherited fields of CurveElementInterval

  Handle(StepFEA_CurveElementLocation) aCurveElementInterval_FinishPosition;
  data->ReadEntity (num, 1, "curve_element_interval.finish_position", ach, STANDARD_TYPE(StepFEA_CurveElementLocation), aCurveElementInterval_FinishPosition);

  Handle(StepBasic_EulerAngles) aCurveElementInterval_EuAngles;
  data->ReadEntity (num, 2, "curve_element_interval.eu_angles", ach, STANDARD_TYPE(StepBasic_EulerAngles), aCurveElementInterval_EuAngles);

  // Own fields of CurveElementIntervalConstant

  Handle(StepElement_CurveElementSectionDefinition) aSection;
  data->ReadEntity (num, 3, "section", ach, STANDARD_TYPE(StepElement_CurveElementSectionDefinition), aSection);

  // Initialize entity
  ent->Init(aCurveElementInterval_FinishPosition,
            aCurveElementInterval_EuAngles,
            aSection);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementIntervalConstant::WriteStep (StepData_StepWriter& SW,
                                                          const Handle(StepFEA_CurveElementIntervalConstant) &ent) const
{

  // Inherited fields of CurveElementInterval

  SW.Send (ent->StepFEA_CurveElementInterval::FinishPosition());

  SW.Send (ent->StepFEA_CurveElementInterval::EuAngles());

  // Own fields of CurveElementIntervalConstant

  SW.Send (ent->Section());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementIntervalConstant::Share (const Handle(StepFEA_CurveElementIntervalConstant) &ent,
                                                      Interface_EntityIterator& iter) const
{

  // Inherited fields of CurveElementInterval

  iter.AddItem (ent->StepFEA_CurveElementInterval::FinishPosition());

  iter.AddItem (ent->StepFEA_CurveElementInterval::EuAngles());

  // Own fields of CurveElementIntervalConstant

  iter.AddItem (ent->Section());
}
