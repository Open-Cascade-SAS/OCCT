
#include <RWStepGeom_RWSweptSurface.ixx>
#include <StepGeom_Curve.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_SweptSurface.hxx>


RWStepGeom_RWSweptSurface::RWStepGeom_RWSweptSurface () {}

void RWStepGeom_RWSweptSurface::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_SweptSurface)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"swept_surface")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : sweptCurve ---

	Handle(StepGeom_Curve) aSweptCurve;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"swept_curve", ach, STANDARD_TYPE(StepGeom_Curve), aSweptCurve);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aSweptCurve);
}


void RWStepGeom_RWSweptSurface::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_SweptSurface)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : sweptCurve ---

	SW.Send(ent->SweptCurve());
}


void RWStepGeom_RWSweptSurface::Share(const Handle(StepGeom_SweptSurface)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->SweptCurve());
}

