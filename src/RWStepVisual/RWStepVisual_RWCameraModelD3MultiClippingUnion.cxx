// Created on: 2016-11-14
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepVisual_RWCameraModelD3MultiClippingUnion.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_CameraModelD3MultiClippingUnion.hxx>
#include <StepVisual_HArray1OfCameraModelD3MultiClippingUnionSelect.hxx>

//=======================================================================
//function : RWStepVisual_RWCameraModelD3MultiClippingUnion
//purpose  : 
//=======================================================================
RWStepVisual_RWCameraModelD3MultiClippingUnion::RWStepVisual_RWCameraModelD3MultiClippingUnion() {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWCameraModelD3MultiClippingUnion::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepVisual_CameraModelD3MultiClippingUnion)& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 2, ach, "camera_model_d3_multi_clipping_union"))
    return;

  // Inherited field : name
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  // Own field : shape_clipping
  Handle(StepVisual_HArray1OfCameraModelD3MultiClippingUnionSelect) aShapeClipping;
  StepVisual_CameraModelD3MultiClippingUnionSelect anEnt;
  Standard_Integer nbSub;
  if (data->ReadSubList(num, 2, "shape_clipping", ach, nbSub)) {
    Standard_Integer nbElements = data->NbParams(nbSub);
    aShapeClipping = new StepVisual_HArray1OfCameraModelD3MultiClippingUnionSelect(1, nbElements);
    for (Standard_Integer i = 1; i <= nbElements; i++) {
      if (data->ReadEntity(nbSub, i, "shape_clipping", ach, anEnt))
        aShapeClipping->SetValue(i, anEnt);
    }
  }

  // Initialization of the read entity
  ent->Init(aName, aShapeClipping);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWCameraModelD3MultiClippingUnion::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepVisual_CameraModelD3MultiClippingUnion)& ent) const
{
  // Inherited field name
  SW.Send(ent->Name());

  // Own field: shape_clipping
  SW.OpenSub();
  for (Standard_Integer i = 1; i <= ent->ShapeClipping()->Length(); i++) {
    SW.Send(ent->ShapeClipping()->Value(i).Value());
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================
void RWStepVisual_RWCameraModelD3MultiClippingUnion::Share
  (const Handle(StepVisual_CameraModelD3MultiClippingUnion)& ent,
   Interface_EntityIterator& iter) const
{
  // Own field: shape_clipping
  Standard_Integer i, nb = ent->ShapeClipping()->Length();
  for (i = 1; i <= nb; i++)
    iter.AddItem(ent->ShapeClipping()->Value(i).Value());
}

