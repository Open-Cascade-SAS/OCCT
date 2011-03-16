#include <StepShape_ToleranceMethodDefinition.ixx>

StepShape_ToleranceMethodDefinition::StepShape_ToleranceMethodDefinition  ()    {  }

Standard_Integer  StepShape_ToleranceMethodDefinition::CaseNum
  (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepShape_ToleranceValue))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_LimitsAndFits))) return 2;
  return 0;
}

Handle(StepShape_ToleranceValue)  StepShape_ToleranceMethodDefinition::ToleranceValue () const
{  return Handle(StepShape_ToleranceValue)::DownCast(Value());  }

Handle(StepShape_LimitsAndFits)  StepShape_ToleranceMethodDefinition::LimitsAndFits () const
{  return Handle(StepShape_LimitsAndFits)::DownCast(Value());  }
