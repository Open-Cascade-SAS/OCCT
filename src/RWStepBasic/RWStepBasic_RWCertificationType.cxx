// File:	RWStepBasic_RWCertificationType.cxx
// Created:	Fri Nov 26 16:26:35 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWCertificationType.ixx>

//=======================================================================
//function : RWStepBasic_RWCertificationType
//purpose  : 
//=======================================================================

RWStepBasic_RWCertificationType::RWStepBasic_RWCertificationType ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWCertificationType::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                const Standard_Integer num,
                                                Handle(Interface_Check)& ach,
                                                const Handle(StepBasic_CertificationType) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"certification_type") ) return;

  // Own fields of CertificationType

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 1, "description", ach, aDescription);

  // Initialize entity
  ent->Init(aDescription);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWCertificationType::WriteStep (StepData_StepWriter& SW,
                                                 const Handle(StepBasic_CertificationType) &ent) const
{

  // Own fields of CertificationType

  SW.Send (ent->Description());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWCertificationType::Share (const Handle(StepBasic_CertificationType) &ent,
                                             Interface_EntityIterator& iter) const
{

  // Own fields of CertificationType
}
