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

#ifndef _StepShape_FaceBasedSurfaceModel_HeaderFile
#define _StepShape_FaceBasedSurfaceModel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_HArray1OfConnectedFaceSet.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
class TCollection_HAsciiString;

class StepShape_FaceBasedSurfaceModel;
DEFINE_STANDARD_HANDLE(StepShape_FaceBasedSurfaceModel, StepGeom_GeometricRepresentationItem)

//! Representation of STEP entity FaceBasedSurfaceModel
class StepShape_FaceBasedSurfaceModel : public StepGeom_GeometricRepresentationItem
{

public:
  //! Empty constructor
  Standard_EXPORT StepShape_FaceBasedSurfaceModel();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aRepresentationItem_Name,
                            const Handle(StepShape_HArray1OfConnectedFaceSet)& aFbsmFaces);

  //! Returns field FbsmFaces
  Standard_EXPORT Handle(StepShape_HArray1OfConnectedFaceSet) FbsmFaces() const;

  //! Set field FbsmFaces
  Standard_EXPORT void SetFbsmFaces(const Handle(StepShape_HArray1OfConnectedFaceSet)& FbsmFaces);

  DEFINE_STANDARD_RTTIEXT(StepShape_FaceBasedSurfaceModel, StepGeom_GeometricRepresentationItem)

protected:
private:
  Handle(StepShape_HArray1OfConnectedFaceSet) theFbsmFaces;
};

#endif // _StepShape_FaceBasedSurfaceModel_HeaderFile
