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
#include <RWStepAP203_RWCcDesignCertification.hxx>
#include <StepAP203_CcDesignCertification.hxx>
#include <StepAP203_CertifiedItem.hxx>
#include <StepAP203_HArray1OfCertifiedItem.hxx>
#include <StepBasic_Certification.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepAP203_RWCcDesignCertification
//purpose  : 
//=======================================================================
RWStepAP203_RWCcDesignCertification::RWStepAP203_RWCcDesignCertification ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepAP203_RWCcDesignCertification::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepAP203_CcDesignCertification) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"cc_design_certification") ) return;

  // Inherited fields of CertificationAssignment

  Handle(StepBasic_Certification) aCertificationAssignment_AssignedCertification;
  data->ReadEntity (num, 1, "certification_assignment.assigned_certification", ach, STANDARD_TYPE(StepBasic_Certification), aCertificationAssignment_AssignedCertification);

  // Own fields of CcDesignCertification

  Handle(StepAP203_HArray1OfCertifiedItem) aItems;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "items", ach, sub2) ) {
    Standard_Integer num2 = sub2;
    Standard_Integer nb0 = data->NbParams(num2);
    aItems = new StepAP203_HArray1OfCertifiedItem (1, nb0);
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepAP203_CertifiedItem anIt0;
      data->ReadEntity (num2, i0, "items", ach, anIt0);
      aItems->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aCertificationAssignment_AssignedCertification,
            aItems);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepAP203_RWCcDesignCertification::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepAP203_CcDesignCertification) &ent) const
{

  // Inherited fields of CertificationAssignment

  SW.Send (ent->StepBasic_CertificationAssignment::AssignedCertification());

  // Own fields of CcDesignCertification

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->Items()->Length(); i1++ ) {
    StepAP203_CertifiedItem Var0 = ent->Items()->Value(i1);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepAP203_RWCcDesignCertification::Share (const Handle(StepAP203_CcDesignCertification) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of CertificationAssignment

  iter.AddItem (ent->StepBasic_CertificationAssignment::AssignedCertification());

  // Own fields of CcDesignCertification

  for (Standard_Integer i2=1; i2 <= ent->Items()->Length(); i2++ ) {
    StepAP203_CertifiedItem Var0 = ent->Items()->Value(i2);
    iter.AddItem (Var0.Value());
  }
}
