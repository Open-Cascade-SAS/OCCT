
#include <RWStepBasic_RWPlaneAngleMeasureWithUnit.ixx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_MeasureValueMember.hxx>

#include <TCollection_AsciiString.hxx>

#include <Interface_EntityIterator.hxx>


#include <StepBasic_PlaneAngleMeasureWithUnit.hxx>

//=======================================================================
//function : RWStepBasic_RWPlaneAngleMeasureWithUnit
//purpose  : 
//=======================================================================

RWStepBasic_RWPlaneAngleMeasureWithUnit::RWStepBasic_RWPlaneAngleMeasureWithUnit () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWPlaneAngleMeasureWithUnit::ReadStep (const Handle(StepData_StepReaderData)& data,
							const Standard_Integer num,
							Handle(Interface_Check)& ach,
							const Handle(StepBasic_PlaneAngleMeasureWithUnit)& ent) const
{
  // --- Number of Parameter Control ---
  if (!data->CheckNbParams(num,2,ach,"plane_angle_measure_with_unit")) return;
  
  // --- inherited field : valueComponent ---
  // UPDATE 21-02-96 ,  31-MARS-1997 by CKY
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

void RWStepBasic_RWPlaneAngleMeasureWithUnit::WriteStep	(StepData_StepWriter& SW,
							 const Handle(StepBasic_PlaneAngleMeasureWithUnit)& ent) const
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

void RWStepBasic_RWPlaneAngleMeasureWithUnit::Share (const Handle(StepBasic_PlaneAngleMeasureWithUnit)& ent, 
						     Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->UnitComponent().Value());
}

