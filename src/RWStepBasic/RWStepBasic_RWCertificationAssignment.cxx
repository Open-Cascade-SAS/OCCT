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
#include <RWStepBasic_RWCertificationAssignment.hxx>
#include <StepBasic_Certification.hxx>
#include <StepBasic_CertificationAssignment.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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
