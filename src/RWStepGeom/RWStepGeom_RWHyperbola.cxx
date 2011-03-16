
#include <RWStepGeom_RWHyperbola.ixx>
#include <StepGeom_Axis2Placement.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_Hyperbola.hxx>


RWStepGeom_RWHyperbola::RWStepGeom_RWHyperbola () {}

void RWStepGeom_RWHyperbola::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_Hyperbola)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"hyperbola")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : position ---

	StepGeom_Axis2Placement aPosition;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num,2,"position",ach,aPosition);

	// --- own field : semiAxis ---

	Standard_Real aSemiAxis;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"semi_axis",ach,aSemiAxis);

	// --- own field : semiImagAxis ---

	Standard_Real aSemiImagAxis;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"semi_imag_axis",ach,aSemiImagAxis);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPosition, aSemiAxis, aSemiImagAxis);
}


void RWStepGeom_RWHyperbola::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_Hyperbola)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field position ---

	SW.Send(ent->Position().Value());

	// --- own field : semiAxis ---

	SW.Send(ent->SemiAxis());

	// --- own field : semiImagAxis ---

	SW.Send(ent->SemiImagAxis());
}


void RWStepGeom_RWHyperbola::Share(const Handle(StepGeom_Hyperbola)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Position().Value());
}

