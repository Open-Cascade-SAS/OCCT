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
#include <RWStepBasic_RWDerivedUnit.hxx>
#include <StepBasic_DerivedUnit.hxx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <StepBasic_HArray1OfDerivedUnitElement.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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

