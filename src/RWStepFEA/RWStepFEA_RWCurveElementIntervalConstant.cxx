// File:	RWStepFEA_RWCurveElementIntervalConstant.cxx
// Created:	Thu Dec 12 17:51:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
