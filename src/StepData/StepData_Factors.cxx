// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <StepData_Factors.hxx>

// ============================================================================
// Method : Consturctor
// Purpose:
// ============================================================================
StepData_Factors::StepData_Factors()
  :myLengthFactor(1.), myPlaneAngleFactor(1.),
   mySolidAngleFactor(1.), myFactRD(1.),
   myFactDR(1.), myCascadeUnit(1.)
{}

// ============================================================================
// Method : InitializeFactors
// Purpose:
// ============================================================================
void StepData_Factors::InitializeFactors(
  const Standard_Real theLengthFactor,
  const Standard_Real thePlaneAngleFactor,
  const Standard_Real theSolidAngleFactor)
{
  myLengthFactor = theLengthFactor;
  myPlaneAngleFactor = thePlaneAngleFactor;
  mySolidAngleFactor = theSolidAngleFactor;
  myFactRD = 1. / thePlaneAngleFactor;
  myFactDR = thePlaneAngleFactor;
}

// ============================================================================
// Method : LengthFactor
// Purpose:
// ============================================================================
Standard_Real StepData_Factors::LengthFactor() const
{
  return myLengthFactor;
}

// ============================================================================
// Method : PlaneAngleFactor
// Purpose:
// ============================================================================
Standard_Real StepData_Factors::PlaneAngleFactor() const
{
  return myPlaneAngleFactor;
}

// ============================================================================
// Method : SolidAngleFactor
// Purpose:
// ============================================================================
Standard_Real StepData_Factors::SolidAngleFactor() const
{
  return mySolidAngleFactor;
}

// ============================================================================
// Method : FactorRadianDegree
// Purpose:
// ============================================================================
Standard_Real StepData_Factors::FactorRadianDegree() const
{
  return myFactRD;
}

// ============================================================================
// Method : FactorDegreeRadian
// Purpose:
// ============================================================================
Standard_Real StepData_Factors::FactorDegreeRadian() const
{
  return myFactDR;
}

// ============================================================================
// Method : SetCascadeUnit
// Purpose:
// ============================================================================
void StepData_Factors::SetCascadeUnit(const Standard_Real theUnit)
{
  myCascadeUnit = theUnit;
}

// ============================================================================
// Method : CascadeUnit
// Purpose:
// ============================================================================
Standard_Real StepData_Factors::CascadeUnit() const
{
  return myCascadeUnit;
}
