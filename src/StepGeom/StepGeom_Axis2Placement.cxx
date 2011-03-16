
#include <StepGeom_Axis2Placement.ixx>
#include <Interface_Macros.hxx>

StepGeom_Axis2Placement::StepGeom_Axis2Placement () {  }

Standard_Integer StepGeom_Axis2Placement::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement2d))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d))) return 2;
	return 0;
}

Handle(StepGeom_Axis2Placement2d) StepGeom_Axis2Placement::Axis2Placement2d () const
{
	return GetCasted(StepGeom_Axis2Placement2d,Value());
}

Handle(StepGeom_Axis2Placement3d) StepGeom_Axis2Placement::Axis2Placement3d () const
{
	return GetCasted(StepGeom_Axis2Placement3d,Value());
}
