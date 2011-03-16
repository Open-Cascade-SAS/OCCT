#include <StepBasic_UncertaintyMeasureWithUnit.ixx>


//=======================================================================
//function : StepBasic_UncertaintyMeasureWithUnit
//purpose  : 
//=======================================================================

StepBasic_UncertaintyMeasureWithUnit::StepBasic_UncertaintyMeasureWithUnit ()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_UncertaintyMeasureWithUnit::Init(
	const Handle(StepBasic_MeasureValueMember)& aValueComponent,
	const StepBasic_Unit& aUnitComponent)
{

	StepBasic_MeasureWithUnit::Init(aValueComponent, aUnitComponent);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_UncertaintyMeasureWithUnit::Init(
	const Handle(StepBasic_MeasureValueMember)& aValueComponent,
	const StepBasic_Unit& aUnitComponent,
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription)
{
	// --- classe own fields ---
	name = aName;
	description = aDescription;
	// --- classe inherited fields ---
	StepBasic_MeasureWithUnit::Init(aValueComponent, aUnitComponent);
}


//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_UncertaintyMeasureWithUnit::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_UncertaintyMeasureWithUnit::Name() const
{
	return name;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_UncertaintyMeasureWithUnit::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_UncertaintyMeasureWithUnit::Description() const
{
	return description;
}
