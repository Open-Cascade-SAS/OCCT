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
#include <Interface_ShareTool.hxx>
#include <RWStepGeom_RWToroidalSurface.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_ToroidalSurface.hxx>

RWStepGeom_RWToroidalSurface::RWStepGeom_RWToroidalSurface () {}

void RWStepGeom_RWToroidalSurface::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_ToroidalSurface)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"toroidal_surface")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : position ---

	Handle(StepGeom_Axis2Placement3d) aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"position", ach, STANDARD_TYPE(StepGeom_Axis2Placement3d), aPosition);

	// --- own field : majorRadius ---

	Standard_Real aMajorRadius;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"major_radius",ach,aMajorRadius);

	// --- own field : minorRadius ---

	Standard_Real aMinorRadius;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"minor_radius",ach,aMinorRadius);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition, aMajorRadius, aMinorRadius);
}


void RWStepGeom_RWToroidalSurface::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_ToroidalSurface)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field position ---

	SW.Send(ent->Position());

	// --- own field : majorRadius ---

	SW.Send(ent->MajorRadius());

	// --- own field : minorRadius ---

	SW.Send(ent->MinorRadius());
}


void RWStepGeom_RWToroidalSurface::Share(const Handle(StepGeom_ToroidalSurface)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position());
}



void RWStepGeom_RWToroidalSurface::Check
  (const Handle(StepGeom_ToroidalSurface)& ent,
   const Interface_ShareTool& ,
   Handle(Interface_Check)& ach) const
{
//  cout << "------ calling CheckToroidalSurface ------" << endl;
  if (ent->MajorRadius() < 0.0)
    ach->AddFail("ERROR: ToroidalSurface: MajorRadius < 0.0");
  if (ent->MinorRadius() < 0.0)
    ach->AddFail("ERROR: ToroidalSurface: MinorRadius < 0.0");
  if (ent->MajorRadius() < ent->MinorRadius())
    ach->AddWarning("ToroidalSurface: MajorRadius smaller than MinorRadius");
}
