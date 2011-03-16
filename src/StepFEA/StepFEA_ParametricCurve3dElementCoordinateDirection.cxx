// File:	StepFEA_ParametricCurve3dElementCoordinateDirection.cxx
// Created:	Thu Dec 12 17:51:07 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_ParametricCurve3dElementCoordinateDirection.ixx>

//=======================================================================
//function : StepFEA_ParametricCurve3dElementCoordinateDirection
//purpose  : 
//=======================================================================

StepFEA_ParametricCurve3dElementCoordinateDirection::StepFEA_ParametricCurve3dElementCoordinateDirection ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_ParametricCurve3dElementCoordinateDirection::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                                                const Handle(StepGeom_Direction) &aOrientation)
{
  StepFEA_FeaRepresentationItem::Init(aRepresentationItem_Name);

  theOrientation = aOrientation;
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

Handle(StepGeom_Direction) StepFEA_ParametricCurve3dElementCoordinateDirection::Orientation () const
{
  return theOrientation;
}

//=======================================================================
//function : SetOrientation
//purpose  : 
//=======================================================================

void StepFEA_ParametricCurve3dElementCoordinateDirection::SetOrientation (const Handle(StepGeom_Direction) &aOrientation)
{
  theOrientation = aOrientation;
}
