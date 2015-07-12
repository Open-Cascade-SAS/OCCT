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

//:k4 abv 30.11.98: TR9: warnings for BWV

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <RWStepShape_RWBrepWithVoids.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_HArray1OfOrientedClosedShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>

RWStepShape_RWBrepWithVoids::RWStepShape_RWBrepWithVoids () {}

void RWStepShape_RWBrepWithVoids::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_BrepWithVoids)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"brep_with_voids")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : outer ---

	Handle(StepShape_ClosedShell) aOuter;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"outer", ach, STANDARD_TYPE(StepShape_ClosedShell), aOuter);

	// --- own field : voids ---

	Handle(StepShape_HArray1OfOrientedClosedShell) aVoids;
	Handle(StepShape_OrientedClosedShell) anent3;
	Standard_Integer nsub3;
	if (data->ReadSubList (num,3,"voids",ach,nsub3)) {
	  Standard_Integer nb3 = data->NbParams(nsub3);
	  aVoids = new StepShape_HArray1OfOrientedClosedShell (1, nb3);
	  for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	    if (data->ReadEntity (nsub3, i3,"oriented_closed_shell", ach,
				  STANDARD_TYPE(StepShape_OrientedClosedShell), anent3))
	      aVoids->SetValue(i3, anent3);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aOuter, aVoids);
}


void RWStepShape_RWBrepWithVoids::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_BrepWithVoids)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field outer ---

	SW.Send(ent->Outer());

	// --- own field : voids ---

	SW.OpenSub();
	for (Standard_Integer i3 = 1;  i3 <= ent->NbVoids();  i3 ++) {
	  SW.Send(ent->VoidsValue(i3));
	}
	SW.CloseSub();
}


void RWStepShape_RWBrepWithVoids::Share(const Handle(StepShape_BrepWithVoids)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Outer());


	Standard_Integer nbElem2 = ent->NbVoids();
	for (Standard_Integer is2=1; is2<=nbElem2; is2 ++) {
	  iter.GetOneItem(ent->VoidsValue(is2));
	}

}

//:k4 abv 30 Nov 98: ProSTEP TR9: add warning
void RWStepShape_RWBrepWithVoids::Check
  (const Handle(StepShape_BrepWithVoids)& ent,
   const Interface_ShareTool& ,
   Handle(Interface_Check)& ach) const
{
  for ( Standard_Integer i=1; i <= ent->NbVoids(); i++ ) 
    if ( ent->VoidsValue(i)->Orientation() ) {
      ach->AddWarning ("Void has orientation .T. while .F. is required by API 214");
      break;
    }
}
