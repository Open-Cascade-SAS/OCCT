
#include <RWStepGeom_RWPointOnCurve.ixx>
#include <StepGeom_Curve.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_PointOnCurve.hxx>


RWStepGeom_RWPointOnCurve::RWStepGeom_RWPointOnCurve () {}

void RWStepGeom_RWPointOnCurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_PointOnCurve)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"point_on_curve")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : basisCurve ---

	Handle(StepGeom_Curve) aBasisCurve;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"basis_curve", ach, STANDARD_TYPE(StepGeom_Curve), aBasisCurve);

	// --- own field : pointParameter ---

	Standard_Real aPointParameter;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"point_parameter",ach,aPointParameter);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aBasisCurve, aPointParameter);
}


void RWStepGeom_RWPointOnCurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_PointOnCurve)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : basisCurve ---

	SW.Send(ent->BasisCurve());

	// --- own field : pointParameter ---

	SW.Send(ent->PointParameter());
}


void RWStepGeom_RWPointOnCurve::Share(const Handle(StepGeom_PointOnCurve)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->BasisCurve());
}

