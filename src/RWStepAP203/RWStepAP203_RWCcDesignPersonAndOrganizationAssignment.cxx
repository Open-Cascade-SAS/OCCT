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

#include <RWStepAP203_RWCcDesignPersonAndOrganizationAssignment.ixx>
#include <StepAP203_HArray1OfPersonOrganizationItem.hxx>
#include <StepAP203_PersonOrganizationItem.hxx>

//=======================================================================
//function : RWStepAP203_RWCcDesignPersonAndOrganizationAssignment
//purpose  : 
//=======================================================================

RWStepAP203_RWCcDesignPersonAndOrganizationAssignment::RWStepAP203_RWCcDesignPersonAndOrganizationAssignment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepAP203_RWCcDesignPersonAndOrganizationAssignment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                      const Standard_Integer num,
                                                                      Handle(Interface_Check)& ach,
                                                                      const Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"cc_design_person_and_organization_assignment") ) return;

  // Inherited fields of PersonAndOrganizationAssignment

  Handle(StepBasic_PersonAndOrganization) aPersonAndOrganizationAssignment_AssignedPersonAndOrganization;
  data->ReadEntity (num, 1, "person_and_organization_assignment.assigned_person_and_organization", ach, STANDARD_TYPE(StepBasic_PersonAndOrganization), aPersonAndOrganizationAssignment_AssignedPersonAndOrganization);

  Handle(StepBasic_PersonAndOrganizationRole) aPersonAndOrganizationAssignment_Role;
  data->ReadEntity (num, 2, "person_and_organization_assignment.role", ach, STANDARD_TYPE(StepBasic_PersonAndOrganizationRole), aPersonAndOrganizationAssignment_Role);

  // Own fields of CcDesignPersonAndOrganizationAssignment

  Handle(StepAP203_HArray1OfPersonOrganizationItem) aItems;
  Standard_Integer sub3 = 0;
  if ( data->ReadSubList (num, 3, "items", ach, sub3) ) {
    Standard_Integer num2 = sub3;
    Standard_Integer nb0 = data->NbParams(num2);
    aItems = new StepAP203_HArray1OfPersonOrganizationItem (1, nb0);
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepAP203_PersonOrganizationItem anIt0;
      data->ReadEntity (num2, i0, "items", ach, anIt0);
      aItems->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aPersonAndOrganizationAssignment_AssignedPersonAndOrganization,
            aPersonAndOrganizationAssignment_Role,
            aItems);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepAP203_RWCcDesignPersonAndOrganizationAssignment::WriteStep (StepData_StepWriter& SW,
                                                                       const Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) &ent) const
{

  // Inherited fields of PersonAndOrganizationAssignment

  SW.Send (ent->StepBasic_PersonAndOrganizationAssignment::AssignedPersonAndOrganization());

  SW.Send (ent->StepBasic_PersonAndOrganizationAssignment::Role());

  // Own fields of CcDesignPersonAndOrganizationAssignment

  SW.OpenSub();
  for (Standard_Integer i2=1; i2 <= ent->Items()->Length(); i2++ ) {
    StepAP203_PersonOrganizationItem Var0 = ent->Items()->Value(i2);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepAP203_RWCcDesignPersonAndOrganizationAssignment::Share (const Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) &ent,
                                                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of PersonAndOrganizationAssignment

  iter.AddItem (ent->StepBasic_PersonAndOrganizationAssignment::AssignedPersonAndOrganization());

  iter.AddItem (ent->StepBasic_PersonAndOrganizationAssignment::Role());

  // Own fields of CcDesignPersonAndOrganizationAssignment

  for (Standard_Integer i3=1; i3 <= ent->Items()->Length(); i3++ ) {
    StepAP203_PersonOrganizationItem Var0 = ent->Items()->Value(i3);
    iter.AddItem (Var0.Value());
  }
}
