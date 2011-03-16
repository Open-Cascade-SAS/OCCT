
#include <StepVisual_CurveStyleFontSelect.ixx>
#include <Interface_Macros.hxx>

StepVisual_CurveStyleFontSelect::StepVisual_CurveStyleFontSelect () {  }

Standard_Integer StepVisual_CurveStyleFontSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_CurveStyleFont))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PreDefinedCurveFont))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_ExternallyDefinedCurveFont))) return 3;
	return 0;
}

Handle(StepVisual_CurveStyleFont) StepVisual_CurveStyleFontSelect::CurveStyleFont () const
{
	return GetCasted(StepVisual_CurveStyleFont,Value());
}

Handle(StepVisual_PreDefinedCurveFont) StepVisual_CurveStyleFontSelect::PreDefinedCurveFont () const
{
	return GetCasted(StepVisual_PreDefinedCurveFont,Value());
}

Handle(StepVisual_ExternallyDefinedCurveFont) StepVisual_CurveStyleFontSelect::ExternallyDefinedCurveFont () const
{
	return GetCasted(StepVisual_ExternallyDefinedCurveFont,Value());
}
