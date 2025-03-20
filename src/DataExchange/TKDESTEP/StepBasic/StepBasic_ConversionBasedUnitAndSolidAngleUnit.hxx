// Created on: 1994-06-17
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _StepBasic_ConversionBasedUnitAndSolidAngleUnit_HeaderFile
#define _StepBasic_ConversionBasedUnitAndSolidAngleUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ConversionBasedUnit.hxx>
class StepBasic_SolidAngleUnit;
class StepBasic_DimensionalExponents;
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;

class StepBasic_ConversionBasedUnitAndSolidAngleUnit;
DEFINE_STANDARD_HANDLE(StepBasic_ConversionBasedUnitAndSolidAngleUnit,
                       StepBasic_ConversionBasedUnit)

class StepBasic_ConversionBasedUnitAndSolidAngleUnit : public StepBasic_ConversionBasedUnit
{

public:
  //! Returns a ConversionBasedUnitAndSolidAngleUnit
  Standard_EXPORT StepBasic_ConversionBasedUnitAndSolidAngleUnit();

  Standard_EXPORT void Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
                            const Handle(TCollection_HAsciiString)&       aName,
                            const Handle(StepBasic_MeasureWithUnit)&      aConversionFactor);

  Standard_EXPORT void SetSolidAngleUnit(const Handle(StepBasic_SolidAngleUnit)& aSolidAngleUnit);

  Standard_EXPORT Handle(StepBasic_SolidAngleUnit) SolidAngleUnit() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_ConversionBasedUnitAndSolidAngleUnit,
                          StepBasic_ConversionBasedUnit)

protected:
private:
  Handle(StepBasic_SolidAngleUnit) solidAngleUnit;
};

#endif // _StepBasic_ConversionBasedUnitAndSolidAngleUnit_HeaderFile
