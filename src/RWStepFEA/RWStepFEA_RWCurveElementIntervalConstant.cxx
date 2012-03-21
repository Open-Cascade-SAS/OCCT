// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <RWStepFEA_RWCurveElementIntervalConstant.ixx>

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
