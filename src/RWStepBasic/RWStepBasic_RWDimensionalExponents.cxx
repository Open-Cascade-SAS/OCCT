// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <RWStepBasic_RWDimensionalExponents.ixx>


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
