// File:	RWStepBasic_RWExternalIdentificationAssignment.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <RWStepBasic_RWExternalIdentificationAssignment.ixx>

//=======================================================================
//function : RWStepBasic_RWExternalIdentificationAssignment
//purpose  : 
//=======================================================================

RWStepBasic_RWExternalIdentificationAssignment::RWStepBasic_RWExternalIdentificationAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternalIdentificationAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                               const Standard_Integer num,
                                                               Handle(Interface_Check)& ach,
                                                               const Handle(StepBasic_ExternalIdentificationAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"external_identification_assignment") ) return;

  // Inherited fields of IdentificationAssignment

  Handle(TCollection_HAsciiString) aIdentificationAssignment_AssignedId;
  data->ReadString (num, 1, "identification_assignment.assigned_id", ach, aIdentificationAssignment_AssignedId);

  Handle(StepBasic_IdentificationRole) aIdentificationAssignment_Role;
  data->ReadEntity (num, 2, "identification_assignment.role", ach, STANDARD_TYPE(StepBasic_IdentificationRole), aIdentificationAssignment_Role);

  // Own fields of ExternalIdentificationAssignment

  Handle(StepBasic_ExternalSource) aSource;
  data->ReadEntity (num, 3, "source", ach, STANDARD_TYPE(StepBasic_ExternalSource), aSource);

  // Initialize entity
  ent->Init(aIdentificationAssignment_AssignedId,
            aIdentificationAssignment_Role,
            aSource);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternalIdentificationAssignment::WriteStep (StepData_StepWriter& SW,
                                                                const Handle(StepBasic_ExternalIdentificationAssignment) &ent) const
{

  // Inherited fields of IdentificationAssignment

  SW.Send (ent->StepBasic_IdentificationAssignment::AssignedId());

  SW.Send (ent->StepBasic_IdentificationAssignment::Role());

  // Own fields of ExternalIdentificationAssignment

  SW.Send (ent->Source());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternalIdentificationAssignment::Share (const Handle(StepBasic_ExternalIdentificationAssignment) &ent,
                                                            Interface_EntityIterator& iter) const
{

  // Inherited fields of IdentificationAssignment

  iter.AddItem (ent->StepBasic_IdentificationAssignment::Role());

  // Own fields of ExternalIdentificationAssignment

  iter.AddItem (ent->Source());
}
