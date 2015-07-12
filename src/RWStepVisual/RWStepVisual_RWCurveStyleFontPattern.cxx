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
#include <RWStepVisual_RWCurveStyleFontPattern.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_CurveStyleFontPattern.hxx>

RWStepVisual_RWCurveStyleFontPattern::RWStepVisual_RWCurveStyleFontPattern () {}

void RWStepVisual_RWCurveStyleFontPattern::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_CurveStyleFontPattern)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"curve_style_font_pattern")) return;

	// --- own field : visibleSegmentLength ---

	Standard_Real aVisibleSegmentLength;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadReal (num,1,"visible_segment_length",ach,aVisibleSegmentLength);

	// --- own field : invisibleSegmentLength ---

	Standard_Real aInvisibleSegmentLength;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadReal (num,2,"invisible_segment_length",ach,aInvisibleSegmentLength);

	//--- Initialisation of the read entity ---


	ent->Init(aVisibleSegmentLength, aInvisibleSegmentLength);
}


void RWStepVisual_RWCurveStyleFontPattern::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_CurveStyleFontPattern)& ent) const
{

	// --- own field : visibleSegmentLength ---

	SW.Send(ent->VisibleSegmentLength());

	// --- own field : invisibleSegmentLength ---

	SW.Send(ent->InvisibleSegmentLength());
}
