// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StepVisual_SurfaceStyleSilhouette_HeaderFile
#define _StepVisual_SurfaceStyleSilhouette_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepVisual_CurveStyle;

class StepVisual_SurfaceStyleSilhouette;
DEFINE_STANDARD_HANDLE(StepVisual_SurfaceStyleSilhouette, Standard_Transient)

class StepVisual_SurfaceStyleSilhouette : public Standard_Transient
{

public:
  //! Returns a SurfaceStyleSilhouette
  Standard_EXPORT StepVisual_SurfaceStyleSilhouette();

  Standard_EXPORT void Init(const Handle(StepVisual_CurveStyle)& aStyleOfSilhouette);

  Standard_EXPORT void SetStyleOfSilhouette(
    const Handle(StepVisual_CurveStyle)& aStyleOfSilhouette);

  Standard_EXPORT Handle(StepVisual_CurveStyle) StyleOfSilhouette() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_SurfaceStyleSilhouette, Standard_Transient)

protected:
private:
  Handle(StepVisual_CurveStyle) styleOfSilhouette;
};

#endif // _StepVisual_SurfaceStyleSilhouette_HeaderFile
