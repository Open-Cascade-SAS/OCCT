
#include <RWStepBasic_RWConversionBasedUnit.ixx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_ConversionBasedUnit.hxx>


RWStepBasic_RWConversionBasedUnit::RWStepBasic_RWConversionBasedUnit () {}

void RWStepBasic_RWConversionBasedUnit::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ConversionBasedUnit)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"conversion_based_unit")) return;

	// --- inherited field : dimensions ---

	Handle(StepBasic_DimensionalExponents) aDimensions;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aDimensions);

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"name",ach,aName);

	// --- own field : conversionFactor ---

	Handle(StepBasic_MeasureWithUnit) aConversionFactor;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"conversion_factor", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aConversionFactor);

	//--- Initialisation of the read entity ---


	ent->Init(aDimensions, aName, aConversionFactor);
}


void RWStepBasic_RWConversionBasedUnit::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ConversionBasedUnit)& ent) const
{

	// --- inherited field dimensions ---

	SW.Send(ent->Dimensions());

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : conversionFactor ---

	SW.Send(ent->ConversionFactor());
}


void RWStepBasic_RWConversionBasedUnit::Share(const Handle(StepBasic_ConversionBasedUnit)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Dimensions());


	iter.GetOneItem(ent->ConversionFactor());
}

