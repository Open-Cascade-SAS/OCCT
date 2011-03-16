// File:	StepFEA_ParametricCurve3dElementCoordinateSystem.cxx
// Created:	Thu Dec 12 17:51:07 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_ParametricCurve3dElementCoordinateSystem.ixx>

//=======================================================================
//function : StepFEA_ParametricCurve3dElementCoordinateSystem
//purpose  : 
//=======================================================================

StepFEA_ParametricCurve3dElementCoordinateSystem::StepFEA_ParametricCurve3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_ParametricCurve3dElementCoordinateSystem::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                                             const Handle(StepFEA_ParametricCurve3dElementCoordinateDirection) &aDirection)
{
  StepFEA_FeaRepresentationItem::Init(aRepresentationItem_Name);

  theDirection = aDirection;
}

//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

Handle(StepFEA_ParametricCurve3dElementCoordinateDirection) StepFEA_ParametricCurve3dElementCoordinateSystem::Direction () const
{
  return theDirection;
}

//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================

void StepFEA_ParametricCurve3dElementCoordinateSystem::SetDirection (const Handle(StepFEA_ParametricCurve3dElementCoordinateDirection) &aDirection)
{
  theDirection = aDirection;
}
