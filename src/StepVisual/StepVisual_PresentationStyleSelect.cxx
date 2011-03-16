
#include <StepVisual_PresentationStyleSelect.ixx>
#include <Interface_Macros.hxx>

StepVisual_PresentationStyleSelect::StepVisual_PresentationStyleSelect () {  }

Standard_Integer StepVisual_PresentationStyleSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PointStyle))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_CurveStyle))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_SurfaceStyleUsage))) return 3;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_SymbolStyle))) return 4;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_FillAreaStyle))) return 5;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_TextStyle))) return 6;
	return 0;
}

Handle(StepVisual_PointStyle) StepVisual_PresentationStyleSelect::PointStyle () const
{
	return GetCasted(StepVisual_PointStyle,Value());
}

Handle(StepVisual_CurveStyle) StepVisual_PresentationStyleSelect::CurveStyle () const
{
	return GetCasted(StepVisual_CurveStyle,Value());
}

Handle(StepVisual_SurfaceStyleUsage) StepVisual_PresentationStyleSelect::SurfaceStyleUsage () const
{
	return GetCasted(StepVisual_SurfaceStyleUsage,Value());
}
