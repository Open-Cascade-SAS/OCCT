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


#include <RWStepShape_RWLoopAndPath.ixx>
#include <StepShape_Loop.hxx>
#include <StepShape_Path.hxx>
#include <StepShape_HArray1OfOrientedEdge.hxx>
#include <StepShape_OrientedEdge.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_LoopAndPath.hxx>


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

