
#include <RWStepGeom_RWConicalSurface.ixx>
#include <StepGeom_Axis2Placement3d.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_ConicalSurface.hxx>


RWStepGeom_RWConicalSurface::RWStepGeom_RWConicalSurface () {}

void RWStepGeom_RWConicalSurface::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_ConicalSurface)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"conical_surface")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : position ---

	Handle(StepGeom_Axis2Placement3d) aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"position", ach, STANDARD_TYPE(StepGeom_Axis2Placement3d), aPosition);

	// --- own field : radius ---

	Standard_Real aRadius;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"radius",ach,aRadius);

	// --- own field : semiAngle ---

	Standard_Real aSemiAngle;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"semi_angle",ach,aSemiAngle);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition, aRadius, aSemiAngle);
}


void RWStepGeom_RWConicalSurface::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_ConicalSurface)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field position ---

	SW.Send(ent->Position());

	// --- own field : radius ---

	SW.Send(ent->Radius());

	// --- own field : semiAngle ---

	SW.Send(ent->SemiAngle());
}


void RWStepGeom_RWConicalSurface::Share(const Handle(StepGeom_ConicalSurface)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position());
}

