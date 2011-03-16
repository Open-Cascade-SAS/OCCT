
#include <StepShape_SurfaceModel.ixx>
#include <Interface_Macros.hxx>

StepShape_SurfaceModel::StepShape_SurfaceModel () {  }

Standard_Integer StepShape_SurfaceModel::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel))) return 1;
//	if (ent->IsKind(STANDARD_TYPE(StepShape_FaceBasedSurfaceModel))) return 2;
	return 0;
}

Handle(StepShape_ShellBasedSurfaceModel) StepShape_SurfaceModel::ShellBasedSurfaceModel () const
{
	return GetCasted(StepShape_ShellBasedSurfaceModel,Value());
}
