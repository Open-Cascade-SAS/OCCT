
#include <StepShape_GeometricSetSelect.ixx>
#include <Interface_Macros.hxx>

StepShape_GeometricSetSelect::StepShape_GeometricSetSelect () {  }

Standard_Integer StepShape_GeometricSetSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Point))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Curve))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Surface))) return 3;
	return 0;
}

Handle(StepGeom_Point) StepShape_GeometricSetSelect::Point () const
{
	return GetCasted(StepGeom_Point,Value());
}

Handle(StepGeom_Curve) StepShape_GeometricSetSelect::Curve () const
{
	return GetCasted(StepGeom_Curve,Value());
}

Handle(StepGeom_Surface) StepShape_GeometricSetSelect::Surface () const
{
	return GetCasted(StepGeom_Surface,Value());
}
