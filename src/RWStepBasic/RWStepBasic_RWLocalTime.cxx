
#include <RWStepBasic_RWLocalTime.ixx>
#include <StepBasic_CoordinatedUniversalTimeOffset.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_LocalTime.hxx>


RWStepBasic_RWLocalTime::RWStepBasic_RWLocalTime () {}

void RWStepBasic_RWLocalTime::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_LocalTime)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"local_time")) return;

	// --- own field : hourComponent ---

	Standard_Integer aHourComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"hour_component",ach,aHourComponent);

	// --- own field : minuteComponent ---

	Standard_Integer aMinuteComponent;
	Standard_Boolean hasAminuteComponent = Standard_True;
	if (data->IsParamDefined(num,2)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadInteger (num,2,"minute_component",ach,aMinuteComponent);
	}
	else {
	  hasAminuteComponent = Standard_False;
	  aMinuteComponent = 0;
	}

	// --- own field : secondComponent ---

	Standard_Real aSecondComponent;
	Standard_Boolean hasAsecondComponent = Standard_True;
	if (data->IsParamDefined(num,3)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	  data->ReadReal (num,3,"second_component",ach,aSecondComponent);
	}
	else {
	  hasAsecondComponent = Standard_False;
	  aSecondComponent = 0.;
	}

	// --- own field : zone ---

	Handle(StepBasic_CoordinatedUniversalTimeOffset) aZone;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num, 4,"zone", ach, STANDARD_TYPE(StepBasic_CoordinatedUniversalTimeOffset), aZone);

	//--- Initialisation of the read entity ---


	ent->Init(aHourComponent, hasAminuteComponent, aMinuteComponent, hasAsecondComponent, aSecondComponent, aZone);
}


void RWStepBasic_RWLocalTime::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_LocalTime)& ent) const
{

	// --- own field : hourComponent ---

	SW.Send(ent->HourComponent());

	// --- own field : minuteComponent ---

	Standard_Boolean hasAminuteComponent = ent->HasMinuteComponent();
	if (hasAminuteComponent) {
	  SW.Send(ent->MinuteComponent());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : secondComponent ---

	Standard_Boolean hasAsecondComponent = ent->HasSecondComponent();
	if (hasAsecondComponent) {
	  SW.Send(ent->SecondComponent());
	}
	else {
	  SW.SendUndef();
	}

	// --- own field : zone ---

	SW.Send(ent->Zone());
}


void RWStepBasic_RWLocalTime::Share(const Handle(StepBasic_LocalTime)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Zone());
}

