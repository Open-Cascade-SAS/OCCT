// Created on: 2016-10-25
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

#include <StepGeom_Axis2Placement3d.hxx>
#include <StepVisual_CameraModelD3MultiClipping.hxx>
#include <StepVisual_ViewVolume.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_CameraModelD3MultiClipping, StepVisual_CameraModelD3)

//=================================================================================================

StepVisual_CameraModelD3MultiClipping::StepVisual_CameraModelD3MultiClipping() {}

//=================================================================================================

void StepVisual_CameraModelD3MultiClipping::Init(
  const Handle(TCollection_HAsciiString)&  theName,
  const Handle(StepGeom_Axis2Placement3d)& theViewReferenceSystem,
  const Handle(StepVisual_ViewVolume)&     thePerspectiveOfVolume,
  const Handle(StepVisual_HArray1OfCameraModelD3MultiClippingInterectionSelect)& theShapeClipping)
{
  // Own field
  myShapeClipping = theShapeClipping;
  // Inherited fields
  StepVisual_CameraModelD3::Init(theName, theViewReferenceSystem, thePerspectiveOfVolume);
}
