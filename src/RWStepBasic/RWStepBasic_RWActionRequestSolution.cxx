// File:	RWStepBasic_RWActionRequestSolution.cxx
// Created:	Fri Nov 26 16:26:31 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWActionRequestSolution.ixx>

//=======================================================================
//function : RWStepBasic_RWActionRequestSolution
//purpose  : 
//=======================================================================

RWStepBasic_RWActionRequestSolution::RWStepBasic_RWActionRequestSolution ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestSolution::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepBasic_ActionRequestSolution) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"action_request_solution") ) return;

  // Own fields of ActionRequestSolution

  Handle(StepBasic_ActionMethod) aMethod;
  data->ReadEntity (num, 1, "method", ach, STANDARD_TYPE(StepBasic_ActionMethod), aMethod);

  Handle(StepBasic_VersionedActionRequest) aRequest;
  data->ReadEntity (num, 2, "request", ach, STANDARD_TYPE(StepBasic_VersionedActionRequest), aRequest);

  // Initialize entity
  ent->Init(aMethod,
            aRequest);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestSolution::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepBasic_ActionRequestSolution) &ent) const
{

  // Own fields of ActionRequestSolution

  SW.Send (ent->Method());

  SW.Send (ent->Request());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionRequestSolution::Share (const Handle(StepBasic_ActionRequestSolution) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Own fields of ActionRequestSolution

  iter.AddItem (ent->Method());

  iter.AddItem (ent->Request());
}
