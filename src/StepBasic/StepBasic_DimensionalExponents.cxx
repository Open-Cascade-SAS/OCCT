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


#include <Standard_Type.hxx>
#include <StepBasic_DimensionalExponents.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DimensionalExponents,Standard_Transient)

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
