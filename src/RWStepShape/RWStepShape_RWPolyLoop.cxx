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
#include <RWStepShape_RWPolyLoop.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepShape_PolyLoop.hxx>

RWStepShape_RWPolyLoop::RWStepShape_RWPolyLoop () {}

void RWStepShape_RWPolyLoop::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_PolyLoop)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"poly_loop")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : polygon ---

	Handle(StepGeom_HArray1OfCartesianPoint) aPolygon;
	Handle(StepGeom_CartesianPoint) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"polygon",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aPolygon = new StepGeom_HArray1OfCartesianPoint (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2, i2,"cartesian_point", ach, STANDARD_TYPE(StepGeom_CartesianPoint), anent2))
	      aPolygon->SetValue(i2, anent2);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPolygon);
}


void RWStepShape_RWPolyLoop::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_PolyLoop)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : polygon ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbPolygon();  i2 ++) {
	  SW.Send(ent->PolygonValue(i2));
	}
	SW.CloseSub();
}


void RWStepShape_RWPolyLoop::Share(const Handle(StepShape_PolyLoop)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbPolygon();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->PolygonValue(is1));
	}

}

