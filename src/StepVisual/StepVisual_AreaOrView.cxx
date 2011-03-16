
#include <StepVisual_AreaOrView.ixx>
#include <Interface_Macros.hxx>

StepVisual_AreaOrView::StepVisual_AreaOrView () {  }

Standard_Integer StepVisual_AreaOrView::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationArea))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationView))) return 2;
	return 0;
}

Handle(StepVisual_PresentationArea) StepVisual_AreaOrView::PresentationArea () const
{
	return GetCasted(StepVisual_PresentationArea,Value());
}

Handle(StepVisual_PresentationView) StepVisual_AreaOrView::PresentationView () const
{
	return GetCasted(StepVisual_PresentationView,Value());
}
