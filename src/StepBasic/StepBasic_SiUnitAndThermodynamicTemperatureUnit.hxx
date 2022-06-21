// Created on: 2002-12-15
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepBasic_SiUnitAndThermodynamicTemperatureUnit_HeaderFile
#define _StepBasic_SiUnitAndThermodynamicTemperatureUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SiPrefix.hxx>
#include <StepBasic_SiUnitName.hxx>
class StepBasic_ThermodynamicTemperatureUnit;


class StepBasic_SiUnitAndThermodynamicTemperatureUnit;
DEFINE_STANDARD_HANDLE(StepBasic_SiUnitAndThermodynamicTemperatureUnit, StepBasic_SiUnit)


class StepBasic_SiUnitAndThermodynamicTemperatureUnit : public StepBasic_SiUnit
{

public:

  
  //! Returns a SiUnitAndThermodynamicTemperatureUnit
  Standard_EXPORT StepBasic_SiUnitAndThermodynamicTemperatureUnit();
  
  Standard_EXPORT void Init (const Standard_Boolean hasAprefix, const StepBasic_SiPrefix aPrefix, const StepBasic_SiUnitName aName);
  
  Standard_EXPORT void SetThermodynamicTemperatureUnit (const Handle(StepBasic_ThermodynamicTemperatureUnit)& aThermodynamicTemperatureUnit);
  
  Standard_EXPORT Handle(StepBasic_ThermodynamicTemperatureUnit) ThermodynamicTemperatureUnit() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_SiUnitAndThermodynamicTemperatureUnit,StepBasic_SiUnit)

protected:




private:


  Handle(StepBasic_ThermodynamicTemperatureUnit) thermodynamicTemperatureUnit;


};







#endif // _StepBasic_SiUnitAndThermodynamicTemperatureUnit_HeaderFile
