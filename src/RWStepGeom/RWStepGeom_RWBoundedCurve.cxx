
#include <RWStepGeom_RWBoundedCurve.ixx>


RWStepGeom_RWBoundedCurve::RWStepGeom_RWBoundedCurve () {}

void RWStepGeom_RWBoundedCurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_BoundedCurve)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"bounded_curve")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepGeom_RWBoundedCurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_BoundedCurve)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());
}
