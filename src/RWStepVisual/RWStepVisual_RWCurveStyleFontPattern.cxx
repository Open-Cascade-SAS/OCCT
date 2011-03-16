
#include <RWStepVisual_RWCurveStyleFontPattern.ixx>


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
