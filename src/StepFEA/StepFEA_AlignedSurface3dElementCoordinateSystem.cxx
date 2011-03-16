// File:	StepFEA_AlignedSurface3dElementCoordinateSystem.cxx
// Created:	Thu Dec 26 15:06:34 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_AlignedSurface3dElementCoordinateSystem.ixx>

//=======================================================================
//function : StepFEA_AlignedSurface3dElementCoordinateSystem
//purpose  : 
//=======================================================================

StepFEA_AlignedSurface3dElementCoordinateSystem::StepFEA_AlignedSurface3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_AlignedSurface3dElementCoordinateSystem::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                                            const Handle(StepFEA_FeaAxis2Placement3d) &aCoordinateSystem)
{
  StepFEA_FeaRepresentationItem::Init(aRepresentationItem_Name);

  theCoordinateSystem = aCoordinateSystem;
}

//=======================================================================
//function : CoordinateSystem
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaAxis2Placement3d) StepFEA_AlignedSurface3dElementCoordinateSystem::CoordinateSystem () const
{
  return theCoordinateSystem;
}

//=======================================================================
//function : SetCoordinateSystem
//purpose  : 
//=======================================================================

void StepFEA_AlignedSurface3dElementCoordinateSystem::SetCoordinateSystem (const Handle(StepFEA_FeaAxis2Placement3d) &aCoordinateSystem)
{
  theCoordinateSystem = aCoordinateSystem;
}
