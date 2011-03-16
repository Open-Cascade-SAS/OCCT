
#include <StepVisual_InvisibleItem.ixx>
#include <Interface_Macros.hxx>

StepVisual_InvisibleItem::StepVisual_InvisibleItem () {  }

Standard_Integer StepVisual_InvisibleItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_StyledItem))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationLayerAssignment))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_PresentationRepresentation))) return 3;
	return 0;
}

Handle(StepVisual_StyledItem) StepVisual_InvisibleItem::StyledItem () const
{
	return GetCasted(StepVisual_StyledItem,Value());
}

Handle(StepVisual_PresentationLayerAssignment) StepVisual_InvisibleItem::PresentationLayerAssignment () const
{
	return GetCasted(StepVisual_PresentationLayerAssignment,Value());
}

Handle(StepVisual_PresentationRepresentation) StepVisual_InvisibleItem::PresentationRepresentation () const
{
	return GetCasted(StepVisual_PresentationRepresentation,Value());
}
