// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Transient.hxx>
#include <StepFEA_AlignedCurve3dElementCoordinateSystem.hxx>
#include <StepFEA_CurveElementEndCoordinateSystem.hxx>
#include <StepFEA_FeaAxis2Placement3d.hxx>
#include <StepFEA_ParametricCurve3dElementCoordinateSystem.hxx>

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
