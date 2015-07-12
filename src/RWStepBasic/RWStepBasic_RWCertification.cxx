// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWCertification.hxx>
#include <StepBasic_Certification.hxx>
#include <StepBasic_CertificationType.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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
