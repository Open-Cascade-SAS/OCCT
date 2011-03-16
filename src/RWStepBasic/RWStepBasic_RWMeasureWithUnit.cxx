
#include <RWStepBasic_RWMeasureWithUnit.ixx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_MeasureWithUnit.hxx>


//=======================================================================
//function : RWStepBasic_RWMeasureWithUnit
//purpose  : 
//=======================================================================

RWStepBasic_RWMeasureWithUnit::RWStepBasic_RWMeasureWithUnit () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWMeasureWithUnit::ReadStep (const Handle(StepData_StepReaderData)& data,
					      const Standard_Integer num,
					      Handle(Interface_Check)& ach,
					      const Handle(StepBasic_MeasureWithUnit)& ent) const
{
  // --- Number of Parameter Control ---
  if (!data->CheckNbParams(num,2,ach,"measure_with_unit")) return;

  // --- own field : valueComponent ---
  Handle(StepBasic_MeasureValueMember) mvc = new StepBasic_MeasureValueMember;
  data->ReadMember (num,1, "value_component", ach, mvc);

  // --- own field : unitComponent ---
  StepBasic_Unit aUnitComponent;
  data->ReadEntity(num, 2,"unit_component", ach, aUnitComponent);

  //--- Initialisation of the read entity ---
  ent->Init(mvc, aUnitComponent);
}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWMeasureWithUnit::WriteStep (StepData_StepWriter& SW,
					       const Handle(StepBasic_MeasureWithUnit)& ent) const
{
  // --- own field : valueComponent ---
  SW.Send(ent->ValueComponentMember());

  // --- own field : unitComponent ---
  SW.Send(ent->UnitComponent().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWMeasureWithUnit::Share (const Handle(StepBasic_MeasureWithUnit)& ent, 
					   Interface_EntityIterator& iter) const
{

  iter.AddItem(ent->UnitComponent().Value());
}

