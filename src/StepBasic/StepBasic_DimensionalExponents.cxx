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
