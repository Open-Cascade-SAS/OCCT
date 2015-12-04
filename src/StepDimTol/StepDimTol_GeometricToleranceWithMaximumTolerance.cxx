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

#include <StepDimTol_GeometricToleranceWithMaximumTolerance.hxx>

#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_GeometricToleranceWithMaximumTolerance,StepDimTol_GeometricToleranceWithModifiers)

//=======================================================================
//function : StepDimTol_GeometricToleranceWithModifiers
//purpose  : 
//=======================================================================

StepDimTol_GeometricToleranceWithMaximumTolerance::StepDimTol_GeometricToleranceWithMaximumTolerance ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceWithMaximumTolerance::
  Init (const Handle(TCollection_HAsciiString) &theName,
        const Handle(TCollection_HAsciiString) &theDescription,
        const Handle(StepBasic_MeasureWithUnit) &theMagnitude,
        const StepDimTol_GeometricToleranceTarget &theTolerancedShapeAspect,
        const Handle(StepDimTol_HArray1OfGeometricToleranceModifier) &theModifiers,
        const Handle(StepBasic_LengthMeasureWithUnit) &theMaximumUpperTolerance)
{
  StepDimTol_GeometricToleranceWithModifiers::Init(theName, theDescription, theMagnitude, theTolerancedShapeAspect, theModifiers);
  myMaximumUpperTolerance = theMaximumUpperTolerance;
}
