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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepShape_RWFaceBasedSurfaceModel.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_FaceBasedSurfaceModel.hxx>
#include <StepShape_HArray1OfConnectedFaceSet.hxx>

//=======================================================================
//function : RWStepShape_RWFaceBasedSurfaceModel
//purpose  : 
//=======================================================================
RWStepShape_RWFaceBasedSurfaceModel::RWStepShape_RWFaceBasedSurfaceModel ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWFaceBasedSurfaceModel::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepShape_FaceBasedSurfaceModel) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"face_based_surface_model") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of FaceBasedSurfaceModel

  Handle(StepShape_HArray1OfConnectedFaceSet) aFbsmFaces;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "fbsm_faces", ach, sub2) ) {
    Standard_Integer num2 = sub2;
    Standard_Integer nb0 = data->NbParams(num2);
    aFbsmFaces = new StepShape_HArray1OfConnectedFaceSet (1, nb0);
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepShape_ConnectedFaceSet) anIt0;
      data->ReadEntity (num2, i0, "fbsm_faces", ach, STANDARD_TYPE(StepShape_ConnectedFaceSet), anIt0);
      aFbsmFaces->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aFbsmFaces);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWFaceBasedSurfaceModel::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepShape_FaceBasedSurfaceModel) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of FaceBasedSurfaceModel

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->FbsmFaces()->Length(); i1++ ) {
    Handle(StepShape_ConnectedFaceSet) Var0 = ent->FbsmFaces()->Value(i1);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWFaceBasedSurfaceModel::Share (const Handle(StepShape_FaceBasedSurfaceModel) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of FaceBasedSurfaceModel

  for (Standard_Integer i1=1; i1 <= ent->FbsmFaces()->Length(); i1++ ) {
    Handle(StepShape_ConnectedFaceSet) Var0 = ent->FbsmFaces()->Value(i1);
    iter.AddItem (Var0);
  }
}
