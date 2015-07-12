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
#include <RWStepRepr_RWGlobalUncertaintyAssignedContext.hxx>
#include <StepBasic_HArray1OfUncertaintyMeasureWithUnit.hxx>
#include <StepBasic_UncertaintyMeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>

RWStepRepr_RWGlobalUncertaintyAssignedContext::RWStepRepr_RWGlobalUncertaintyAssignedContext () {}

void RWStepRepr_RWGlobalUncertaintyAssignedContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_GlobalUncertaintyAssignedContext)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"global_uncertainty_assigned_context")) return;

	// --- inherited field : contextIdentifier ---

	Handle(TCollection_HAsciiString) aContextIdentifier;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"context_identifier",ach,aContextIdentifier);

	// --- inherited field : contextType ---

	Handle(TCollection_HAsciiString) aContextType;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"context_type",ach,aContextType);

	// --- own field : uncertainty ---

	Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit) aUncertainty;
	Handle(StepBasic_UncertaintyMeasureWithUnit) anent3;
	Standard_Integer nsub3;
	if (data->ReadSubList (num,3,"uncertainty",ach,nsub3)) {
	  Standard_Integer nb3 = data->NbParams(nsub3);
	  aUncertainty = new StepBasic_HArray1OfUncertaintyMeasureWithUnit (1, nb3);
	  for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	    if (data->ReadEntity (nsub3, i3,"uncertainty_measure_with_unit", ach,
				  STANDARD_TYPE(StepBasic_UncertaintyMeasureWithUnit), anent3))
	      aUncertainty->SetValue(i3, anent3);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aContextIdentifier, aContextType, aUncertainty);
}


void RWStepRepr_RWGlobalUncertaintyAssignedContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_GlobalUncertaintyAssignedContext)& ent) const
{

	// --- inherited field contextIdentifier ---

	SW.Send(ent->ContextIdentifier());

	// --- inherited field contextType ---

	SW.Send(ent->ContextType());

	// --- own field : uncertainty ---

	SW.OpenSub();
	for (Standard_Integer i3 = 1;  i3 <= ent->NbUncertainty();  i3 ++) {
	  SW.Send(ent->UncertaintyValue(i3));
	}
	SW.CloseSub();
}


void RWStepRepr_RWGlobalUncertaintyAssignedContext::Share(const Handle(StepRepr_GlobalUncertaintyAssignedContext)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbUncertainty();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->UncertaintyValue(is1));
	}

}

