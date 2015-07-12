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
#include <RWStepShape_RWClosedShell.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_HArray1OfFace.hxx>

RWStepShape_RWClosedShell::RWStepShape_RWClosedShell () {}

void RWStepShape_RWClosedShell::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_ClosedShell)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"closed_shell")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : cfsFaces ---

	Handle(StepShape_HArray1OfFace) aCfsFaces;
	Handle(StepShape_Face) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"cfs_faces",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aCfsFaces = new StepShape_HArray1OfFace (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2, i2,"face", ach, STANDARD_TYPE(StepShape_Face), anent2))
	      aCfsFaces->SetValue(i2, anent2);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aCfsFaces);
}


void RWStepShape_RWClosedShell::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_ClosedShell)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field cfsFaces ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbCfsFaces();  i2 ++) {
	  SW.Send(ent->CfsFacesValue(i2));
	}
	SW.CloseSub();
}


void RWStepShape_RWClosedShell::Share(const Handle(StepShape_ClosedShell)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbCfsFaces();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->CfsFacesValue(is1));
	}

}

