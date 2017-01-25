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


#include <Standard_Type.hxx>
#include <StepVisual_CameraModelD3MultiClippingIntersection.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_CameraModelD3MultiClippingIntersection, StepGeom_GeometricRepresentationItem)

//=======================================================================
//function : StepVisual_CameraModelD3MultiClippingIntersection
//purpose  : 
//=======================================================================
StepVisual_CameraModelD3MultiClippingIntersection::StepVisual_CameraModelD3MultiClippingIntersection()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void StepVisual_CameraModelD3MultiClippingIntersection::Init(
  const Handle(TCollection_HAsciiString)& theName,
  const Handle(StepVisual_HArray1OfCameraModelD3MultiClippingInterectionSelect)& theShapeClipping)
{
  // Own field
  myShapeClipping = theShapeClipping;
  // Inherited fields
  StepGeom_GeometricRepresentationItem::Init(theName);
}
