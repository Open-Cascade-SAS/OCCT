
#include <RWStepShape_RWHalfSpaceSolid.ixx>
#include <StepGeom_Surface.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepShape_HalfSpaceSolid.hxx>


RWStepShape_RWHalfSpaceSolid::RWStepShape_RWHalfSpaceSolid () {}

void RWStepShape_RWHalfSpaceSolid::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_HalfSpaceSolid)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"half_space_solid")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : baseSurface ---

	Handle(StepGeom_Surface) aBaseSurface;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"base_surface", ach, STANDARD_TYPE(StepGeom_Surface), aBaseSurface);

	// --- own field : agreementFlag ---

	Standard_Boolean aAgreementFlag;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadBoolean (num,3,"agreement_flag",ach,aAgreementFlag);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aBaseSurface, aAgreementFlag);
}


void RWStepShape_RWHalfSpaceSolid::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_HalfSpaceSolid)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : baseSurface ---

	SW.Send(ent->BaseSurface());

	// --- own field : agreementFlag ---

	SW.SendBoolean(ent->AgreementFlag());
}


void RWStepShape_RWHalfSpaceSolid::Share(const Handle(StepShape_HalfSpaceSolid)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->BaseSurface());
}

