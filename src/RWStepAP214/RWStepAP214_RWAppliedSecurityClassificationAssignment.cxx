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


#include <RWStepAP214_RWAppliedSecurityClassificationAssignment.ixx>
#include <StepAP214_HArray1OfSecurityClassificationItem.hxx>
#include <StepAP214_SecurityClassificationItem.hxx>
#include <StepBasic_SecurityClassification.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepAP214_AppliedSecurityClassificationAssignment.hxx>


RWStepAP214_RWAppliedSecurityClassificationAssignment::RWStepAP214_RWAppliedSecurityClassificationAssignment () {}

void RWStepAP214_RWAppliedSecurityClassificationAssignment::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepAP214_AppliedSecurityClassificationAssignment)& ent) const
{


  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,2,ach,"applied_security_classification_assignment")) return;
  
  // --- inherited field : assignedSecurityClassification ---
  
  Handle(StepBasic_SecurityClassification) aAssignedSecurityClassification;
#ifdef DEB
  Standard_Boolean stat1 = 
#endif
    data->ReadEntity(num, 1,"assigned_security_classification", ach, STANDARD_TYPE(StepBasic_SecurityClassification), aAssignedSecurityClassification);
  
  // --- own field : items ---
  
  Handle(StepAP214_HArray1OfSecurityClassificationItem) aItems;
  StepAP214_SecurityClassificationItem anent2;
  Standard_Integer nsub2;
  if (data->ReadSubList (num,2,"items",ach,nsub2)) {
    Standard_Integer nb2 = data->NbParams(nsub2);
    aItems = new StepAP214_HArray1OfSecurityClassificationItem (1, nb2);
    for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
      Standard_Boolean stat2 = data->ReadEntity
	(nsub2, i2,"items", ach, anent2); 
      if (stat2) aItems->SetValue(i2, anent2);
    }
  }

  //--- Initialisation of the read entity ---


  ent->Init(aAssignedSecurityClassification, aItems);
}


void RWStepAP214_RWAppliedSecurityClassificationAssignment::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepAP214_AppliedSecurityClassificationAssignment)& ent) const
{

  // --- inherited field assignedSecurityClassification ---

  SW.Send(ent->AssignedSecurityClassification());

  // --- own field : items ---

  SW.OpenSub();
  for (Standard_Integer i2 = 1;  i2 <= ent->NbItems();  i2 ++) {
    SW.Send(ent->ItemsValue(i2).Value());
  }
  SW.CloseSub();
}


void RWStepAP214_RWAppliedSecurityClassificationAssignment::Share(const Handle(StepAP214_AppliedSecurityClassificationAssignment)& ent, Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->AssignedSecurityClassification());

  Standard_Integer nbElem2 = ent->NbItems();
  for (Standard_Integer is2=1; is2<=nbElem2; is2 ++) {
    iter.GetOneItem(ent->ItemsValue(is2).Value());
  }
  
}

