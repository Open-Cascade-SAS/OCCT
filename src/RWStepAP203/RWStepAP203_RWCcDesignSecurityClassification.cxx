// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <RWStepAP203_RWCcDesignSecurityClassification.ixx>
#include <StepAP203_HArray1OfClassifiedItem.hxx>
#include <StepAP203_ClassifiedItem.hxx>

//=======================================================================
//function : RWStepAP203_RWCcDesignSecurityClassification
//purpose  : 
//=======================================================================

RWStepAP203_RWCcDesignSecurityClassification::RWStepAP203_RWCcDesignSecurityClassification ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepAP203_RWCcDesignSecurityClassification::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                             const Standard_Integer num,
                                                             Handle(Interface_Check)& ach,
                                                             const Handle(StepAP203_CcDesignSecurityClassification) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"cc_design_security_classification") ) return;

  // Inherited fields of SecurityClassificationAssignment

  Handle(StepBasic_SecurityClassification) aSecurityClassificationAssignment_AssignedSecurityClassification;
  data->ReadEntity (num, 1, "security_classification_assignment.assigned_security_classification", ach, STANDARD_TYPE(StepBasic_SecurityClassification), aSecurityClassificationAssignment_AssignedSecurityClassification);

  // Own fields of CcDesignSecurityClassification

  Handle(StepAP203_HArray1OfClassifiedItem) aItems;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "items", ach, sub2) ) {
    Standard_Integer num2 = sub2;
    Standard_Integer nb0 = data->NbParams(num2);
    aItems = new StepAP203_HArray1OfClassifiedItem (1, nb0);
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepAP203_ClassifiedItem anIt0;
      data->ReadEntity (num2, i0, "items", ach, anIt0);
      aItems->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aSecurityClassificationAssignment_AssignedSecurityClassification,
            aItems);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepAP203_RWCcDesignSecurityClassification::WriteStep (StepData_StepWriter& SW,
                                                              const Handle(StepAP203_CcDesignSecurityClassification) &ent) const
{

  // Inherited fields of SecurityClassificationAssignment

  SW.Send (ent->StepBasic_SecurityClassificationAssignment::AssignedSecurityClassification());

  // Own fields of CcDesignSecurityClassification

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->Items()->Length(); i1++ ) {
    StepAP203_ClassifiedItem Var0 = ent->Items()->Value(i1);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepAP203_RWCcDesignSecurityClassification::Share (const Handle(StepAP203_CcDesignSecurityClassification) &ent,
                                                          Interface_EntityIterator& iter) const
{

  // Inherited fields of SecurityClassificationAssignment

  iter.AddItem (ent->StepBasic_SecurityClassificationAssignment::AssignedSecurityClassification());

  // Own fields of CcDesignSecurityClassification

  for (Standard_Integer i2=1; i2 <= ent->Items()->Length(); i2++ ) {
    StepAP203_ClassifiedItem Var0 = ent->Items()->Value(i2);
    iter.AddItem (Var0.Value());
  }
}
