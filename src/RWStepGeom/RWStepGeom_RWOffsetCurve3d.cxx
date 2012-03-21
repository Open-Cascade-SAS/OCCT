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


#include <RWStepGeom_RWOffsetCurve3d.ixx>
#include <StepGeom_Curve.hxx>
#include <StepData_Logical.hxx>
#include <StepGeom_Direction.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_OffsetCurve3d.hxx>


RWStepGeom_RWOffsetCurve3d::RWStepGeom_RWOffsetCurve3d () {}

void RWStepGeom_RWOffsetCurve3d::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_OffsetCurve3d)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,5,ach,"offset_curve_3d")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : basisCurve ---

	Handle(StepGeom_Curve) aBasisCurve;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"basis_curve", ach, STANDARD_TYPE(StepGeom_Curve), aBasisCurve);

	// --- own field : distance ---

	Standard_Real aDistance;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"distance",ach,aDistance);

	// --- own field : selfIntersect ---

	StepData_Logical aSelfIntersect;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadLogical (num,4,"self_intersect",ach,aSelfIntersect);

	// --- own field : refDirection ---

	Handle(StepGeom_Direction) aRefDirection;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadEntity(num, 5,"ref_direction", ach, STANDARD_TYPE(StepGeom_Direction), aRefDirection);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aBasisCurve, aDistance, aSelfIntersect, aRefDirection);
}


void RWStepGeom_RWOffsetCurve3d::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_OffsetCurve3d)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : basisCurve ---

	SW.Send(ent->BasisCurve());

	// --- own field : distance ---

	SW.Send(ent->Distance());

	// --- own field : selfIntersect ---

	SW.SendLogical(ent->SelfIntersect());

	// --- own field : refDirection ---

	SW.Send(ent->RefDirection());
}


void RWStepGeom_RWOffsetCurve3d::Share(const Handle(StepGeom_OffsetCurve3d)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->BasisCurve());


	iter.GetOneItem(ent->RefDirection());
}

