// File:	RWStepBasic_RWIdentificationAssignment.cxx
// Created:	Wed May 10 15:09:08 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWIdentificationAssignment.ixx>

//=======================================================================
//function : RWStepBasic_RWIdentificationAssignment
//purpose  : 
//=======================================================================

RWStepBasic_RWIdentificationAssignment::RWStepBasic_RWIdentificationAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWIdentificationAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                       const Standard_Integer num,
                                                       Handle(Interface_Check)& ach,
                                                       const Handle(StepBasic_IdentificationAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"identification_assignment") ) return;

  // Own fields of IdentificationAssignment

  Handle(TCollection_HAsciiString) aAssignedId;
  data->ReadString (num, 1, "assigned_id", ach, aAssignedId);

  Handle(StepBasic_IdentificationRole) aRole;
  data->ReadEntity (num, 2, "role", ach, STANDARD_TYPE(StepBasic_IdentificationRole), aRole);

  // Initialize entity
  ent->Init(aAssignedId,
            aRole);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWIdentificationAssignment::WriteStep (StepData_StepWriter& SW,
                                                        const Handle(StepBasic_IdentificationAssignment) &ent) const
{

  // Own fields of IdentificationAssignment

  SW.Send (ent->AssignedId());

  SW.Send (ent->Role());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWIdentificationAssignment::Share (const Handle(StepBasic_IdentificationAssignment) &ent,
                                                    Interface_EntityIterator& iter) const
{

  // Own fields of IdentificationAssignment

  iter.AddItem (ent->Role());
}
