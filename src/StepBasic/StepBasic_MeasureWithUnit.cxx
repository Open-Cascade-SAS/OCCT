#include <StepBasic_MeasureWithUnit.ixx>


//=======================================================================
//function : StepBasic_MeasureWithUnit
//purpose  : 
//=======================================================================

StepBasic_MeasureWithUnit::StepBasic_MeasureWithUnit ()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_MeasureWithUnit::Init(
	const Handle(StepBasic_MeasureValueMember)& aValueComponent,
	const StepBasic_Unit &aUnitComponent)
{
	// --- classe own fields ---
	valueComponent = aValueComponent;
	unitComponent = aUnitComponent;
}


//=======================================================================
//function : SetValueComponent
//purpose  : 
//=======================================================================

void StepBasic_MeasureWithUnit::SetValueComponent(const Standard_Real aValueComponent)
{
  if (valueComponent.IsNull()) valueComponent = new StepBasic_MeasureValueMember;
  valueComponent->SetReal(aValueComponent);
}

//=======================================================================
//function : ValueComponent
//purpose  : 
//=======================================================================

Standard_Real StepBasic_MeasureWithUnit::ValueComponent() const
{
  return (valueComponent.IsNull() ? 0.0 : valueComponent->Real());
}

//=======================================================================
//function : ValueComponentMember
//purpose  : 
//=======================================================================

Handle(StepBasic_MeasureValueMember)  StepBasic_MeasureWithUnit::ValueComponentMember () const
{  return valueComponent;  }

//=======================================================================
//function : SetValueComponentMember
//purpose  : 
//=======================================================================

void  StepBasic_MeasureWithUnit::SetValueComponentMember (const Handle(StepBasic_MeasureValueMember)& val)
{  valueComponent = val;  }

//=======================================================================
//function : SetUnitComponent
//purpose  : 
//=======================================================================

void StepBasic_MeasureWithUnit::SetUnitComponent(const StepBasic_Unit& aUnitComponent)
{
	unitComponent = aUnitComponent;
}

//=======================================================================
//function : UnitComponent
//purpose  : 
//=======================================================================

StepBasic_Unit StepBasic_MeasureWithUnit::UnitComponent() const
{
	return unitComponent;
}
