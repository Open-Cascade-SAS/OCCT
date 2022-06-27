// Created on: 2015-07-13
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepDimTol_RunoutZoneOrientation_HeaderFile
#define _StepDimTol_RunoutZoneOrientation_HeaderFile

#include <StepDimTol_RunoutZoneOrientation.hxx>

#include <Standard_Transient.hxx>
#include <Standard.hxx>
#include <StepBasic_PlaneAngleMeasureWithUnit.hxx>

class StepDimTol_RunoutZoneOrientation;
DEFINE_STANDARD_HANDLE(StepDimTol_RunoutZoneOrientation, Standard_Transient)
//! Added for Dimensional Tolerances
class StepDimTol_RunoutZoneOrientation : public Standard_Transient
{

public:
  
  Standard_EXPORT StepDimTol_RunoutZoneOrientation();
  
  //! Init all field own and inherited
  Standard_EXPORT void Init (const Handle(StepBasic_PlaneAngleMeasureWithUnit)& theAngle);

  //! Returns field Angle
  inline Handle(StepBasic_PlaneAngleMeasureWithUnit) Angle()
  {
    return myAngle;
  }
  
  //! Set field Angle
  inline void SetAngle(const Handle(StepBasic_PlaneAngleMeasureWithUnit) &theAngle)
  {
    myAngle = theAngle;
  }
  
  DEFINE_STANDARD_RTTIEXT(StepDimTol_RunoutZoneOrientation,Standard_Transient)
  
private:
  Handle(StepBasic_PlaneAngleMeasureWithUnit) myAngle;

};
#endif // _StepDimTol_RunoutZoneOrientation_HeaderFile
