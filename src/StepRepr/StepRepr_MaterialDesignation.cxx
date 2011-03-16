#include <StepRepr_MaterialDesignation.ixx>

StepRepr_MaterialDesignation::StepRepr_MaterialDesignation  ()    {  }

void  StepRepr_MaterialDesignation::Init
(const Handle(TCollection_HAsciiString)& aName,
 const StepRepr_CharacterizedDefinition& aOfDefinition)
{
  name = aName;
  ofDefinition = aOfDefinition;
}

void  StepRepr_MaterialDesignation::SetName
  (const Handle(TCollection_HAsciiString)& aName)
{
  name = aName;
}

Handle(TCollection_HAsciiString)  StepRepr_MaterialDesignation::Name () const
{
  return name;
}

void  StepRepr_MaterialDesignation::SetOfDefinition
  (const StepRepr_CharacterizedDefinition& aOfDefinition)
{
  ofDefinition = aOfDefinition;
}

StepRepr_CharacterizedDefinition  StepRepr_MaterialDesignation::OfDefinition () const
{
  return ofDefinition;
}
