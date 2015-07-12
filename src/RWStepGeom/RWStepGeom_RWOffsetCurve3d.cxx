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
#include <RWStepGeom_RWOffsetCurve3d.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_Direction.hxx>
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

