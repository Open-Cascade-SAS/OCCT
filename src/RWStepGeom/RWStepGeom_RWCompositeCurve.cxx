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


#include <RWStepGeom_RWCompositeCurve.ixx>
#include <StepGeom_HArray1OfCompositeCurveSegment.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
#include <StepData_Logical.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_CompositeCurve.hxx>


RWStepGeom_RWCompositeCurve::RWStepGeom_RWCompositeCurve () {}

void RWStepGeom_RWCompositeCurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_CompositeCurve)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"composite_curve")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : segments ---

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

	// --- own field : selfIntersect ---

	StepData_Logical aSelfIntersect;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadLogical (num,3,"self_intersect",ach,aSelfIntersect);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aSegments, aSelfIntersect);
}


void RWStepGeom_RWCompositeCurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_CompositeCurve)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : segments ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbSegments();  i2 ++) {
	  SW.Send(ent->SegmentsValue(i2));
	}
	SW.CloseSub();

	// --- own field : selfIntersect ---

	SW.SendLogical(ent->SelfIntersect());
}


void RWStepGeom_RWCompositeCurve::Share(const Handle(StepGeom_CompositeCurve)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbSegments();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->SegmentsValue(is1));
	}

}

