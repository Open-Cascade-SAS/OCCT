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
#include <RWStepBasic_RWDimensionalExponents.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWDimensionalExponents::RWStepBasic_RWDimensionalExponents () {}

void RWStepBasic_RWDimensionalExponents::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_DimensionalExponents)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,7,ach,"dimensional_exponents")) return;

	// --- own field : lengthExponent ---

	Standard_Real aLengthExponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadReal (num,1,"length_exponent",ach,aLengthExponent);

	// --- own field : massExponent ---

	Standard_Real aMassExponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadReal (num,2,"mass_exponent",ach,aMassExponent);

	// --- own field : timeExponent ---

	Standard_Real aTimeExponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadReal (num,3,"time_exponent",ach,aTimeExponent);

	// --- own field : electricCurrentExponent ---

	Standard_Real aElectricCurrentExponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadReal (num,4,"electric_current_exponent",ach,aElectricCurrentExponent);

	// --- own field : thermodynamicTemperatureExponent ---

	Standard_Real aThermodynamicTemperatureExponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadReal (num,5,"thermodynamic_temperature_exponent",ach,aThermodynamicTemperatureExponent);

	// --- own field : amountOfSubstanceExponent ---

	Standard_Real aAmountOfSubstanceExponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	data->ReadReal (num,6,"amount_of_substance_exponent",ach,aAmountOfSubstanceExponent);

	// --- own field : luminousIntensityExponent ---

	Standard_Real aLuminousIntensityExponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat7 =` not needed
	data->ReadReal (num,7,"luminous_intensity_exponent",ach,aLuminousIntensityExponent);

	//--- Initialisation of the read entity ---


	ent->Init(aLengthExponent, aMassExponent, aTimeExponent, aElectricCurrentExponent, aThermodynamicTemperatureExponent, aAmountOfSubstanceExponent, aLuminousIntensityExponent);
}


void RWStepBasic_RWDimensionalExponents::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_DimensionalExponents)& ent) const
{

	// --- own field : lengthExponent ---

	SW.Send(ent->LengthExponent());

	// --- own field : massExponent ---

	SW.Send(ent->MassExponent());

	// --- own field : timeExponent ---

	SW.Send(ent->TimeExponent());

	// --- own field : electricCurrentExponent ---

	SW.Send(ent->ElectricCurrentExponent());

	// --- own field : thermodynamicTemperatureExponent ---

	SW.Send(ent->ThermodynamicTemperatureExponent());

	// --- own field : amountOfSubstanceExponent ---

	SW.Send(ent->AmountOfSubstanceExponent());

	// --- own field : luminousIntensityExponent ---

	SW.Send(ent->LuminousIntensityExponent());
}
