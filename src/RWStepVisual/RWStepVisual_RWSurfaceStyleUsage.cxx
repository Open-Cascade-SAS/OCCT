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


#include <RWStepVisual_RWSurfaceStyleUsage.ixx>
#include <StepVisual_SurfaceSide.hxx>
#include <StepVisual_SurfaceSideStyle.hxx>


#include <Interface_EntityIterator.hxx>


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

