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

#ifndef _StepBasic_ConversionBasedUnit_HeaderFile
#define _StepBasic_ConversionBasedUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_NamedUnit.hxx>
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepBasic_DimensionalExponents;


class StepBasic_ConversionBasedUnit;
DEFINE_STANDARD_HANDLE(StepBasic_ConversionBasedUnit, StepBasic_NamedUnit)


class StepBasic_ConversionBasedUnit : public StepBasic_NamedUnit
{

public:

  
  //! Returns a ConversionBasedUnit
  Standard_EXPORT StepBasic_ConversionBasedUnit();
  
  Standard_EXPORT void Init (const Handle(StepBasic_DimensionalExponents)& aDimensions, const Handle(TCollection_HAsciiString)& aName, const Handle(StepBasic_MeasureWithUnit)& aConversionFactor);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetConversionFactor (const Handle(StepBasic_MeasureWithUnit)& aConversionFactor);
  
  Standard_EXPORT Handle(StepBasic_MeasureWithUnit) ConversionFactor() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_ConversionBasedUnit,StepBasic_NamedUnit)

protected:




private:


  Handle(TCollection_HAsciiString) name;
  Handle(StepBasic_MeasureWithUnit) conversionFactor;


};







#endif // _StepBasic_ConversionBasedUnit_HeaderFile
