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


#include <RWStepBasic_RWOrdinalDate.ixx>


RWStepBasic_RWOrdinalDate::RWStepBasic_RWOrdinalDate () {}

void RWStepBasic_RWOrdinalDate::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_OrdinalDate)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"ordinal_date")) return;

	// --- inherited field : yearComponent ---

	Standard_Integer aYearComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"year_component",ach,aYearComponent);

	// --- own field : dayComponent ---

	Standard_Integer aDayComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadInteger (num,2,"day_component",ach,aDayComponent);

	//--- Initialisation of the read entity ---


	ent->Init(aYearComponent, aDayComponent);
}


void RWStepBasic_RWOrdinalDate::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_OrdinalDate)& ent) const
{

	// --- inherited field yearComponent ---

	SW.Send(ent->YearComponent());

	// --- own field : dayComponent ---

	SW.Send(ent->DayComponent());
}
