// File:	StepFEA_AlignedCurve3dElementCoordinateSystem.cxx
// Created:	Thu Dec 12 17:51:03 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_AlignedCurve3dElementCoordinateSystem.ixx>

//=======================================================================
//function : StepFEA_AlignedCurve3dElementCoordinateSystem
//purpose  : 
//=======================================================================

StepFEA_AlignedCurve3dElementCoordinateSystem::StepFEA_AlignedCurve3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_AlignedCurve3dElementCoordinateSystem::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                                          const Handle(StepFEA_FeaAxis2Placement3d) &aCoordinateSystem)
{
  StepFEA_FeaRepresentationItem::Init(aRepresentationItem_Name);

  theCoordinateSystem = aCoordinateSystem;
}

//=======================================================================
//function : CoordinateSystem
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaAxis2Placement3d) StepFEA_AlignedCurve3dElementCoordinateSystem::CoordinateSystem () const
{
  return theCoordinateSystem;
}

//=======================================================================
//function : SetCoordinateSystem
//purpose  : 
//=======================================================================

void StepFEA_AlignedCurve3dElementCoordinateSystem::SetCoordinateSystem (const Handle(StepFEA_FeaAxis2Placement3d) &aCoordinateSystem)
{
  theCoordinateSystem = aCoordinateSystem;
}
