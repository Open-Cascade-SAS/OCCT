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
#include <RWStepGeom_RWCompositeCurveSegment.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_TransitionCode.hxx>
#include <TCollection_AsciiString.hxx>

// --- Enum : TransitionCode ---
static TCollection_AsciiString tcDiscontinuous(".DISCONTINUOUS.");
static TCollection_AsciiString tcContSameGradientSameCurvature(".CONT_SAME_GRADIENT_SAME_CURVATURE.");
static TCollection_AsciiString tcContSameGradient(".CONT_SAME_GRADIENT.");
static TCollection_AsciiString tcContinuous(".CONTINUOUS.");

RWStepGeom_RWCompositeCurveSegment::RWStepGeom_RWCompositeCurveSegment () {}

void RWStepGeom_RWCompositeCurveSegment::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_CompositeCurveSegment)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"composite_curve_segment")) return;

	// --- own field : transition ---

	StepGeom_TransitionCode aTransition = StepGeom_tcDiscontinuous;
	if (data->ParamType(num,1) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,1);
	  if      (tcDiscontinuous.IsEqual(text)) aTransition = StepGeom_tcDiscontinuous;
	  else if (tcContSameGradientSameCurvature.IsEqual(text)) aTransition = StepGeom_tcContSameGradientSameCurvature;
	  else if (tcContSameGradient.IsEqual(text)) aTransition = StepGeom_tcContSameGradient;
	  else if (tcContinuous.IsEqual(text)) aTransition = StepGeom_tcContinuous;
	  else ach->AddFail("Enumeration transition_code has not an allowed value");
	}
	else ach->AddFail("Parameter #1 (transition) is not an enumeration");

	// --- own field : sameSense ---

	Standard_Boolean aSameSense;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadBoolean (num,2,"same_sense",ach,aSameSense);

	// --- own field : parentCurve ---

	Handle(StepGeom_Curve) aParentCurve;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"parent_curve", ach, STANDARD_TYPE(StepGeom_Curve), aParentCurve);

	//--- Initialisation of the read entity ---


	ent->Init(aTransition, aSameSense, aParentCurve);
}


void RWStepGeom_RWCompositeCurveSegment::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_CompositeCurveSegment)& ent) const
{

	// --- own field : transition ---

	switch(ent->Transition()) {
	  case StepGeom_tcDiscontinuous : SW.SendEnum (tcDiscontinuous); break;
	  case StepGeom_tcContSameGradientSameCurvature : SW.SendEnum (tcContSameGradientSameCurvature); break;
	  case StepGeom_tcContSameGradient : SW.SendEnum (tcContSameGradient); break;
	  case StepGeom_tcContinuous : SW.SendEnum (tcContinuous); break;
	}

	// --- own field : sameSense ---

	SW.SendBoolean(ent->SameSense());

	// --- own field : parentCurve ---

	SW.Send(ent->ParentCurve());
}


void RWStepGeom_RWCompositeCurveSegment::Share(const Handle(StepGeom_CompositeCurveSegment)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->ParentCurve());
}

