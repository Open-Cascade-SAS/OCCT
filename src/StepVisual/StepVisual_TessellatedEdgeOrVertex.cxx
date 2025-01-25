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

#include <StepVisual_TessellatedEdgeOrVertex.hxx>
#include <StepVisual_TessellatedEdge.hxx>
#include <StepVisual_TessellatedVertex.hxx>

//=================================================================================================

StepVisual_TessellatedEdgeOrVertex::StepVisual_TessellatedEdgeOrVertex() {}

//=================================================================================================

Standard_Integer StepVisual_TessellatedEdgeOrVertex::CaseNum(
  const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_TessellatedEdge)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_TessellatedVertex)))
    return 2;
  return 0;
}

//=================================================================================================

Handle(StepVisual_TessellatedEdge) StepVisual_TessellatedEdgeOrVertex::TessellatedEdge() const
{
  return Handle(StepVisual_TessellatedEdge)::DownCast(Value());
}

//=================================================================================================

Handle(StepVisual_TessellatedVertex) StepVisual_TessellatedEdgeOrVertex::TessellatedVertex() const
{
  return Handle(StepVisual_TessellatedVertex)::DownCast(Value());
}
