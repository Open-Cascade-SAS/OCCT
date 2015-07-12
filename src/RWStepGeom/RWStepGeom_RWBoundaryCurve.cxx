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
#include <RWStepGeom_RWBoundaryCurve.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BoundaryCurve.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
#include <StepGeom_HArray1OfCompositeCurveSegment.hxx>

RWStepGeom_RWBoundaryCurve::RWStepGeom_RWBoundaryCurve () {}

void RWStepGeom_RWBoundaryCurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_BoundaryCurve)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"boundary_curve")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : segments ---

	Handle(StepGeom_HArray1OfCompositeCurveSegment) aSegments;
	Handle(StepGeom_CompositeCurveSegment) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"segments",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aSegments = new StepGeom_HArray1OfCompositeCurveSegment (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2, i2,"composite_curve_segment", ach,
				  STANDARD_TYPE(StepGeom_CompositeCurveSegment), anent2))
	      aSegments->SetValue(i2, anent2);
	  }
	}

	// --- inherited field : selfIntersect ---

	StepData_Logical aSelfIntersect;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadLogical (num,3,"self_intersect",ach,aSelfIntersect);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aSegments, aSelfIntersect);
}


void RWStepGeom_RWBoundaryCurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_BoundaryCurve)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field segments ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbSegments();  i2 ++) {
	  SW.Send(ent->SegmentsValue(i2));
	}
	SW.CloseSub();

	// --- inherited field selfIntersect ---

	SW.SendLogical(ent->SelfIntersect());
}


void RWStepGeom_RWBoundaryCurve::Share(const Handle(StepGeom_BoundaryCurve)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbSegments();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->SegmentsValue(is1));
	}

}

