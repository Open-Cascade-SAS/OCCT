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


#include <RWStepVisual_RWPlanarExtent.ixx>


RWStepVisual_RWPlanarExtent::RWStepVisual_RWPlanarExtent () {}

void RWStepVisual_RWPlanarExtent::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_PlanarExtent)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"planar_extent")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : sizeInX ---

	Standard_Real aSizeInX;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadReal (num,2,"size_in_x",ach,aSizeInX);

	// --- own field : sizeInY ---

	Standard_Real aSizeInY;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"size_in_y",ach,aSizeInY);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aSizeInX, aSizeInY);
}


void RWStepVisual_RWPlanarExtent::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_PlanarExtent)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : sizeInX ---

	SW.Send(ent->SizeInX());

	// --- own field : sizeInY ---

	SW.Send(ent->SizeInY());
}
