// File:	StepDimTol_ShapeToleranceSelect.cxx
// Created:	Wed Jun  4 13:34:33 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepDimTol_ShapeToleranceSelect.ixx>

//=======================================================================
//function : StepDimTol_ShapeToleranceSelect
//purpose  : 
//=======================================================================

StepDimTol_ShapeToleranceSelect::StepDimTol_ShapeToleranceSelect ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepDimTol_ShapeToleranceSelect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_PlusMinusTolerance))) return 2;
  return 0;
}

//=======================================================================
//function : GeometricTolerance
//purpose  : 
//=======================================================================

Handle(StepDimTol_GeometricTolerance) StepDimTol_ShapeToleranceSelect::GeometricTolerance () const
{
  return Handle(StepDimTol_GeometricTolerance)::DownCast(Value());
}

//=======================================================================
//function : PlusMinusTolerance
//purpose  : 
//=======================================================================

Handle(StepShape_PlusMinusTolerance) StepDimTol_ShapeToleranceSelect::PlusMinusTolerance () const
{
  return Handle(StepShape_PlusMinusTolerance)::DownCast(Value());
}
