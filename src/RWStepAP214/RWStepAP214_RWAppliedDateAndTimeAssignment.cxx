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

//create  s4134 from CC2 to DIS
#include <RWStepAP214_RWAppliedDateAndTimeAssignment.ixx>
#include <StepAP214_HArray1OfDateAndTimeItem.hxx>
#include <StepAP214_DateAndTimeItem.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_DateTimeRole.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepAP214_AppliedDateAndTimeAssignment.hxx>


RWStepAP214_RWAppliedDateAndTimeAssignment::RWStepAP214_RWAppliedDateAndTimeAssignment () {}

void RWStepAP214_RWAppliedDateAndTimeAssignment::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepAP214_AppliedDateAndTimeAssignment)& ent) const
{


  // --- Number of Parameter Control ---
  
  if (!data->CheckNbParams(num,3,ach,"applied_date_and_time_assignment")) return;

  // --- inherited field : assignedDateAndTime ---
  
  Handle(StepBasic_DateAndTime) aAssignedDateAndTime;
  data->ReadEntity(num, 1,"assigned_date_and_time", ach, STANDARD_TYPE(StepBasic_DateAndTime), aAssignedDateAndTime);
  
  // --- inherited field : role ---
  
  Handle(StepBasic_DateTimeRole) aRole;
  data->ReadEntity(num, 2,"role", ach, STANDARD_TYPE(StepBasic_DateTimeRole), aRole);
  
  // --- own field : items ---
  
  Handle(StepAP214_HArray1OfDateAndTimeItem) aItems;
  StepAP214_DateAndTimeItem aItemsItem;
  Standard_Integer nsub3;
  if (data->ReadSubList(num,3,"items",ach,nsub3)) {
    Standard_Integer nb3 = data->NbParams(nsub3);
    aItems = new StepAP214_HArray1OfDateAndTimeItem (1, nb3);
    for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
      Standard_Boolean stat3 = data->ReadEntity
	(nsub3,i3,"items",ach,aItemsItem); 
      if (stat3) aItems->SetValue(i3,aItemsItem);
    }
  }

  //--- Initialisation of the read entity ---


  ent->Init(aAssignedDateAndTime, aRole, aItems);
}


void RWStepAP214_RWAppliedDateAndTimeAssignment::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepAP214_AppliedDateAndTimeAssignment)& ent) const
{

  // --- inherited field assignedDateAndTime ---
  
  SW.Send(ent->AssignedDateAndTime());
  
  // --- inherited field role ---

  SW.Send(ent->Role());

  // --- own field : items ---

  SW.OpenSub();
  for (Standard_Integer i3 = 1;  i3 <= ent->NbItems();  i3 ++) {
    SW.Send(ent->ItemsValue(i3).Value());
  }
  SW.CloseSub();
}


void RWStepAP214_RWAppliedDateAndTimeAssignment::Share(const Handle(StepAP214_AppliedDateAndTimeAssignment)& ent, Interface_EntityIterator& iter) const
{

  iter.GetOneItem(ent->AssignedDateAndTime());
  iter.GetOneItem(ent->Role());
  Standard_Integer nbElem3 = ent->NbItems();
  for (Standard_Integer is3=1; is3<=nbElem3; is3 ++) {
    iter.GetOneItem(ent->ItemsValue(is3).Value());
  }

}

