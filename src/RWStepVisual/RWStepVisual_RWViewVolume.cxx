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
#include <RWStepVisual_RWViewVolume.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepVisual_CentralOrParallel.hxx>
#include <StepVisual_PlanarBox.hxx>
#include <StepVisual_ViewVolume.hxx>
#include <TCollection_AsciiString.hxx>

// --- Enum : CentralOrParallel ---
static TCollection_AsciiString copCentral(".CENTRAL.");
static TCollection_AsciiString copParallel(".PARALLEL.");

RWStepVisual_RWViewVolume::RWStepVisual_RWViewVolume () {}

void RWStepVisual_RWViewVolume::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_ViewVolume)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,9,ach,"view_volume")) return;

	// --- own field : projectionType ---

	StepVisual_CentralOrParallel aProjectionType = StepVisual_copCentral;
	if (data->ParamType(num,1) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,1);
	  if      (copCentral.IsEqual(text)) aProjectionType = StepVisual_copCentral;
	  else if (copParallel.IsEqual(text)) aProjectionType = StepVisual_copParallel;
	  else ach->AddFail("Enumeration central_or_parallel has not an allowed value");
	}
	else ach->AddFail("Parameter #1 (projection_type) is not an enumeration");

	// --- own field : projectionPoint ---

	Handle(StepGeom_CartesianPoint) aProjectionPoint;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"projection_point", ach, STANDARD_TYPE(StepGeom_CartesianPoint), aProjectionPoint);

	// --- own field : viewPlaneDistance ---

	Standard_Real aViewPlaneDistance;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"view_plane_distance",ach,aViewPlaneDistance);

	// --- own field : frontPlaneDistance ---

	Standard_Real aFrontPlaneDistance;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"front_plane_distance",ach,aFrontPlaneDistance);

	// --- own field : frontPlaneClipping ---

	Standard_Boolean aFrontPlaneClipping;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadBoolean (num,5,"front_plane_clipping",ach,aFrontPlaneClipping);

	// --- own field : backPlaneDistance ---

	Standard_Real aBackPlaneDistance;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	data->ReadReal (num,6,"back_plane_distance",ach,aBackPlaneDistance);

	// --- own field : backPlaneClipping ---

	Standard_Boolean aBackPlaneClipping;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat7 =` not needed
	data->ReadBoolean (num,7,"back_plane_clipping",ach,aBackPlaneClipping);

	// --- own field : viewVolumeSidesClipping ---

	Standard_Boolean aViewVolumeSidesClipping;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat8 =` not needed
	data->ReadBoolean (num,8,"view_volume_sides_clipping",ach,aViewVolumeSidesClipping);

	// --- own field : viewWindow ---

	Handle(StepVisual_PlanarBox) aViewWindow;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat9 =` not needed
	data->ReadEntity(num, 9,"view_window", ach, STANDARD_TYPE(StepVisual_PlanarBox), aViewWindow);

	//--- Initialisation of the read entity ---


	ent->Init(aProjectionType, aProjectionPoint, aViewPlaneDistance, aFrontPlaneDistance, aFrontPlaneClipping, aBackPlaneDistance, aBackPlaneClipping, aViewVolumeSidesClipping, aViewWindow);
}


void RWStepVisual_RWViewVolume::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_ViewVolume)& ent) const
{

	// --- own field : projectionType ---

	switch(ent->ProjectionType()) {
	  case StepVisual_copCentral : SW.SendEnum (copCentral); break;
	  case StepVisual_copParallel : SW.SendEnum (copParallel); break;
	}

	// --- own field : projectionPoint ---

	SW.Send(ent->ProjectionPoint());

	// --- own field : viewPlaneDistance ---

	SW.Send(ent->ViewPlaneDistance());

	// --- own field : frontPlaneDistance ---

	SW.Send(ent->FrontPlaneDistance());

	// --- own field : frontPlaneClipping ---

	SW.SendBoolean(ent->FrontPlaneClipping());

	// --- own field : backPlaneDistance ---

	SW.Send(ent->BackPlaneDistance());

	// --- own field : backPlaneClipping ---

	SW.SendBoolean(ent->BackPlaneClipping());

	// --- own field : viewVolumeSidesClipping ---

	SW.SendBoolean(ent->ViewVolumeSidesClipping());

	// --- own field : viewWindow ---

	SW.Send(ent->ViewWindow());
}


void RWStepVisual_RWViewVolume::Share(const Handle(StepVisual_ViewVolume)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->ProjectionPoint());


	iter.GetOneItem(ent->ViewWindow());
}

