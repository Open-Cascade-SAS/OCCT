
#include <RWStepVisual_RWPlanarBox.ixx>
#include <StepGeom_Axis2Placement.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_PlanarBox.hxx>


RWStepVisual_RWPlanarBox::RWStepVisual_RWPlanarBox () {}

void RWStepVisual_RWPlanarBox::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_PlanarBox)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"planar_box")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : sizeInX ---

	Standard_Real aSizeInX;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadReal (num,2,"size_in_x",ach,aSizeInX);

	// --- inherited field : sizeInY ---

	Standard_Real aSizeInY;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"size_in_y",ach,aSizeInY);

	// --- own field : placement ---

	StepGeom_Axis2Placement aPlacement;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num,4,"placement",ach,aPlacement);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aSizeInX, aSizeInY, aPlacement);
}


void RWStepVisual_RWPlanarBox::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_PlanarBox)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field sizeInX ---

	SW.Send(ent->SizeInX());

	// --- inherited field sizeInY ---

	SW.Send(ent->SizeInY());

	// --- own field : placement ---

	SW.Send(ent->Placement().Value());
}


void RWStepVisual_RWPlanarBox::Share(const Handle(StepVisual_PlanarBox)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Placement().Value());
}

