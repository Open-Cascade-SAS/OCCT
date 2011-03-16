// File:	RWStepBasic_RWGroupAssignment.cxx
// Created:	Wed May 10 15:09:07 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWGroupAssignment.ixx>

//=======================================================================
//function : RWStepBasic_RWGroupAssignment
//purpose  : 
//=======================================================================

RWStepBasic_RWGroupAssignment::RWStepBasic_RWGroupAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGroupAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                              const Standard_Integer num,
                                              Handle(Interface_Check)& ach,
                                              const Handle(StepBasic_GroupAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"group_assignment") ) return;

  // Own fields of GroupAssignment

  Handle(StepBasic_Group) aAssignedGroup;
  data->ReadEntity (num, 1, "assigned_group", ach, STANDARD_TYPE(StepBasic_Group), aAssignedGroup);

  // Initialize entity
  ent->Init(aAssignedGroup);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGroupAssignment::WriteStep (StepData_StepWriter& SW,
                                               const Handle(StepBasic_GroupAssignment) &ent) const
{

  // Own fields of GroupAssignment

  SW.Send (ent->AssignedGroup());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWGroupAssignment::Share (const Handle(StepBasic_GroupAssignment) &ent,
                                           Interface_EntityIterator& iter) const
{

  // Own fields of GroupAssignment

  iter.AddItem (ent->AssignedGroup());
}
