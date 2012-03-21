// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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
