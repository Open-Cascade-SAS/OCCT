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

class StepBasic_DimensionalExponents;
DEFINE_STANDARD_HANDLE(StepBasic_DimensionalExponents, Standard_Transient)

class StepBasic_DimensionalExponents : public Standard_Transient
{

public:
  //! Returns a DimensionalExponents
  Standard_EXPORT StepBasic_DimensionalExponents();

  Standard_EXPORT void Init(const Standard_Real aLengthExponent,
                            const Standard_Real aMassExponent,
                            const Standard_Real aTimeExponent,
                            const Standard_Real aElectricCurrentExponent,
                            const Standard_Real aThermodynamicTemperatureExponent,
                            const Standard_Real aAmountOfSubstanceExponent,
                            const Standard_Real aLuminousIntensityExponent);

  Standard_EXPORT void SetLengthExponent(const Standard_Real aLengthExponent);

  Standard_EXPORT Standard_Real LengthExponent() const;

  Standard_EXPORT void SetMassExponent(const Standard_Real aMassExponent);

  Standard_EXPORT Standard_Real MassExponent() const;

  Standard_EXPORT void SetTimeExponent(const Standard_Real aTimeExponent);

  Standard_EXPORT Standard_Real TimeExponent() const;

  Standard_EXPORT void SetElectricCurrentExponent(const Standard_Real aElectricCurrentExponent);

  Standard_EXPORT Standard_Real ElectricCurrentExponent() const;

  Standard_EXPORT void SetThermodynamicTemperatureExponent(
    const Standard_Real aThermodynamicTemperatureExponent);

  Standard_EXPORT Standard_Real ThermodynamicTemperatureExponent() const;

  Standard_EXPORT void SetAmountOfSubstanceExponent(const Standard_Real aAmountOfSubstanceExponent);

  Standard_EXPORT Standard_Real AmountOfSubstanceExponent() const;

  Standard_EXPORT void SetLuminousIntensityExponent(const Standard_Real aLuminousIntensityExponent);

  Standard_EXPORT Standard_Real LuminousIntensityExponent() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_DimensionalExponents, Standard_Transient)

protected:
private:
  Standard_Real lengthExponent;
  Standard_Real massExponent;
  Standard_Real timeExponent;
  Standard_Real electricCurrentExponent;
  Standard_Real thermodynamicTemperatureExponent;
  Standard_Real amountOfSubstanceExponent;
  Standard_Real luminousIntensityExponent;
};

#endif // _StepBasic_DimensionalExponents_HeaderFile
