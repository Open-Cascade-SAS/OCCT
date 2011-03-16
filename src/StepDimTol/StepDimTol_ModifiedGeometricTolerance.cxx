// File:	StepDimTol_ModifiedGeometricTolerance.cxx
// Created:	Wed Jun  4 13:34:35 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
