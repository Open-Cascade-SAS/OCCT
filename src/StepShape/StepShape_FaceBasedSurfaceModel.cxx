// Created on: 2001-12-28
// Created by: Andrey BETENEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Type.hxx>
#include <StepShape_FaceBasedSurfaceModel.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_FaceBasedSurfaceModel,StepGeom_GeometricRepresentationItem)

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
