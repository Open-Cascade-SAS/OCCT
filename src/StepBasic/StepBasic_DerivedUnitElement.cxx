#include <StepBasic_DerivedUnitElement.ixx>

StepBasic_DerivedUnitElement::StepBasic_DerivedUnitElement ()    {  }

void  StepBasic_DerivedUnitElement::Init (const Handle(StepBasic_NamedUnit)& aUnit, const Standard_Real aExponent)
{  theUnit = aUnit;  theExponent = aExponent;  }

void  StepBasic_DerivedUnitElement::SetUnit (const Handle(StepBasic_NamedUnit)& aUnit)
{  theUnit = aUnit;  }

Handle(StepBasic_NamedUnit)  StepBasic_DerivedUnitElement::Unit () const
{  return theUnit;  }

void  StepBasic_DerivedUnitElement::SetExponent (const Standard_Real aExponent)
{  theExponent = aExponent;  }

Standard_Real  StepBasic_DerivedUnitElement::Exponent () const
{  return theExponent;  }
