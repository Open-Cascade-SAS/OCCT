// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//gka 05.03.99 S4134 upgrade from CD to DIS

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWShapeAspect.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_ShapeAspect.hxx>

RWStepRepr_RWShapeAspect::RWStepRepr_RWShapeAspect () {}

void RWStepRepr_RWShapeAspect::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_ShapeAspect)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"shape_aspect")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	if (data->IsParamDefined (num,2)) { //gka 05.03.99 S4134 upgrade from CD to DIS
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadString (num,2,"description",ach,aDescription);
	}
	// --- own field : ofShape ---

	Handle(StepRepr_ProductDefinitionShape) aOfShape;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"of_shape", ach, STANDARD_TYPE(StepRepr_ProductDefinitionShape), aOfShape);

	// --- own field : productDefinitional ---

	StepData_Logical aProductDefinitional;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadLogical (num,4,"product_definitional",ach,aProductDefinitional);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aDescription, aOfShape, aProductDefinitional);
}


void RWStepRepr_RWShapeAspect::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_ShapeAspect)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : description ---

	SW.Send(ent->Description());

	// --- own field : ofShape ---

	SW.Send(ent->OfShape());

	// --- own field : productDefinitional ---

	SW.SendLogical(ent->ProductDefinitional());
}


void RWStepRepr_RWShapeAspect::Share(const Handle(StepRepr_ShapeAspect)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->OfShape());
}

