#include <StepBasic_ProductDefinitionEffectivity.ixx>

StepBasic_ProductDefinitionEffectivity::StepBasic_ProductDefinitionEffectivity  ()    {  }

void  StepBasic_ProductDefinitionEffectivity::Init
  (const Handle(TCollection_HAsciiString)& aId,
   const Handle(StepBasic_ProductDefinitionRelationship)& aUsage)
{
  SetId (aId);
  theUsage = aUsage;
}

Handle(StepBasic_ProductDefinitionRelationship)  StepBasic_ProductDefinitionEffectivity::Usage () const
{  return theUsage;  }

void  StepBasic_ProductDefinitionEffectivity::SetUsage (const Handle(StepBasic_ProductDefinitionRelationship)& aUsage)
{  theUsage = aUsage;  }
