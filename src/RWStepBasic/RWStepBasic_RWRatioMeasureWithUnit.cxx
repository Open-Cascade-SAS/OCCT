
#include <RWStepBasic_RWRatioMeasureWithUnit.ixx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_RatioMeasureWithUnit.hxx>

//=======================================================================
//function : RWStepBasic_RWRatioMeasureWithUnit
//purpose  : 
//=======================================================================

RWStepBasic_RWRatioMeasureWithUnit::RWStepBasic_RWRatioMeasureWithUnit () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWRatioMeasureWithUnit::ReadStep (const Handle(StepData_StepReaderData)& data,
						   const Standard_Integer num,
						   Handle(Interface_Check)& ach,
						   const Handle(StepBasic_RatioMeasureWithUnit)& ent) const
{
  // --- Number of Parameter Control ---
  if (!data->CheckNbParams(num,2,ach,"ratio_measure_with_unit")) return;

  // --- inherited field : valueComponent ---
  Handle(StepBasic_MeasureValueMember) mvc = new StepBasic_MeasureValueMember;
  data->ReadMember (num,1, "value_component", ach, mvc);

  // --- inherited field : unitComponent ---
  StepBasic_Unit aUnitComponent;
  data->ReadEntity(num, 2,"unit_component", ach, aUnitComponent);

  //--- Initialisation of the read entity ---
  ent->Init(mvc, aUnitComponent);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWRatioMeasureWithUnit::WriteStep (StepData_StepWriter& SW,
						    const Handle(StepBasic_RatioMeasureWithUnit)& ent) const
{
  // --- inherited field valueComponent ---
  SW.Send(ent->ValueComponentMember());

  // --- inherited field unitComponent ---
  SW.Send(ent->UnitComponent().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWRatioMeasureWithUnit::Share (const Handle(StepBasic_RatioMeasureWithUnit)& ent, 
						Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->UnitComponent().Value());
}

