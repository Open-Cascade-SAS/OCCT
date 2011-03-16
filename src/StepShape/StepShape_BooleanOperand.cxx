
#include <StepShape_BooleanOperand.ixx>
#include <Interface_Macros.hxx>

StepShape_BooleanOperand::StepShape_BooleanOperand () {  }

Handle(StepShape_SolidModel) StepShape_BooleanOperand::SolidModel () const
{
  return theSolidModel;
}

void StepShape_BooleanOperand::SetSolidModel
(const Handle(StepShape_SolidModel)& aSolidModel) 
{
  theSolidModel = aSolidModel;
}

Handle(StepShape_HalfSpaceSolid) StepShape_BooleanOperand::HalfSpaceSolid () const
{
  return theHalfSpaceSolid;
}

void StepShape_BooleanOperand::SetHalfSpaceSolid
(const Handle(StepShape_HalfSpaceSolid)& aHalfSpaceSolid)
{
  theHalfSpaceSolid = aHalfSpaceSolid;
}

StepShape_CsgPrimitive StepShape_BooleanOperand::CsgPrimitive () const
{
  return theCsgPrimitive;
}

void StepShape_BooleanOperand::SetCsgPrimitive
(const StepShape_CsgPrimitive& aCsgPrimitive)
{
  theCsgPrimitive = aCsgPrimitive;
}

Handle(StepShape_BooleanResult) StepShape_BooleanOperand::BooleanResult () const
{
  return theBooleanResult;
}

void StepShape_BooleanOperand::SetBooleanResult
(const Handle(StepShape_BooleanResult)& aBooleanResult)
{
  theBooleanResult = aBooleanResult;
}

void StepShape_BooleanOperand::SetTypeOfContent(const Standard_Integer aType)
{
  theTypeOfContent = aType;
}

Standard_Integer StepShape_BooleanOperand::TypeOfContent() const
{
  return theTypeOfContent;
}
