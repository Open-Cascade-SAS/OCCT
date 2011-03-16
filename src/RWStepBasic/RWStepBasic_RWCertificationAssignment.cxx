// File:	RWStepBasic_RWCertificationAssignment.cxx
// Created:	Fri Nov 26 16:26:34 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWCertificationAssignment.ixx>

//=======================================================================
//function : RWStepBasic_RWCertificationAssignment
//purpose  : 
//=======================================================================

RWStepBasic_RWCertificationAssignment::RWStepBasic_RWCertificationAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWCertificationAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                      const Standard_Integer num,
                                                      Handle(Interface_Check)& ach,
                                                      const Handle(StepBasic_CertificationAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"certification_assignment") ) return;

  // Own fields of CertificationAssignment

  Handle(StepBasic_Certification) aAssignedCertification;
  data->ReadEntity (num, 1, "assigned_certification", ach, STANDARD_TYPE(StepBasic_Certification), aAssignedCertification);

  // Initialize entity
  ent->Init(aAssignedCertification);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWCertificationAssignment::WriteStep (StepData_StepWriter& SW,
                                                       const Handle(StepBasic_CertificationAssignment) &ent) const
{

  // Own fields of CertificationAssignment

  SW.Send (ent->AssignedCertification());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWCertificationAssignment::Share (const Handle(StepBasic_CertificationAssignment) &ent,
                                                   Interface_EntityIterator& iter) const
{

  // Own fields of CertificationAssignment

  iter.AddItem (ent->AssignedCertification());
}
