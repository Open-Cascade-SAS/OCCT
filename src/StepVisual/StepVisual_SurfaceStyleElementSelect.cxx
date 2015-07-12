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


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepVisual_SurfaceStyleBoundary.hxx>
#include <StepVisual_SurfaceStyleControlGrid.hxx>
#include <StepVisual_SurfaceStyleElementSelect.hxx>
#include <StepVisual_SurfaceStyleFillArea.hxx>
#include <StepVisual_SurfaceStyleParameterLine.hxx>
#include <StepVisual_SurfaceStyleSegmentationCurve.hxx>
#include <StepVisual_SurfaceStyleSilhouette.hxx>

StepVisual_SurfaceStyleElementSelect::StepVisual_SurfaceStyleElementSelect () {  }

Standard_Integer StepVisual_SurfaceStyleElementSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_SurfaceStyleFillArea))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_SurfaceStyleBoundary))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_SurfaceStyleParameterLine))) return 3;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_SurfaceStyleSilhouette))) return 4;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_SurfaceStyleSegmentationCurve))) return 5;
//	if (ent->IsKind(STANDARD_TYPE(StepVisual_SurfaceStyleControlGrid))) return 6;
	return 0;
}

Handle(StepVisual_SurfaceStyleFillArea) StepVisual_SurfaceStyleElementSelect::SurfaceStyleFillArea () const
{
	return GetCasted(StepVisual_SurfaceStyleFillArea,Value());
}

Handle(StepVisual_SurfaceStyleBoundary) StepVisual_SurfaceStyleElementSelect::SurfaceStyleBoundary () const
{
	return GetCasted(StepVisual_SurfaceStyleBoundary,Value());
}

Handle(StepVisual_SurfaceStyleParameterLine) StepVisual_SurfaceStyleElementSelect::SurfaceStyleParameterLine () const
{
	return GetCasted(StepVisual_SurfaceStyleParameterLine,Value());
}
