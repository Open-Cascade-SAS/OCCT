// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepDimTol_ModifiedGeometricTolerance.ixx>

//=======================================================================
//function : StepDimTol_ModifiedGeometricTolerance
//purpose  : 
//=======================================================================

StepDimTol_ModifiedGeometricTolerance::StepDimTol_ModifiedGeometricTolerance ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_ModifiedGeometricTolerance::Init (const Handle(TCollection_HAsciiString) &aGeometricTolerance_Name,
                                                  const Handle(TCollection_HAsciiString) &aGeometricTolerance_Description,
                                                  const Handle(StepBasic_MeasureWithUnit) &aGeometricTolerance_Magnitude,
                                                  const Handle(StepRepr_ShapeAspect) &aGeometricTolerance_TolerancedShapeAspect,
                                                  const StepDimTol_LimitCondition aModifier)
{
  StepDimTol_GeometricTolerance::Init(aGeometricTolerance_Name,
                                      aGeometricTolerance_Description,
                                      aGeometricTolerance_Magnitude,
                                      aGeometricTolerance_TolerancedShapeAspect);

  theModifier = aModifier;
}

//=======================================================================
//function : Modifier
//purpose  : 
//=======================================================================

StepDimTol_LimitCondition StepDimTol_ModifiedGeometricTolerance::Modifier () const
{
  return theModifier;
}

//=======================================================================
//function : SetModifier
//purpose  : 
//=======================================================================

void StepDimTol_ModifiedGeometricTolerance::SetModifier (const StepDimTol_LimitCondition aModifier)
{
  theModifier = aModifier;
}
