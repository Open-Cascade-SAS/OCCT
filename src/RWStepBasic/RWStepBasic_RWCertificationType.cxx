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
#include <RWStepBasic_RWCertificationType.hxx>
#include <StepBasic_CertificationType.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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

void RWStepBasic_RWCertificationType::Share (const Handle(StepBasic_CertificationType) &,
                                             Interface_EntityIterator&) const
{
  // Own fields of CertificationType
}
