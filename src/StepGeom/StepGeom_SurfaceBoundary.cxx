// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Standard_Transient.hxx>
#include <StepGeom_BoundaryCurve.hxx>
#include <StepGeom_DegeneratePcurve.hxx>
#include <StepGeom_SurfaceBoundary.hxx>

//=======================================================================
//function : StepGeom_SurfaceBoundary
//purpose  : 
//=======================================================================
StepGeom_SurfaceBoundary::StepGeom_SurfaceBoundary ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepGeom_SurfaceBoundary::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_BoundaryCurve))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_DegeneratePcurve))) return 2;
  return 0;
}

//=======================================================================
//function : BoundaryCurve
//purpose  : 
//=======================================================================

Handle(StepGeom_BoundaryCurve) StepGeom_SurfaceBoundary::BoundaryCurve () const
{
  return Handle(StepGeom_BoundaryCurve)::DownCast(Value());
}

//=======================================================================
//function : DegeneratePcurve
//purpose  : 
//=======================================================================

Handle(StepGeom_DegeneratePcurve) StepGeom_SurfaceBoundary::DegeneratePcurve () const
{
  return Handle(StepGeom_DegeneratePcurve)::DownCast(Value());
}
