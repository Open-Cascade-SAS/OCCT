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

#include <StepDimTol_GeometricToleranceWithDatumReference.ixx>

//=======================================================================
//function : StepDimTol_GeometricToleranceWithDatumReference
//purpose  : 
//=======================================================================

StepDimTol_GeometricToleranceWithDatumReference::StepDimTol_GeometricToleranceWithDatumReference ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceWithDatumReference::Init (const Handle(TCollection_HAsciiString) &aGeometricTolerance_Name,
                                                            const Handle(TCollection_HAsciiString) &aGeometricTolerance_Description,
                                                            const Handle(StepBasic_MeasureWithUnit) &aGeometricTolerance_Magnitude,
                                                            const Handle(StepRepr_ShapeAspect) &aGeometricTolerance_TolerancedShapeAspect,
                                                            const Handle(StepDimTol_HArray1OfDatumReference) &aDatumSystem)
{
  StepDimTol_GeometricTolerance::Init(aGeometricTolerance_Name,
                                      aGeometricTolerance_Description,
                                      aGeometricTolerance_Magnitude,
                                      aGeometricTolerance_TolerancedShapeAspect);

  theDatumSystem = aDatumSystem;
}

//=======================================================================
//function : DatumSystem
//purpose  : 
//=======================================================================

Handle(StepDimTol_HArray1OfDatumReference) StepDimTol_GeometricToleranceWithDatumReference::DatumSystem () const
{
  return theDatumSystem;
}

//=======================================================================
//function : SetDatumSystem
//purpose  : 
//=======================================================================

void StepDimTol_GeometricToleranceWithDatumReference::SetDatumSystem (const Handle(StepDimTol_HArray1OfDatumReference) &aDatumSystem)
{
  theDatumSystem = aDatumSystem;
}
