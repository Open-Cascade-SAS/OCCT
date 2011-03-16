// File:	RWStepFEA_RWCurveElementInterval.cxx
// Created:	Thu Dec 12 17:51:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWCurveElementInterval.ixx>

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
