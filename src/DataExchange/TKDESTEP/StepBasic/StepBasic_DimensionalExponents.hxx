// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepBasic_DimensionalExponents_HeaderFile
#define _StepBasic_DimensionalExponents_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>

class StepBasic_DimensionalExponents : public Standard_Transient
{

public:
  //! Returns a DimensionalExponents
  Standard_EXPORT StepBasic_DimensionalExponents();

  Standard_EXPORT void Init(const double aLengthExponent,
                            const double aMassExponent,
                            const double aTimeExponent,
                            const double aElectricCurrentExponent,
                            const double aThermodynamicTemperatureExponent,
                            const double aAmountOfSubstanceExponent,
                            const double aLuminousIntensityExponent);

  Standard_EXPORT void SetLengthExponent(const double aLengthExponent);

  Standard_EXPORT double LengthExponent() const;

  Standard_EXPORT void SetMassExponent(const double aMassExponent);

  Standard_EXPORT double MassExponent() const;

  Standard_EXPORT void SetTimeExponent(const double aTimeExponent);

  Standard_EXPORT double TimeExponent() const;

  Standard_EXPORT void SetElectricCurrentExponent(const double aElectricCurrentExponent);

  Standard_EXPORT double ElectricCurrentExponent() const;

  Standard_EXPORT void SetThermodynamicTemperatureExponent(
    const double aThermodynamicTemperatureExponent);

  Standard_EXPORT double ThermodynamicTemperatureExponent() const;

  Standard_EXPORT void SetAmountOfSubstanceExponent(const double aAmountOfSubstanceExponent);

  Standard_EXPORT double AmountOfSubstanceExponent() const;

  Standard_EXPORT void SetLuminousIntensityExponent(const double aLuminousIntensityExponent);

  Standard_EXPORT double LuminousIntensityExponent() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_DimensionalExponents, Standard_Transient)

private:
  double lengthExponent;
  double massExponent;
  double timeExponent;
  double electricCurrentExponent;
  double thermodynamicTemperatureExponent;
  double amountOfSubstanceExponent;
  double luminousIntensityExponent;
};

#endif // _StepBasic_DimensionalExponents_HeaderFile
