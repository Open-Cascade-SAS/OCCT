// File:	StepFEA_CurveElementEndCoordinateSystem.cxx
// Created:	Thu Dec 12 17:51:03 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_CurveElementEndCoordinateSystem.ixx>

//=======================================================================
//function : StepFEA_CurveElementEndCoordinateSystem
//purpose  : 
//=======================================================================

StepFEA_CurveElementEndCoordinateSystem::StepFEA_CurveElementEndCoordinateSystem ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_CurveElementEndCoordinateSystem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepFEA_FeaAxis2Placement3d))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepFEA_AlignedCurve3dElementCoordinateSystem))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepFEA_ParametricCurve3dElementCoordinateSystem))) return 3;
  return 0;
}

//=======================================================================
//function : FeaAxis2Placement3d
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaAxis2Placement3d) StepFEA_CurveElementEndCoordinateSystem::FeaAxis2Placement3d () const
{
  return Handle(StepFEA_FeaAxis2Placement3d)::DownCast(Value());
}

//=======================================================================
//function : AlignedCurve3dElementCoordinateSystem
//purpose  : 
//=======================================================================

Handle(StepFEA_AlignedCurve3dElementCoordinateSystem) StepFEA_CurveElementEndCoordinateSystem::AlignedCurve3dElementCoordinateSystem () const
{
  return Handle(StepFEA_AlignedCurve3dElementCoordinateSystem)::DownCast(Value());
}

//=======================================================================
//function : ParametricCurve3dElementCoordinateSystem
//purpose  : 
//=======================================================================

Handle(StepFEA_ParametricCurve3dElementCoordinateSystem) StepFEA_CurveElementEndCoordinateSystem::ParametricCurve3dElementCoordinateSystem () const
{
  return Handle(StepFEA_ParametricCurve3dElementCoordinateSystem)::DownCast(Value());
}
