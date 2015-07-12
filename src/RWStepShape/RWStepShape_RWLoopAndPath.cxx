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
#include <RWStepShape_RWLoopAndPath.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_HArray1OfOrientedEdge.hxx>
#include <StepShape_Loop.hxx>
#include <StepShape_LoopAndPath.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_Path.hxx>

RWStepShape_RWLoopAndPath::RWStepShape_RWLoopAndPath () {}

void RWStepShape_RWLoopAndPath::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_LoopAndPath)& ent) const
{

	Standard_Integer num = num0;


	// --- Instance of plex componant Loop ---

	if (!data->CheckNbParams(num,0,ach,"loop")) return;

	num = data->NextForComplex(num);

	// --- Instance of plex componant Path ---

	if (!data->CheckNbParams(num,1,ach,"path")) return;

	// --- field : edgeList ---

	Handle(StepShape_HArray1OfOrientedEdge) aEdgeList;
	Handle(StepShape_OrientedEdge) anent1;
	Standard_Integer nsub1;
	if (data->ReadSubList (num,1,"edge_list",ach,nsub1)) {
	  Standard_Integer nb1 = data->NbParams(nsub1);
	  aEdgeList = new StepShape_HArray1OfOrientedEdge (1, nb1);
	  for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	    if (data->ReadEntity (nsub1, i1,"oriented_edge", ach, STANDARD_TYPE(StepShape_OrientedEdge), anent1))
	      aEdgeList->SetValue(i1, anent1);
	  }
	}

	num = data->NextForComplex(num);

	// --- Instance of plex componant RepresentationItem ---

	if (!data->CheckNbParams(num,1,ach,"representation_item")) return;

	// --- field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	num = data->NextForComplex(num);

	// --- Instance of common supertype TopologicalRepresentationItem ---

	if (!data->CheckNbParams(num,0,ach,"topological_representation_item")) return;

	//--- Initialisation of the red entity ---

	ent->Init(aName,aEdgeList);
}


void RWStepShape_RWLoopAndPath::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_LoopAndPath)& ent) const
{

	// --- Instance of plex componant Loop ---

	SW.StartEntity("LOOP");

	// --- Instance of plex componant Path ---

	SW.StartEntity("PATH");
	// --- field : edgeList ---

	SW.OpenSub();
	for (Standard_Integer i1 = 1;  i1 <= ent->NbEdgeList();  i1 ++) {
	  SW.Send(ent->EdgeListValue(i1));
	}
	SW.CloseSub();

	// --- Instance of plex componant RepresentationItem ---

	SW.StartEntity("REPRESENTATION_ITEM");
	// --- field : name ---

	SW.Send(ent->Name());

	// --- Instance of common supertype TopologicalRepresentationItem ---

	SW.StartEntity("TOPOLOGICAL_REPRESENTATION_ITEM");
}


void RWStepShape_RWLoopAndPath::Share(const Handle(StepShape_LoopAndPath)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbEdgeList();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->EdgeListValue(is1));
	}

}

