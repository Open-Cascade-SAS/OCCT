// File:	StepFEA_CurveElementLocation.cxx
// Created:	Thu Dec 12 17:51:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_CurveElementLocation.ixx>

//=======================================================================
//function : StepFEA_CurveElementLocation
//purpose  : 
//=======================================================================

StepFEA_CurveElementLocation::StepFEA_CurveElementLocation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_CurveElementLocation::Init (const Handle(StepFEA_FeaParametricPoint) &aCoordinate)
{

  theCoordinate = aCoordinate;
}

//=======================================================================
//function : Coordinate
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaParametricPoint) StepFEA_CurveElementLocation::Coordinate () const
{
  return theCoordinate;
}

//=======================================================================
//function : SetCoordinate
//purpose  : 
//=======================================================================

void StepFEA_CurveElementLocation::SetCoordinate (const Handle(StepFEA_FeaParametricPoint) &aCoordinate)
{
  theCoordinate = aCoordinate;
}
