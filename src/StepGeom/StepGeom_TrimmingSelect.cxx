
#include <StepGeom_TrimmingSelect.ixx>
#include <StepGeom_TrimmingMember.hxx>
#include <Interface_Macros.hxx>

StepGeom_TrimmingSelect::StepGeom_TrimmingSelect () {  }

Standard_Integer StepGeom_TrimmingSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_CartesianPoint))) return 1;
  return 0;
}

Handle(StepData_SelectMember)  StepGeom_TrimmingSelect::NewMember () const
{  return  new StepGeom_TrimmingMember;  }

Standard_Integer  StepGeom_TrimmingSelect::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
  if (ent.IsNull()) return 0;
  Interface_ParamType type = ent->ParamType();
//  Void : on admet "non defini" (en principe, on ne devrait pas)
  if (type != Interface_ParamVoid && type != Interface_ParamReal) return 0;
  if (ent->Matches("PARAMETER_VALUE")) return 1;
  return 0;
}

Handle(StepGeom_CartesianPoint) StepGeom_TrimmingSelect::CartesianPoint () const
{
  return GetCasted(StepGeom_CartesianPoint,Value());
}

void StepGeom_TrimmingSelect::SetParameterValue(const Standard_Real aParameterValue)
{  SetReal (aParameterValue,"PARAMETER_VALUE");  }

Standard_Real StepGeom_TrimmingSelect::ParameterValue () const
{  return Real();  }
