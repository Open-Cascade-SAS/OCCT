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


#include <RWStepShape_RWOrientedEdge.ixx>
#include <StepShape_Edge.hxx>
#include <StepShape_Vertex.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_OrientedEdge.hxx>


RWStepShape_RWOrientedEdge::RWStepShape_RWOrientedEdge () {}

void RWStepShape_RWOrientedEdge::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_OrientedEdge)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,5,ach,"oriented_edge")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : edgeStart ---
	// --- this field is redefined ---
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->CheckDerived(num,2,"edge_start",ach,Standard_False);

	// --- inherited field : edgeEnd ---
	// --- this field is redefined ---
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->CheckDerived(num,3,"edge_end",ach,Standard_False);

	// --- own field : edgeElement ---

	Handle(StepShape_Edge) aEdgeElement;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num, 4,"edge_element", ach, STANDARD_TYPE(StepShape_Edge), aEdgeElement);

	// --- own field : orientation ---

	Standard_Boolean aOrientation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadBoolean (num,5,"orientation",ach,aOrientation);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aEdgeElement, aOrientation);
}


void RWStepShape_RWOrientedEdge::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_OrientedEdge)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field edgeStart ---

	SW.SendDerived();

	// --- inherited field edgeEnd ---

	SW.SendDerived();

	// --- own field : edgeElement ---

	SW.Send(ent->EdgeElement());

	// --- own field : orientation ---

	SW.SendBoolean(ent->Orientation());
}


void RWStepShape_RWOrientedEdge::Share(const Handle(StepShape_OrientedEdge)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->EdgeElement());
}

