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


#include <RWStepAP214_RWAutoDesignSecurityClassificationAssignment.ixx>
#include <StepBasic_HArray1OfApproval.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_SecurityClassification.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepAP214_AutoDesignSecurityClassificationAssignment.hxx>


RWStepAP214_RWAutoDesignSecurityClassificationAssignment::RWStepAP214_RWAutoDesignSecurityClassificationAssignment () {}

void RWStepAP214_RWAutoDesignSecurityClassificationAssignment::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepAP214_AutoDesignSecurityClassificationAssignment)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"auto_design_security_classification_assignment")) return;

	// --- inherited field : assignedSecurityClassification ---

	Handle(StepBasic_SecurityClassification) aAssignedSecurityClassification;
#ifdef DEB
	Standard_Boolean stat1 = 
#endif
	  data->ReadEntity(num, 1,"assigned_security_classification", ach, STANDARD_TYPE(StepBasic_SecurityClassification), aAssignedSecurityClassification);

	// --- own field : items ---

	Handle(StepBasic_HArray1OfApproval) aItems;
	Handle(StepBasic_Approval) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"items",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aItems = new StepBasic_HArray1OfApproval (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    Standard_Boolean stat2 = data->ReadEntity
	      (nsub2, i2,"auto_design_security_classified_item", ach, STANDARD_TYPE(StepBasic_Approval), anent2);
	      if (stat2) aItems->SetValue(i2, anent2);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aAssignedSecurityClassification, aItems);
}


void RWStepAP214_RWAutoDesignSecurityClassificationAssignment::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepAP214_AutoDesignSecurityClassificationAssignment)& ent) const
{

	// --- inherited field assignedSecurityClassification ---

	SW.Send(ent->AssignedSecurityClassification());

	// --- own field : items ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbItems();  i2 ++) {
	  SW.Send(ent->ItemsValue(i2));
	}
	SW.CloseSub();
}


void RWStepAP214_RWAutoDesignSecurityClassificationAssignment::Share(const Handle(StepAP214_AutoDesignSecurityClassificationAssignment)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->AssignedSecurityClassification());


	Standard_Integer nbElem2 = ent->NbItems();
	for (Standard_Integer is2=1; is2<=nbElem2; is2 ++) {
	  iter.GetOneItem(ent->ItemsValue(is2));
	}

}

