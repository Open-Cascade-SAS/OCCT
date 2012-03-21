// Created on: 2002-12-26
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
