
#include <StepShape_CsgSelect.ixx>
#include <Interface_Macros.hxx>

StepShape_CsgSelect::StepShape_CsgSelect () {  }

void StepShape_CsgSelect::SetTypeOfContent(const Standard_Integer aType) 
{
  theTypeOfContent = aType;
}

Standard_Integer StepShape_CsgSelect::TypeOfContent() const 
{
  return theTypeOfContent;
}

Handle(StepShape_BooleanResult) StepShape_CsgSelect::BooleanResult () const
{
	return theBooleanResult;
}

void StepShape_CsgSelect::SetBooleanResult(const Handle(StepShape_BooleanResult)& aBooleanResult)
{
  theBooleanResult = aBooleanResult;
  theTypeOfContent = 1;
}

StepShape_CsgPrimitive StepShape_CsgSelect::CsgPrimitive () const
{
	return theCsgPrimitive;
}

void StepShape_CsgSelect::SetCsgPrimitive (const StepShape_CsgPrimitive& aCsgPrimitive)
{
  theCsgPrimitive  = aCsgPrimitive;
  theTypeOfContent = 2;
}
