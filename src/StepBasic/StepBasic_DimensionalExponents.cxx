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

#include <StepBasic_DimensionalExponents.ixx>


StepBasic_DimensionalExponents::StepBasic_DimensionalExponents ()  {}

void StepBasic_DimensionalExponents::Init(
	const Standard_Real aLengthExponent,
	const Standard_Real aMassExponent,
	const Standard_Real aTimeExponent,
	const Standard_Real aElectricCurrentExponent,
	const Standard_Real aThermodynamicTemperatureExponent,
	const Standard_Real aAmountOfSubstanceExponent,
	const Standard_Real aLuminousIntensityExponent)
{
	// --- classe own fields ---
	lengthExponent = aLengthExponent;
	massExponent = aMassExponent;
	timeExponent = aTimeExponent;
	electricCurrentExponent = aElectricCurrentExponent;
	thermodynamicTemperatureExponent = aThermodynamicTemperatureExponent;
	amountOfSubstanceExponent = aAmountOfSubstanceExponent;
	luminousIntensityExponent = aLuminousIntensityExponent;
}


void StepBasic_DimensionalExponents::SetLengthExponent(const Standard_Real aLengthExponent)
{
	lengthExponent = aLengthExponent;
}

Standard_Real StepBasic_DimensionalExponents::LengthExponent() const
{
	return lengthExponent;
}

void StepBasic_DimensionalExponents::SetMassExponent(const Standard_Real aMassExponent)
{
	massExponent = aMassExponent;
}

Standard_Real StepBasic_DimensionalExponents::MassExponent() const
{
	return massExponent;
}

void StepBasic_DimensionalExponents::SetTimeExponent(const Standard_Real aTimeExponent)
{
	timeExponent = aTimeExponent;
}

Standard_Real StepBasic_DimensionalExponents::TimeExponent() const
{
	return timeExponent;
}

void StepBasic_DimensionalExponents::SetElectricCurrentExponent(const Standard_Real aElectricCurrentExponent)
{
	electricCurrentExponent = aElectricCurrentExponent;
}

Standard_Real StepBasic_DimensionalExponents::ElectricCurrentExponent() const
{
	return electricCurrentExponent;
}

void StepBasic_DimensionalExponents::SetThermodynamicTemperatureExponent(const Standard_Real aThermodynamicTemperatureExponent)
{
	thermodynamicTemperatureExponent = aThermodynamicTemperatureExponent;
}

Standard_Real StepBasic_DimensionalExponents::ThermodynamicTemperatureExponent() const
{
	return thermodynamicTemperatureExponent;
}

void StepBasic_DimensionalExponents::SetAmountOfSubstanceExponent(const Standard_Real aAmountOfSubstanceExponent)
{
	amountOfSubstanceExponent = aAmountOfSubstanceExponent;
}

Standard_Real StepBasic_DimensionalExponents::AmountOfSubstanceExponent() const
{
	return amountOfSubstanceExponent;
}

void StepBasic_DimensionalExponents::SetLuminousIntensityExponent(const Standard_Real aLuminousIntensityExponent)
{
	luminousIntensityExponent = aLuminousIntensityExponent;
}

Standard_Real StepBasic_DimensionalExponents::LuminousIntensityExponent() const
{
	return luminousIntensityExponent;
}
