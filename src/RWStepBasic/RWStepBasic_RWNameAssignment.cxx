// File:	RWStepBasic_RWNameAssignment.cxx
// Created:	Wed May 10 15:09:08 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWNameAssignment.ixx>

//=======================================================================
//function : RWStepBasic_RWNameAssignment
//purpose  : 
//=======================================================================

RWStepBasic_RWNameAssignment::RWStepBasic_RWNameAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWNameAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                             const Standard_Integer num,
                                             Handle(Interface_Check)& ach,
                                             const Handle(StepBasic_NameAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"name_assignment") ) return;

  // Own fields of NameAssignment

  Handle(TCollection_HAsciiString) aAssignedName;
  data->ReadString (num, 1, "assigned_name", ach, aAssignedName);

  // Initialize entity
  ent->Init(aAssignedName);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWNameAssignment::WriteStep (StepData_StepWriter& SW,
                                              const Handle(StepBasic_NameAssignment) &ent) const
{

  // Own fields of NameAssignment

  SW.Send (ent->AssignedName());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWNameAssignment::Share (const Handle(StepBasic_NameAssignment) &ent,
                                          Interface_EntityIterator& iter) const
{

  // Own fields of NameAssignment
}
