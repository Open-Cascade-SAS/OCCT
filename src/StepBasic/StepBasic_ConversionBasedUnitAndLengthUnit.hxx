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

#ifndef _StepBasic_ConversionBasedUnitAndLengthUnit_HeaderFile
#define _StepBasic_ConversionBasedUnitAndLengthUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ConversionBasedUnit.hxx>
class StepBasic_LengthUnit;
class StepBasic_DimensionalExponents;
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;


class StepBasic_ConversionBasedUnitAndLengthUnit;
DEFINE_STANDARD_HANDLE(StepBasic_ConversionBasedUnitAndLengthUnit, StepBasic_ConversionBasedUnit)


class StepBasic_ConversionBasedUnitAndLengthUnit : public StepBasic_ConversionBasedUnit
{

public:

  
  //! Returns a ConversionBasedUnitAndLengthUnit
  Standard_EXPORT StepBasic_ConversionBasedUnitAndLengthUnit();
  
  Standard_EXPORT void Init (const Handle(StepBasic_DimensionalExponents)& aDimensions, const Handle(TCollection_HAsciiString)& aName, const Handle(StepBasic_MeasureWithUnit)& aConversionFactor);
  
  Standard_EXPORT void SetLengthUnit (const Handle(StepBasic_LengthUnit)& aLengthUnit);
  
  Standard_EXPORT Handle(StepBasic_LengthUnit) LengthUnit() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_ConversionBasedUnitAndLengthUnit,StepBasic_ConversionBasedUnit)

protected:




private:


  Handle(StepBasic_LengthUnit) lengthUnit;


};







#endif // _StepBasic_ConversionBasedUnitAndLengthUnit_HeaderFile
