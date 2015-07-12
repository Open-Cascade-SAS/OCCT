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
#include <RWStepVisual_RWSurfaceStyleUsage.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_SurfaceSide.hxx>
#include <StepVisual_SurfaceSideStyle.hxx>
#include <StepVisual_SurfaceStyleUsage.hxx>
#include <TCollection_AsciiString.hxx>

// --- Enum : SurfaceSide ---
static TCollection_AsciiString ssNegative(".NEGATIVE.");
static TCollection_AsciiString ssPositive(".POSITIVE.");
static TCollection_AsciiString ssBoth(".BOTH.");

RWStepVisual_RWSurfaceStyleUsage::RWStepVisual_RWSurfaceStyleUsage () {}

void RWStepVisual_RWSurfaceStyleUsage::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_SurfaceStyleUsage)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"surface_style_usage")) return;

	// --- own field : side ---

	StepVisual_SurfaceSide aSide = StepVisual_ssNegative;
	if (data->ParamType(num,1) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,1);
	  if      (ssNegative.IsEqual(text)) aSide = StepVisual_ssNegative;
	  else if (ssPositive.IsEqual(text)) aSide = StepVisual_ssPositive;
	  else if (ssBoth.IsEqual(text)) aSide = StepVisual_ssBoth;
	  else ach->AddFail("Enumeration surface_side has not an allowed value");
	}
	else ach->AddFail("Parameter #1 (side) is not an enumeration");

	// --- own field : style ---

	Handle(StepVisual_SurfaceSideStyle) aStyle;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"style", ach, STANDARD_TYPE(StepVisual_SurfaceSideStyle), aStyle);

	//--- Initialisation of the read entity ---


	ent->Init(aSide, aStyle);
}


void RWStepVisual_RWSurfaceStyleUsage::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_SurfaceStyleUsage)& ent) const
{

	// --- own field : side ---

	switch(ent->Side()) {
	  case StepVisual_ssNegative : SW.SendEnum (ssNegative); break;
	  case StepVisual_ssPositive : SW.SendEnum (ssPositive); break;
	  case StepVisual_ssBoth : SW.SendEnum (ssBoth); break;
	}

	// --- own field : style ---

	SW.Send(ent->Style());
}


void RWStepVisual_RWSurfaceStyleUsage::Share(const Handle(StepVisual_SurfaceStyleUsage)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Style());
}

