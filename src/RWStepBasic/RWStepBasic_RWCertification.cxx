// File:	RWStepBasic_RWCertification.cxx
// Created:	Fri Nov 26 16:26:34 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepBasic_RWCertification.ixx>

//=======================================================================
//function : RWStepBasic_RWCertification
//purpose  : 
//=======================================================================

RWStepBasic_RWCertification::RWStepBasic_RWCertification ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWCertification::ReadStep (const Handle(StepData_StepReaderData)& data,
                                            const Standard_Integer num,
                                            Handle(Interface_Check)& ach,
                                            const Handle(StepBasic_Certification) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"certification") ) return;

  // Own fields of Certification

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aPurpose;
  data->ReadString (num, 2, "purpose", ach, aPurpose);

  Handle(StepBasic_CertificationType) aKind;
  data->ReadEntity (num, 3, "kind", ach, STANDARD_TYPE(StepBasic_CertificationType), aKind);

  // Initialize entity
  ent->Init(aName,
            aPurpose,
            aKind);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWCertification::WriteStep (StepData_StepWriter& SW,
                                             const Handle(StepBasic_Certification) &ent) const
{

  // Own fields of Certification

  SW.Send (ent->Name());

  SW.Send (ent->Purpose());

  SW.Send (ent->Kind());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWCertification::Share (const Handle(StepBasic_Certification) &ent,
                                         Interface_EntityIterator& iter) const
{

  // Own fields of Certification

  iter.AddItem (ent->Kind());
}
