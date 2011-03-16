#include <RWStepShape_RWToleranceValue.ixx>
#include <StepBasic_MeasureWithUnit.hxx>


RWStepShape_RWToleranceValue::RWStepShape_RWToleranceValue () {}

void RWStepShape_RWToleranceValue::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_ToleranceValue)& ent) const
{
	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"tolerance_value")) return;

	// --- own field : lower_bound ---

	Handle(StepBasic_MeasureWithUnit) LB;
	data->ReadEntity (num,1,"lower_bound",ach,
			  STANDARD_TYPE(StepBasic_MeasureWithUnit),LB);

	// --- own field : upper_bound ---

	Handle(StepBasic_MeasureWithUnit) UB;
	data->ReadEntity (num,2,"upper_bound",ach,
			  STANDARD_TYPE(StepBasic_MeasureWithUnit),UB);

	//--- Initialisation of the read entity ---

	ent->Init(LB,UB);
}


void RWStepShape_RWToleranceValue::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_ToleranceValue)& ent) const
{
  SW.Send (ent->LowerBound());
  SW.Send (ent->UpperBound());
}


void RWStepShape_RWToleranceValue::Share(const Handle(StepShape_ToleranceValue)& ent, Interface_EntityIterator& iter) const
{
  iter.AddItem (ent->LowerBound());
  iter.AddItem (ent->UpperBound());
}

