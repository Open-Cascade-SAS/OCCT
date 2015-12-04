// Created on: 2015-07-07
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

#ifndef _StepDimTol_GeometricToleranceWithDefinedUnit_HeaderFile
#define _StepDimTol_GeometricToleranceWithDefinedUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepDimTol_GeometricTolerance.hxx>

class StepBasic_LengthMeasureWithUnit;
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepDimTol_GeometricToleranceTarget;
class StepRepr_ShapeAspect;

class StepDimTol_GeometricToleranceWithDefinedUnit;
DEFINE_STANDARD_HANDLE(StepDimTol_GeometricToleranceWithDefinedUnit, StepDimTol_GeometricTolerance)
//! Representation of STEP entity GeometricToleranceWithDefinedUnit
class StepDimTol_GeometricToleranceWithDefinedUnit : public StepDimTol_GeometricTolerance
{

public:
  
  //! Empty constructor
  Standard_EXPORT StepDimTol_GeometricToleranceWithDefinedUnit();
  
  //! Initialize all fields (own and inherited) AP214
  Standard_EXPORT   void Init (const Handle(TCollection_HAsciiString)& theName, const Handle(TCollection_HAsciiString)& theDescription, const Handle(StepBasic_MeasureWithUnit)& theMagnitude, const Handle(StepRepr_ShapeAspect)& theTolerancedShapeAspect, const Handle(StepBasic_LengthMeasureWithUnit)& theUnitSize) ;
  
  //! Initialize all fields (own and inherited) AP242
  Standard_EXPORT   void Init (const Handle(TCollection_HAsciiString)& theName, const Handle(TCollection_HAsciiString)& theDescription, const Handle(StepBasic_MeasureWithUnit)& theMagnitude, const StepDimTol_GeometricToleranceTarget& theTolerancedShapeAspect, const Handle(StepBasic_LengthMeasureWithUnit)& theUnitSize) ;

  //! Returns field UnitSize
  inline Handle(StepBasic_LengthMeasureWithUnit) UnitSize () const
  {
    return myUnitSize;
  }
  
  //! Set field UnitSize
  inline void SetUnitSize (const Handle(StepBasic_LengthMeasureWithUnit) &theUnitSize)
  {
    myUnitSize = theUnitSize;
  } 
  
  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeometricToleranceWithDefinedUnit,StepDimTol_GeometricTolerance)

private: 
  Handle(StepBasic_LengthMeasureWithUnit) myUnitSize;
};
#endif // _StepDimTol_GeometricToleranceWithDefinedUnit_HeaderFile
