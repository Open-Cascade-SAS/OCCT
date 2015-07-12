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
#include <StepGeom_CompositeCurveOnSurface.hxx>
#include <StepGeom_CurveOnSurface.hxx>
#include <StepGeom_Pcurve.hxx>
#include <StepGeom_SurfaceCurve.hxx>

StepGeom_CurveOnSurface::StepGeom_CurveOnSurface () {  }

Standard_Integer StepGeom_CurveOnSurface::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_Pcurve))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_SurfaceCurve))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepGeom_CompositeCurveOnSurface))) return 3;
	return 0;
}

Handle(StepGeom_Pcurve) StepGeom_CurveOnSurface::Pcurve () const
{
	return GetCasted(StepGeom_Pcurve,Value());
}

Handle(StepGeom_SurfaceCurve) StepGeom_CurveOnSurface::SurfaceCurve () const
{
	return GetCasted(StepGeom_SurfaceCurve,Value());
}

Handle(StepGeom_CompositeCurveOnSurface) StepGeom_CurveOnSurface::CompositeCurveOnSurface () const
{
	return GetCasted(StepGeom_CompositeCurveOnSurface,Value());
}
