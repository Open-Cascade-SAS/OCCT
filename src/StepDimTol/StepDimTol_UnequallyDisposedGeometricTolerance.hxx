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

#ifndef _StepDimTol_UnequallyDisposedGeometricTolerance_HeaderFile
#define _StepDimTol_UnequallyDisposedGeometricTolerance_HeaderFile

#include <Standard.hxx>

#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepDimTol_GeometricTolerance.hxx>

class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepDimTol_GeometricToleranceTarget;

class StepDimTol_UnequallyDisposedGeometricTolerance;
DEFINE_STANDARD_HANDLE(StepDimTol_UnequallyDisposedGeometricTolerance, StepDimTol_GeometricTolerance)
//! Representation of STEP entity UnequallyDisposedGeometricTolerance
class StepDimTol_UnequallyDisposedGeometricTolerance : public StepDimTol_GeometricTolerance
{

public:
  
  //! Empty constructor
  Standard_EXPORT StepDimTol_UnequallyDisposedGeometricTolerance();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT   void Init (const Handle(TCollection_HAsciiString)& theName, const Handle(TCollection_HAsciiString)& theDescription, const Handle(StepBasic_MeasureWithUnit)& theMagnitude, const StepDimTol_GeometricToleranceTarget& theTolerancedShapeAspect, const Handle(StepBasic_LengthMeasureWithUnit)& theDisplacement) ;

  //! Returns field Displacement
  inline Handle(StepBasic_LengthMeasureWithUnit) Displacement () const
  {
    return myDisplacement;
  }
  
  //! Set field Displacement
  inline void SetDisplacement (const Handle(StepBasic_LengthMeasureWithUnit) &theDisplacement)
  {
    myDisplacement = theDisplacement;
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_UnequallyDisposedGeometricTolerance,StepDimTol_GeometricTolerance)

private: 
  Handle(StepBasic_LengthMeasureWithUnit) myDisplacement;
};
#endif // _StepDimTol_UnequallyDisposedGeometricTolerance_HeaderFile
