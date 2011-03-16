
#include <RWStepGeom_RWElementarySurface.ixx>
#include <StepGeom_Axis2Placement3d.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_ElementarySurface.hxx>


RWStepGeom_RWElementarySurface::RWStepGeom_RWElementarySurface () {}

void RWStepGeom_RWElementarySurface::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_ElementarySurface)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"elementary_surface")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : position ---

	Handle(StepGeom_Axis2Placement3d) aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"position", ach, STANDARD_TYPE(StepGeom_Axis2Placement3d), aPosition);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition);
}


void RWStepGeom_RWElementarySurface::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_ElementarySurface)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : position ---

	SW.Send(ent->Position());
}


void RWStepGeom_RWElementarySurface::Share(const Handle(StepGeom_ElementarySurface)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position());
}

