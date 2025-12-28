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

#ifndef _StepVisual_CameraModelD3MultiClippingUnion_HeaderFile
#define _StepVisual_CameraModelD3MultiClippingUnion_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepVisual_CameraModelD3MultiClippingUnionSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class TCollection_HAsciiString;

class StepVisual_CameraModelD3MultiClippingUnion : public StepGeom_GeometricRepresentationItem
{
public:
  //! Returns a StepVisual_CameraModelD3MultiClippingUnion
  Standard_EXPORT StepVisual_CameraModelD3MultiClippingUnion();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& theName,
    const occ::handle<NCollection_HArray1<StepVisual_CameraModelD3MultiClippingUnionSelect>>&
      theShapeClipping);

  void SetShapeClipping(
    const occ::handle<NCollection_HArray1<StepVisual_CameraModelD3MultiClippingUnionSelect>>&
      theShapeClipping)
  {
    myShapeClipping = theShapeClipping;
  }

  const occ::handle<NCollection_HArray1<StepVisual_CameraModelD3MultiClippingUnionSelect>>
    ShapeClipping()
  {
    return myShapeClipping;
  }
  DEFINE_STANDARD_RTTIEXT(StepVisual_CameraModelD3MultiClippingUnion,
                          StepGeom_GeometricRepresentationItem)

private:
  occ::handle<NCollection_HArray1<StepVisual_CameraModelD3MultiClippingUnionSelect>>
    myShapeClipping;
};
#endif // _StepVisual_CameraModelD3MultiClippingUnion_HeaderFile
