#include <RWStepShape_RWPlusMinusTolerance.ixx>
#include <StepShape_ToleranceMethodDefinition.hxx>
#include <StepShape_DimensionalCharacteristic.hxx>


RWStepShape_RWPlusMinusTolerance::RWStepShape_RWPlusMinusTolerance () {}

void RWStepShape_RWPlusMinusTolerance::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_PlusMinusTolerance)& ent) const
{
	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"plus_minus_tolerance")) return;

	// --- own field : range ---

	StepShape_ToleranceMethodDefinition aRange;
	data->ReadEntity (num,1,"range",ach,aRange);

	// --- own field : tolerance_dimension ---

	StepShape_DimensionalCharacteristic aTD;
	data->ReadEntity (num,2,"2dtolerance_dimensionrange",ach,aTD);

	//--- Initialisation of the read entity ---

	ent->Init(aRange,aTD);
}


void RWStepShape_RWPlusMinusTolerance::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_PlusMinusTolerance)& ent) const
{
  SW.Send (ent->Range().Value());
  SW.Send (ent->TolerancedDimension().Value());
}


void RWStepShape_RWPlusMinusTolerance::Share(const Handle(StepShape_PlusMinusTolerance)& ent, Interface_EntityIterator& iter) const
{
  iter.AddItem (ent->Range().Value());
  iter.AddItem (ent->TolerancedDimension().Value());
}

