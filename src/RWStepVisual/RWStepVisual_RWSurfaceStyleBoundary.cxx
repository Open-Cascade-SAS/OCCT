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
#include <RWStepVisual_RWSurfaceStyleBoundary.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_CurveStyle.hxx>
#include <StepVisual_SurfaceStyleBoundary.hxx>

RWStepVisual_RWSurfaceStyleBoundary::RWStepVisual_RWSurfaceStyleBoundary () {}

void RWStepVisual_RWSurfaceStyleBoundary::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_SurfaceStyleBoundary)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"surface_style_boundary")) return;

	// --- own field : styleOfBoundary ---

	Handle(StepVisual_CurveStyle) aStyleOfBoundary;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"style_of_boundary", ach, STANDARD_TYPE(StepVisual_CurveStyle), aStyleOfBoundary);

	//--- Initialisation of the read entity ---


	ent->Init(aStyleOfBoundary);
}


void RWStepVisual_RWSurfaceStyleBoundary::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_SurfaceStyleBoundary)& ent) const
{

	// --- own field : styleOfBoundary ---

	SW.Send(ent->StyleOfBoundary());
}


void RWStepVisual_RWSurfaceStyleBoundary::Share(const Handle(StepVisual_SurfaceStyleBoundary)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->StyleOfBoundary());
}

