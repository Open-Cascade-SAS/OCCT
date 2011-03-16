
#include <RWStepShape_RWManifoldSolidBrep.ixx>
#include <StepShape_ClosedShell.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_ManifoldSolidBrep.hxx>


RWStepShape_RWManifoldSolidBrep::RWStepShape_RWManifoldSolidBrep () {}

void RWStepShape_RWManifoldSolidBrep::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_ManifoldSolidBrep)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"manifold_solid_brep")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : outer ---

	Handle(StepShape_ClosedShell) aOuter;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"outer", ach, STANDARD_TYPE(StepShape_ClosedShell), aOuter);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aOuter);
}


void RWStepShape_RWManifoldSolidBrep::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_ManifoldSolidBrep)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : outer ---

	SW.Send(ent->Outer());
}


void RWStepShape_RWManifoldSolidBrep::Share(const Handle(StepShape_ManifoldSolidBrep)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Outer());
}

