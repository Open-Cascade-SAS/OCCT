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

#ifndef _StepVisual_SurfaceStyleControlGrid_HeaderFile
#define _StepVisual_SurfaceStyleControlGrid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepVisual_CurveStyle;

class StepVisual_SurfaceStyleControlGrid;
DEFINE_STANDARD_HANDLE(StepVisual_SurfaceStyleControlGrid, Standard_Transient)

class StepVisual_SurfaceStyleControlGrid : public Standard_Transient
{

public:
  //! Returns a SurfaceStyleControlGrid
  Standard_EXPORT StepVisual_SurfaceStyleControlGrid();

  Standard_EXPORT void Init(const Handle(StepVisual_CurveStyle)& aStyleOfControlGrid);

  Standard_EXPORT void SetStyleOfControlGrid(
    const Handle(StepVisual_CurveStyle)& aStyleOfControlGrid);

  Standard_EXPORT Handle(StepVisual_CurveStyle) StyleOfControlGrid() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_SurfaceStyleControlGrid, Standard_Transient)

protected:
private:
  Handle(StepVisual_CurveStyle) styleOfControlGrid;
};

#endif // _StepVisual_SurfaceStyleControlGrid_HeaderFile
