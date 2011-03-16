#include <StepBasic_Organization.ixx>


StepBasic_Organization::StepBasic_Organization ()  {}

void StepBasic_Organization::Init(
	const Standard_Boolean hasAid,
	const Handle(TCollection_HAsciiString)& aId,
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription)
{
	// --- classe own fields ---
	hasId = hasAid;
	id = aId;
	name = aName;
	description = aDescription;
}


void StepBasic_Organization::SetId(const Handle(TCollection_HAsciiString)& aId)
{
	id = aId;
	hasId = Standard_True;
}

void StepBasic_Organization::UnSetId()
{
	hasId = Standard_False;
	id.Nullify();
}

Handle(TCollection_HAsciiString) StepBasic_Organization::Id() const
{
	return id;
}

Standard_Boolean StepBasic_Organization::HasId() const
{
	return hasId;
}

void StepBasic_Organization::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_Organization::Name() const
{
	return name;
}

void StepBasic_Organization::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepBasic_Organization::Description() const
{
	return description;
}
