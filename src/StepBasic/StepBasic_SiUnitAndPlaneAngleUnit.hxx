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

#ifndef _StepBasic_SiUnitAndPlaneAngleUnit_HeaderFile
#define _StepBasic_SiUnitAndPlaneAngleUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_SiUnit.hxx>
#include <Standard_Boolean.hxx>
#include <StepBasic_SiPrefix.hxx>
#include <StepBasic_SiUnitName.hxx>
class StepBasic_PlaneAngleUnit;
class StepBasic_DimensionalExponents;


class StepBasic_SiUnitAndPlaneAngleUnit;
DEFINE_STANDARD_HANDLE(StepBasic_SiUnitAndPlaneAngleUnit, StepBasic_SiUnit)


class StepBasic_SiUnitAndPlaneAngleUnit : public StepBasic_SiUnit
{

public:

  
  //! Returns a SiUnitAndPlaneAngleUnit
  Standard_EXPORT StepBasic_SiUnitAndPlaneAngleUnit();
  
  Standard_EXPORT void Init (const Standard_Boolean hasAprefix, const StepBasic_SiPrefix aPrefix, const StepBasic_SiUnitName aName);
  
  Standard_EXPORT void SetPlaneAngleUnit (const Handle(StepBasic_PlaneAngleUnit)& aPlaneAngleUnit);
  
  Standard_EXPORT Handle(StepBasic_PlaneAngleUnit) PlaneAngleUnit() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_SiUnitAndPlaneAngleUnit,StepBasic_SiUnit)

protected:




private:


  Handle(StepBasic_PlaneAngleUnit) planeAngleUnit;


};







#endif // _StepBasic_SiUnitAndPlaneAngleUnit_HeaderFile
