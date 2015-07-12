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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit.hxx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndSolidAngleUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_SolidAngleUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit::RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit () {}

void RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ConversionBasedUnitAndSolidAngleUnit)& ent) const
{

	Standard_Integer num = num0;


	// --- Instance of plex componant ConversionBasedUnit ---

	if (!data->CheckNbParams(num,2,ach,"conversion_based_unit")) return;

	// --- field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- field : conversionFactor ---

	Handle(StepBasic_MeasureWithUnit) aConversionFactor;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"conversion_factor", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), aConversionFactor);

	num = data->NextForComplex(num);

	// --- Instance of common supertype NamedUnit ---

	if (!data->CheckNbParams(num,1,ach,"named_unit")) return;
	// --- field : dimensions ---


	Handle(StepBasic_DimensionalExponents) aDimensions;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 1,"dimensions", ach, STANDARD_TYPE(StepBasic_DimensionalExponents), aDimensions);

	num = data->NextForComplex(num);

	// --- Instance of plex componant SolidAngleUnit ---

	if (!data->CheckNbParams(num,0,ach,"solid_angle_unit")) return;

	//--- Initialisation of the red entity ---

	ent->Init(aDimensions,aName,aConversionFactor);
}


void RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ConversionBasedUnitAndSolidAngleUnit)& ent) const
{

	// --- Instance of plex componant ConversionBasedUnit ---

	SW.StartEntity("CONVERSION_BASED_UNIT");
	// --- field : name ---

	SW.Send(ent->Name());
	// --- field : conversionFactor ---

	SW.Send(ent->ConversionFactor());

	// --- Instance of common supertype NamedUnit ---

	SW.StartEntity("NAMED_UNIT");
	// --- field : dimensions ---

	SW.Send(ent->Dimensions());

	// --- Instance of plex componant SolidAngleUnit ---

	SW.StartEntity("SOLID_ANGLE_UNIT");
}


void RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit::Share(const Handle(StepBasic_ConversionBasedUnitAndSolidAngleUnit)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Dimensions());


	iter.GetOneItem(ent->ConversionFactor());
}

