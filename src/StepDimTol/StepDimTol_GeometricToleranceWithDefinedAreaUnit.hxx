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

#ifndef _StepDimTol_GeometricToleranceWithDefinedAreaUnit_HeaderFile
#define _StepDimTol_GeometricToleranceWithDefinedAreaUnit_HeaderFile

#include <Standard.hxx>

#include <Standard_Boolean.hxx>
#include <StepDimTol_AreaUnitType.hxx>
#include <StepDimTol_GeometricToleranceWithDefinedUnit.hxx>

class StepBasic_LengthMeasureWithUnit;
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepDimTol_GeometricToleranceTarget;

class StepDimTol_GeometricToleranceWithDefinedAreaUnit;
DEFINE_STANDARD_HANDLE(StepDimTol_GeometricToleranceWithDefinedAreaUnit, StepDimTol_GeometricToleranceWithDefinedUnit)
//! Representation of STEP entity GeometricToleranceWithDefinedAreaUnit
class StepDimTol_GeometricToleranceWithDefinedAreaUnit : public StepDimTol_GeometricToleranceWithDefinedUnit
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepDimTol_GeometricToleranceWithDefinedAreaUnit();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT   void Init(const Handle(TCollection_HAsciiString)& theName, 
                              const Handle(TCollection_HAsciiString)& theDescription, 
                              const Handle(StepBasic_MeasureWithUnit)& theMagnitude, 
                              const StepDimTol_GeometricToleranceTarget& theTolerancedShapeAspect, 
                              const Handle(StepBasic_LengthMeasureWithUnit)& theUnitSize, 
                              const StepDimTol_AreaUnitType theAreaType, const Standard_Boolean theHasSecondUnitSize, 
                              const Handle(StepBasic_LengthMeasureWithUnit)& theSecondUnitSize) ;
  
  //! Returns field AreaType
  inline StepDimTol_AreaUnitType AreaType () const
  {
    return myAreaType;
  }

  //! Set field AreaType
  inline void SetAreaType (const StepDimTol_AreaUnitType theAreaType)
  {
    myAreaType = theAreaType;
  }

  //! Returns field SecondUnitSize
  inline Handle(StepBasic_LengthMeasureWithUnit) SecondUnitSize () const
  {
    return mySecondUnitSize;
  }

  //! Set field SecondUnitSize
  inline void SetSecondUnitSize (const Handle(StepBasic_LengthMeasureWithUnit) &theSecondUnitSize)
  {
    mySecondUnitSize = theSecondUnitSize;
  }

  //! Indicates if SecondUnitSize field exist
  inline Standard_Boolean HasSecondUnitSize () const
  {
    return mySecondUnitSize.IsNull();
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeometricToleranceWithDefinedAreaUnit,StepDimTol_GeometricToleranceWithDefinedUnit)

private: 
  StepDimTol_AreaUnitType myAreaType;
  Handle(StepBasic_LengthMeasureWithUnit) mySecondUnitSize;
};
#endif // _StepDimTol_GeometricToleranceWithDefinedAreaUnit_HeaderFile
