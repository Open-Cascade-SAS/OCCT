
#include <StepGeom_VectorOrDirection.ixx>
#include <Interface_Macros.hxx>

StepGeom_VectorOrDirection::StepGeom_VectorOrDirection () {  }

Standard_Integer StepGeom_VectorOrDirection::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Vector))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Direction))) return 2;
	return 0;
}

Handle(StepGeom_Vector) StepGeom_VectorOrDirection::Vector () const
{
	return GetCasted(StepGeom_Vector,Value());
}

Handle(StepGeom_Direction) StepGeom_VectorOrDirection::Direction () const
{
	return GetCasted(StepGeom_Direction,Value());
}
