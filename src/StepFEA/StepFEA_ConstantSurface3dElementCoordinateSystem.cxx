// File:	StepFEA_ConstantSurface3dElementCoordinateSystem.cxx
// Created:	Thu Dec 26 15:06:35 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_ConstantSurface3dElementCoordinateSystem.ixx>

//=======================================================================
//function : StepFEA_ConstantSurface3dElementCoordinateSystem
//purpose  : 
//=======================================================================

StepFEA_ConstantSurface3dElementCoordinateSystem::StepFEA_ConstantSurface3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_ConstantSurface3dElementCoordinateSystem::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
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

Standard_Integer StepFEA_ConstantSurface3dElementCoordinateSystem::Axis () const
{
  return theAxis;
}

//=======================================================================
//function : SetAxis
//purpose  : 
//=======================================================================

void StepFEA_ConstantSurface3dElementCoordinateSystem::SetAxis (const Standard_Integer aAxis)
{
  theAxis = aAxis;
}

//=======================================================================
//function : Angle
//purpose  : 
//=======================================================================

Standard_Real StepFEA_ConstantSurface3dElementCoordinateSystem::Angle () const
{
  return theAngle;
}

//=======================================================================
//function : SetAngle
//purpose  : 
//=======================================================================

void StepFEA_ConstantSurface3dElementCoordinateSystem::SetAngle (const Standard_Real aAngle)
{
  theAngle = aAngle;
}
