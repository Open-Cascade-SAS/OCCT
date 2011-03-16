// File:	StepFEA_ParametricSurface3dElementCoordinateSystem.cxx
// Created:	Thu Dec 12 17:51:07 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_ParametricSurface3dElementCoordinateSystem.ixx>

//=======================================================================
//function : StepFEA_ParametricSurface3dElementCoordinateSystem
//purpose  : 
//=======================================================================

StepFEA_ParametricSurface3dElementCoordinateSystem::StepFEA_ParametricSurface3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_ParametricSurface3dElementCoordinateSystem::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                                               const Standard_Integer aAxis,
                                                               const Standard_Real aAngle)
{
  StepFEA_FeaRepresentationItem::Init(aRepresentationItem_Name);

  theAxis = aAxis;

  theAngle = aAngle;
}

//=======================================================================
//function : Axis
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_ParametricSurface3dElementCoordinateSystem::Axis () const
{
  return theAxis;
}

//=======================================================================
//function : SetAxis
//purpose  : 
//=======================================================================

void StepFEA_ParametricSurface3dElementCoordinateSystem::SetAxis (const Standard_Integer aAxis)
{
  theAxis = aAxis;
}

//=======================================================================
//function : Angle
//purpose  : 
//=======================================================================

Standard_Real StepFEA_ParametricSurface3dElementCoordinateSystem::Angle () const
{
  return theAngle;
}

//=======================================================================
//function : SetAngle
//purpose  : 
//=======================================================================

void StepFEA_ParametricSurface3dElementCoordinateSystem::SetAngle (const Standard_Real aAngle)
{
  theAngle = aAngle;
}
