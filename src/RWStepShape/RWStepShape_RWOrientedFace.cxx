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
#include <RWStepShape_RWOrientedFace.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_HArray1OfFaceBound.hxx>
#include <StepShape_OrientedFace.hxx>

RWStepShape_RWOrientedFace::RWStepShape_RWOrientedFace () {}

void RWStepShape_RWOrientedFace::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_OrientedFace)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"oriented_face")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : bounds ---
	// --- this field is redefined ---
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->CheckDerived(num,2,"bounds",ach,Standard_False);

	// --- own field : faceElement ---

	Handle(StepShape_Face) aFaceElement;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"face_element", ach, STANDARD_TYPE(StepShape_Face), aFaceElement);

	// --- own field : orientation ---

	Standard_Boolean aOrientation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadBoolean (num,4,"orientation",ach,aOrientation);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aFaceElement, aOrientation);
}


void RWStepShape_RWOrientedFace::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_OrientedFace)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field bounds ---

	SW.SendDerived();

	// --- own field : faceElement ---

	SW.Send(ent->FaceElement());

	// --- own field : orientation ---

	SW.SendBoolean(ent->Orientation());
}


void RWStepShape_RWOrientedFace::Share(const Handle(StepShape_OrientedFace)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->FaceElement());
}

