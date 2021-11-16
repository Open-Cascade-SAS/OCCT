// Created on: 1999-10-11
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepBasic_SiUnitAndAreaUnit_HeaderFile
#define _StepBasic_SiUnitAndAreaUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_SiUnit.hxx>
class StepBasic_AreaUnit;


class StepBasic_SiUnitAndAreaUnit;
DEFINE_STANDARD_HANDLE(StepBasic_SiUnitAndAreaUnit, StepBasic_SiUnit)


class StepBasic_SiUnitAndAreaUnit : public StepBasic_SiUnit
{

public:

  
  //! Returns a SiUnitAndAreaUnit
  Standard_EXPORT StepBasic_SiUnitAndAreaUnit();
  
  Standard_EXPORT void SetAreaUnit (const Handle(StepBasic_AreaUnit)& anAreaUnit);
  
  Standard_EXPORT Handle(StepBasic_AreaUnit) AreaUnit() const;
  
  DEFINE_STANDARD_RTTIEXT(StepBasic_SiUnitAndAreaUnit,StepBasic_SiUnit)

protected:




private:


  Handle(StepBasic_AreaUnit) areaUnit;


};







#endif // _StepBasic_SiUnitAndAreaUnit_HeaderFile
