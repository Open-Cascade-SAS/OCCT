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


#include <RWStepShape_RWFaceSurface.ixx>
#include <StepGeom_Surface.hxx>
#include <StepShape_HArray1OfFaceBound.hxx>
#include <StepShape_FaceBound.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_FaceSurface.hxx>


RWStepShape_RWFaceSurface::RWStepShape_RWFaceSurface () {}

void RWStepShape_RWFaceSurface::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_FaceSurface)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"face_surface")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : bounds ---

	Handle(StepShape_HArray1OfFaceBound) aBounds;
	Handle(StepShape_FaceBound) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"bounds",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aBounds = new StepShape_HArray1OfFaceBound (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2, i2,"face_bound", ach, STANDARD_TYPE(StepShape_FaceBound), anent2))
	      aBounds->SetValue(i2, anent2);
	  }
	}

	// --- own field : faceGeometry ---

	Handle(StepGeom_Surface) aFaceGeometry;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"face_geometry", ach, STANDARD_TYPE(StepGeom_Surface), aFaceGeometry);

	// --- own field : sameSense ---

	Standard_Boolean aSameSense;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadBoolean (num,4,"same_sense",ach,aSameSense);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aBounds, aFaceGeometry, aSameSense);
}


void RWStepShape_RWFaceSurface::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_FaceSurface)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field bounds ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbBounds();  i2 ++) {
	  SW.Send(ent->BoundsValue(i2));
	}
	SW.CloseSub();

	// --- own field : faceGeometry ---

	SW.Send(ent->FaceGeometry());

	// --- own field : sameSense ---

	SW.SendBoolean(ent->SameSense());
}


void RWStepShape_RWFaceSurface::Share(const Handle(StepShape_FaceSurface)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbBounds();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->BoundsValue(is1));
	}



	iter.GetOneItem(ent->FaceGeometry());
}

