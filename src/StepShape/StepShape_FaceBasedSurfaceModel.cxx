// File:	StepShape_FaceBasedSurfaceModel.cxx
// Created:	Fri Dec 28 16:02:01 2001 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepShape_FaceBasedSurfaceModel.ixx>

//=======================================================================
//function : StepShape_FaceBasedSurfaceModel
//purpose  : 
//=======================================================================

StepShape_FaceBasedSurfaceModel::StepShape_FaceBasedSurfaceModel ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_FaceBasedSurfaceModel::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                            const Handle(StepShape_HArray1OfConnectedFaceSet) &aFbsmFaces)
{
  StepGeom_GeometricRepresentationItem::Init(aRepresentationItem_Name);

  theFbsmFaces = aFbsmFaces;
}

//=======================================================================
//function : FbsmFaces
//purpose  : 
//=======================================================================

Handle(StepShape_HArray1OfConnectedFaceSet) StepShape_FaceBasedSurfaceModel::FbsmFaces () const
{
  return theFbsmFaces;
}

//=======================================================================
//function : SetFbsmFaces
//purpose  : 
//=======================================================================

void StepShape_FaceBasedSurfaceModel::SetFbsmFaces (const Handle(StepShape_HArray1OfConnectedFaceSet) &aFbsmFaces)
{
  theFbsmFaces = aFbsmFaces;
}
