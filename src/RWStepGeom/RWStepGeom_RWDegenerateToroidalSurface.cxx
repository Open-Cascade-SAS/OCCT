
#include <RWStepGeom_RWDegenerateToroidalSurface.ixx>
#include <StepGeom_Axis2Placement3d.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_DegenerateToroidalSurface.hxx>


RWStepGeom_RWDegenerateToroidalSurface::RWStepGeom_RWDegenerateToroidalSurface () {}

void RWStepGeom_RWDegenerateToroidalSurface::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_DegenerateToroidalSurface)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,5,ach,"degenerate_toroidal_surface")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : position ---

	Handle(StepGeom_Axis2Placement3d) aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"position", ach, STANDARD_TYPE(StepGeom_Axis2Placement3d), aPosition);

	// --- inherited field : majorRadius ---

	Standard_Real aMajorRadius;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"major_radius",ach,aMajorRadius);

	// --- inherited field : minorRadius ---

	Standard_Real aMinorRadius;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"minor_radius",ach,aMinorRadius);

	// --- own field : selectOuter ---

	Standard_Boolean aSelectOuter;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadBoolean (num,5,"select_outer",ach,aSelectOuter);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition, aMajorRadius, aMinorRadius, aSelectOuter);
}


void RWStepGeom_RWDegenerateToroidalSurface::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_DegenerateToroidalSurface)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field position ---

	SW.Send(ent->Position());

	// --- inherited field majorRadius ---

	SW.Send(ent->MajorRadius());

	// --- inherited field minorRadius ---

	SW.Send(ent->MinorRadius());

	// --- own field : selectOuter ---

	SW.SendBoolean(ent->SelectOuter());
}


void RWStepGeom_RWDegenerateToroidalSurface::Share(const Handle(StepGeom_DegenerateToroidalSurface)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position());
}

