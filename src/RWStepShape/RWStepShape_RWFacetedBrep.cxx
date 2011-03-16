
#include <RWStepShape_RWFacetedBrep.ixx>
#include <StepShape_ClosedShell.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_FacetedBrep.hxx>


RWStepShape_RWFacetedBrep::RWStepShape_RWFacetedBrep () {}

void RWStepShape_RWFacetedBrep::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_FacetedBrep)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"faceted_brep")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : outer ---

	Handle(StepShape_ClosedShell) aOuter;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"outer", ach, STANDARD_TYPE(StepShape_ClosedShell), aOuter);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aOuter);
}


void RWStepShape_RWFacetedBrep::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_FacetedBrep)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field outer ---

	SW.Send(ent->Outer());
}


void RWStepShape_RWFacetedBrep::Share(const Handle(StepShape_FacetedBrep)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Outer());
}

