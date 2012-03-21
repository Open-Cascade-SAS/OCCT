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


#include <RWStepBasic_RWDerivedUnit.ixx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <StepBasic_HArray1OfDerivedUnitElement.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_DerivedUnit.hxx>


RWStepBasic_RWDerivedUnit::RWStepBasic_RWDerivedUnit () {}

void RWStepBasic_RWDerivedUnit::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_DerivedUnit)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"derived_unit")) return;

	// --- own field : elements ---

	Handle(StepBasic_HArray1OfDerivedUnitElement) elts;
	Handle(StepBasic_DerivedUnitElement) anelt;
	Standard_Integer nsub1;
	if (data->ReadSubList (num,1,"elements",ach,nsub1)) {
	  Standard_Integer nb1 = data->NbParams(nsub1);
	  elts = new StepBasic_HArray1OfDerivedUnitElement (1,nb1);
	  for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean st1 =` not needed
	    if (data->ReadEntity (nsub1,i1,"element",ach,STANDARD_TYPE(StepBasic_DerivedUnitElement),anelt))
	      elts->SetValue (i1,anelt);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(elts);
}


void RWStepBasic_RWDerivedUnit::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_DerivedUnit)& ent) const
{

	// --- own field : dimensions ---

  Standard_Integer i, nb = ent->NbElements();
  SW.OpenSub();
  for (i = 1; i <= nb; i ++) {
    SW.Send (ent->ElementsValue(i));
  }
  SW.CloseSub();
}


void RWStepBasic_RWDerivedUnit::Share(const Handle(StepBasic_DerivedUnit)& ent, Interface_EntityIterator& iter) const
{

  Standard_Integer i, nb = ent->NbElements();
  for (i = 1; i <= nb; i ++) {
    iter.GetOneItem(ent->ElementsValue(i));
  }
}

