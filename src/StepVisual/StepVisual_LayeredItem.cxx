
#include <StepVisual_LayeredItem.ixx>
#include <Interface_Macros.hxx>

StepVisual_LayeredItem::StepVisual_LayeredItem () {  }

Standard_Integer StepVisual_LayeredItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationRepresentation))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationItem))) return 2;
	return 0;
}

Handle(StepVisual_PresentationRepresentation) StepVisual_LayeredItem::PresentationRepresentation () const
{
	return GetCasted(StepVisual_PresentationRepresentation,Value());
}

Handle(StepRepr_RepresentationItem) StepVisual_LayeredItem::RepresentationItem () const
{
	return GetCasted(StepRepr_RepresentationItem,Value());
}
