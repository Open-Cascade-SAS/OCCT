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
#include <RWStepBasic_RWApprovalDateTime.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalDateTime.hxx>
#include <StepBasic_DateTimeSelect.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWApprovalDateTime::RWStepBasic_RWApprovalDateTime () {}

void RWStepBasic_RWApprovalDateTime::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ApprovalDateTime)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"approval_date_time")) return;

	// --- own fields

	StepBasic_DateTimeSelect dts;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num,1,"date_time", ach, dts);

	Handle(StepBasic_Approval) ap;
	//szv#4:S4163:12Mar99 `stat1 =` not needed
	data->ReadEntity (num,2,"dated_approval",ach, STANDARD_TYPE(StepBasic_Approval), ap);

	//--- Initialisation of the read entity ---


	ent->Init(dts,ap);
}


void RWStepBasic_RWApprovalDateTime::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ApprovalDateTime)& ent) const
{

	// --- own field : dimensions ---

	SW.Send(ent->DateTime().Value());
	SW.Send(ent->DatedApproval());
}


void RWStepBasic_RWApprovalDateTime::Share(const Handle(StepBasic_ApprovalDateTime)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->DateTime().Value());
	iter.GetOneItem(ent->DatedApproval());
}
