// Created on: 2015-07-10
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StepVisual_CameraModelD3MultiClippingInterectionSelect.hxx>
#include <Interface_Macros.hxx>
#include <StepGeom_Plane.hxx>
#include <StepVisual_CameraModelD3MultiClippingUnion.hxx>

//=======================================================================
//function : StepVisual_CameraModelD3MultiClippingInterectionSelect
//purpose  : 
//=======================================================================
StepVisual_CameraModelD3MultiClippingInterectionSelect::StepVisual_CameraModelD3MultiClippingInterectionSelect () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================
Standard_Integer StepVisual_CameraModelD3MultiClippingInterectionSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_Plane))) return 1;
  if (ent->IsInstance(STANDARD_TYPE(StepVisual_CameraModelD3MultiClippingUnion))) return 2;
  return 0;
}

Handle(StepGeom_Plane) StepVisual_CameraModelD3MultiClippingInterectionSelect::Plane() const
{  return GetCasted(StepGeom_Plane, Value());  }

Handle(StepVisual_CameraModelD3MultiClippingUnion) StepVisual_CameraModelD3MultiClippingInterectionSelect::CameraModelD3MultiClippingUnion() const
{
  return GetCasted(StepVisual_CameraModelD3MultiClippingUnion, Value());
}
