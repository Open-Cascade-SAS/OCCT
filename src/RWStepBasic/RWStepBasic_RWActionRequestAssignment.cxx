// File:	RWStepBasic_RWActionRequestAssignment.cxx
// Created:	Fri Nov 26 16:26:30 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWActionRequestAssignment.ixx>

//=======================================================================
//function : RWStepBasic_RWActionRequestAssignment
//purpose  : 
//=======================================================================

RWStepBasic_RWActionRequestAssignment::RWStepBasic_RWActionRequestAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                      const Standard_Integer num,
                                                      Handle(Interface_Check)& ach,
                                                      const Handle(StepBasic_ActionRequestAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"action_request_assignment") ) return;

  // Own fields of ActionRequestAssignment

  Handle(StepBasic_VersionedActionRequest) aAssignedActionRequest;
  data->ReadEntity (num, 1, "assigned_action_request", ach, STANDARD_TYPE(StepBasic_VersionedActionRequest), aAssignedActionRequest);

  // Initialize entity
  ent->Init(aAssignedActionRequest);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestAssignment::WriteStep (StepData_StepWriter& SW,
                                                       const Handle(StepBasic_ActionRequestAssignment) &ent) const
{

  // Own fields of ActionRequestAssignment

  SW.Send (ent->AssignedActionRequest());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestAssignment::Share (const Handle(StepBasic_ActionRequestAssignment) &ent,
                                                   Interface_EntityIterator& iter) const
{

  // Own fields of ActionRequestAssignment

  iter.AddItem (ent->AssignedActionRequest());
}
