// Created on : Thu Mar 24 18:30:12 2022 
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#include <StepVisual_EdgeOrCurve.hxx>
#include <StepGeom_Curve.hxx>
#include <StepShape_Edge.hxx>

//=======================================================================
//function : StepVisual_EdgeOrCurve
//purpose  : 
//=======================================================================

StepVisual_EdgeOrCurve::StepVisual_EdgeOrCurve ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_EdgeOrCurve::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_Curve))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_Edge))) return 2;
  return 0;
}

//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Handle(StepGeom_Curve) StepVisual_EdgeOrCurve::Curve () const
{
  return Handle(StepGeom_Curve)::DownCast(Value());
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

Handle(StepShape_Edge) StepVisual_EdgeOrCurve::Edge () const
{
  return Handle(StepShape_Edge)::DownCast(Value());
}
