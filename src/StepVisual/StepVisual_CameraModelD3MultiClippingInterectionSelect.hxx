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

#ifndef _StepVisual_CameraModelD3MultiClippingInterectionSelect_HeaderFile
#define _StepVisual_CameraModelD3MultiClippingInterectionSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepGeom_Plane;
class StepVisual_CameraModelD3MultiClippingUnion;

class StepVisual_CameraModelD3MultiClippingInterectionSelect : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a CameraModelD3MultiClippingInterectionSelect select type
  Standard_EXPORT StepVisual_CameraModelD3MultiClippingInterectionSelect();

  //! Recognizes a IdAttributeSelect Kind Entity that is :
  //! 1 -> Plane
  //! 2 -> CameraModelD3MultiClippingUnion
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a Plane (Null if another type)
  Standard_EXPORT Handle(StepGeom_Plane) Plane() const;

  //! returns Value as a CameraModelD3MultiClippingUnion (Null if another type)
  Standard_EXPORT Handle(StepVisual_CameraModelD3MultiClippingUnion)
    CameraModelD3MultiClippingUnion() const;
};
#endif // StepVisual_CameraModelD3MultiClippingInterectionSelect
