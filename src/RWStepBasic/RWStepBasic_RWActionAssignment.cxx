// File:	RWStepBasic_RWActionAssignment.cxx
// Created:	Fri Nov 26 16:26:29 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWActionAssignment.ixx>

//=======================================================================
//function : RWStepBasic_RWActionAssignment
//purpose  : 
//=======================================================================

RWStepBasic_RWActionAssignment::RWStepBasic_RWActionAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                               const Standard_Integer num,
                                               Handle(Interface_Check)& ach,
                                               const Handle(StepBasic_ActionAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"action_assignment") ) return;

  // Own fields of ActionAssignment

  Handle(StepBasic_Action) aAssignedAction;
  data->ReadEntity (num, 1, "assigned_action", ach, STANDARD_TYPE(StepBasic_Action), aAssignedAction);

  // Initialize entity
  ent->Init(aAssignedAction);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionAssignment::WriteStep (StepData_StepWriter& SW,
                                                const Handle(StepBasic_ActionAssignment) &ent) const
{

  // Own fields of ActionAssignment

  SW.Send (ent->AssignedAction());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWActionAssignment::Share (const Handle(StepBasic_ActionAssignment) &ent,
                                            Interface_EntityIterator& iter) const
{

  // Own fields of ActionAssignment

  iter.AddItem (ent->AssignedAction());
}
