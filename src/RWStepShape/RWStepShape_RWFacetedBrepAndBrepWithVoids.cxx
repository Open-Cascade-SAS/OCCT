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
#include <RWStepShape_RWFacetedBrepAndBrepWithVoids.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_HArray1OfOrientedClosedShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>

RWStepShape_RWFacetedBrepAndBrepWithVoids::RWStepShape_RWFacetedBrepAndBrepWithVoids () {}

void RWStepShape_RWFacetedBrepAndBrepWithVoids::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_FacetedBrepAndBrepWithVoids)& ent) const
{

	Standard_Integer num = num0;


	// --- Instance of plex componant BrepWithVoids ---

	if (!data->CheckNbParams(num,1,ach,"brep_with_voids")) return;

	// --- field : voids ---

	Handle(StepShape_HArray1OfOrientedClosedShell) aVoids;
	Handle(StepShape_OrientedClosedShell) anent;
	Standard_Integer nsub1;
	if (data->ReadSubList (num,1,"voids",ach,nsub1)) {
	  Standard_Integer nb1 = data->NbParams(nsub1);
	  aVoids = new StepShape_HArray1OfOrientedClosedShell (1, nb1);
	  for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	    if (data->ReadEntity (nsub1, i1,"oriented_closed_shell", ach,
				  STANDARD_TYPE(StepShape_OrientedClosedShell), anent))
	      aVoids->SetValue(i1, anent);
	  }
	}

	num = data->NextForComplex(num);

	// --- Instance of plex componant FacetedBrep ---

	if (!data->CheckNbParams(num,0,ach,"faceted_brep")) return;

	num = data->NextForComplex(num);

	// --- Instance of plex componant GeometricRepresentationItem ---

	if (!data->CheckNbParams(num,0,ach,"geometric_representation_item")) return;

	num = data->NextForComplex(num);

	// --- Instance of common supertype ManifoldSolidBrep ---

	if (!data->CheckNbParams(num,1,ach,"manifold_solid_brep")) return;
	// --- field : outer ---


	Handle(StepShape_ClosedShell) aOuter;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 1,"outer", ach, STANDARD_TYPE(StepShape_ClosedShell), aOuter);

	num = data->NextForComplex(num);

	// --- Instance of plex componant RepresentationItem ---

	if (!data->CheckNbParams(num,1,ach,"representation_item")) return;


	// --- field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat10 =` not needed
	data->ReadString (num,1,"name",ach,aName);
	
	num = data->NextForComplex(num);

	// --- Instance of plex componant SolidModel ---

	if (!data->CheckNbParams(num,0,ach,"solid_model")) return;

	//--- Initialisation of the red entity ---

	ent->Init(aName, aOuter,aVoids);
}


void RWStepShape_RWFacetedBrepAndBrepWithVoids::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_FacetedBrepAndBrepWithVoids)& ent) const
{

	// --- Instance of plex componant BrepWithVoids ---

	SW.StartEntity("BREP_WITH_VOIDS");
	// --- field : voids ---

	SW.OpenSub();
	for (Standard_Integer i1 = 1;  i1 <= ent->NbVoids();  i1 ++) {
	  SW.Send(ent->VoidsValue(i1));
	}
	SW.CloseSub();

	// --- Instance of plex componant FacetedBrep ---

	SW.StartEntity("FACETED_BREP");

	// --- Instance of plex componant GeometricRepresentationItem ---

	SW.StartEntity("GEOMETRIC_REPRESENTATION_ITEM");

	// --- Instance of common supertype ManifoldSolidBrep ---

	SW.StartEntity("MANIFOLD_SOLID_BREP");
	// --- field : outer ---

	SW.Send(ent->Outer());

	// --- Instance of plex componant RepresentationItem ---

	SW.StartEntity("REPRESENTATION_ITEM");
	// --- field : name ---

	SW.Send(ent->Name());

	// --- Instance of plex componant SolidModel ---

	SW.StartEntity("SOLID_MODEL");
}


void RWStepShape_RWFacetedBrepAndBrepWithVoids::Share(const Handle(StepShape_FacetedBrepAndBrepWithVoids)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Outer());
	
	Standard_Integer nbElem2 = ent->NbVoids();
	for (Standard_Integer is2=1; is2<=nbElem2; is2 ++) {
	  iter.GetOneItem(ent->VoidsValue(is2));
	}

}

