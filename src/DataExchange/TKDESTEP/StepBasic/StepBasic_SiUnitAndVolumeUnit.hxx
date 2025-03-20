// Created on: 1999-10-12
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

#ifndef _StepBasic_SiUnitAndVolumeUnit_HeaderFile
#define _StepBasic_SiUnitAndVolumeUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_SiUnit.hxx>
class StepBasic_VolumeUnit;

class StepBasic_SiUnitAndVolumeUnit;
DEFINE_STANDARD_HANDLE(StepBasic_SiUnitAndVolumeUnit, StepBasic_SiUnit)

class StepBasic_SiUnitAndVolumeUnit : public StepBasic_SiUnit
{

public:
  //! Returns a SiUnitAndVolumeUnit
  Standard_EXPORT StepBasic_SiUnitAndVolumeUnit();

  Standard_EXPORT void SetVolumeUnit(const Handle(StepBasic_VolumeUnit)& aVolumeUnit);

  Standard_EXPORT Handle(StepBasic_VolumeUnit) VolumeUnit() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_SiUnitAndVolumeUnit, StepBasic_SiUnit)

protected:
private:
  Handle(StepBasic_VolumeUnit) volumeUnit;
};

#endif // _StepBasic_SiUnitAndVolumeUnit_HeaderFile
