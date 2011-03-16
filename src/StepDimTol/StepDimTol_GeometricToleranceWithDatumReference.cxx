// File:	StepDimTol_GeometricToleranceWithDatumReference.cxx
// Created:	Wed Jun  4 13:34:35 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
