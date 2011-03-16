// File:	RWStepAP203_RWCcDesignCertification.cxx
// Created:	Fri Nov 26 16:26:31 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepAP203_RWCcDesignCertification.ixx>
#include <StepAP203_HArray1OfCertifiedItem.hxx>
#include <StepAP203_CertifiedItem.hxx>

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
