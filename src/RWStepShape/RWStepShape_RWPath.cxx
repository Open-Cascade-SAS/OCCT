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


#include <RWStepShape_RWPath.ixx>
#include <StepShape_HArray1OfOrientedEdge.hxx>
#include <StepShape_OrientedEdge.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_Path.hxx>


RWStepShape_RWPath::RWStepShape_RWPath () {}

void RWStepShape_RWPath::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_Path)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"path")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : edgeList ---

	Handle(StepShape_HArray1OfOrientedEdge) aEdgeList;
	Handle(StepShape_OrientedEdge) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"edge_list",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aEdgeList = new StepShape_HArray1OfOrientedEdge (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2, i2,"oriented_edge", ach, STANDARD_TYPE(StepShape_OrientedEdge), anent2))
	      aEdgeList->SetValue(i2, anent2);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aEdgeList);
}


void RWStepShape_RWPath::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_Path)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : edgeList ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbEdgeList();  i2 ++) {
	  SW.Send(ent->EdgeListValue(i2));
	}
	SW.CloseSub();
}


void RWStepShape_RWPath::Share(const Handle(StepShape_Path)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbEdgeList();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->EdgeListValue(is1));
	}

}

