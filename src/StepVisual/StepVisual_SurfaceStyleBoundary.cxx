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

#include <StepVisual_CurveStyle.hxx>
#include <StepVisual_SurfaceStyleBoundary.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceStyleBoundary, Standard_Transient)

StepVisual_SurfaceStyleBoundary::StepVisual_SurfaceStyleBoundary() {}

void StepVisual_SurfaceStyleBoundary::Init(const Handle(StepVisual_CurveStyle)& aStyleOfBoundary)
{
  // --- classe own fields ---
  styleOfBoundary = aStyleOfBoundary;
}

void StepVisual_SurfaceStyleBoundary::SetStyleOfBoundary(
  const Handle(StepVisual_CurveStyle)& aStyleOfBoundary)
{
  styleOfBoundary = aStyleOfBoundary;
}

Handle(StepVisual_CurveStyle) StepVisual_SurfaceStyleBoundary::StyleOfBoundary() const
{
  return styleOfBoundary;
}
