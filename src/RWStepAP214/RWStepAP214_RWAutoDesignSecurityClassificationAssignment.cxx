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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepAP214_RWAutoDesignSecurityClassificationAssignment.hxx>
#include <StepAP214_AutoDesignSecurityClassificationAssignment.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_HArray1OfApproval.hxx>
#include <StepBasic_SecurityClassification.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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

