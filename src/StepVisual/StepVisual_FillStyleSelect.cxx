
#include <StepVisual_FillStyleSelect.ixx>
#include <Interface_Macros.hxx>

StepVisual_FillStyleSelect::StepVisual_FillStyleSelect () {  }

Standard_Integer StepVisual_FillStyleSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_FillAreaStyleColour))) return 1;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_ExternallyDefinedTileStyle))) return 2;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_FillAreaStyleTiles))) return 3;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_ExternallyDefinedHatchStyle))) return 4;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_FillAreaStyleHatching))) return 5;
	return 0;
}

Handle(StepVisual_FillAreaStyleColour) StepVisual_FillStyleSelect::FillAreaStyleColour () const
{
	return GetCasted(StepVisual_FillAreaStyleColour,Value());
}
