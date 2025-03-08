// Created on : Sat May 02 12:41:14 2020
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include <StepRepr_RepresentationOrRepresentationReference.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationReference.hxx>

//=================================================================================================

StepRepr_RepresentationOrRepresentationReference::StepRepr_RepresentationOrRepresentationReference()
{
}

//=================================================================================================

Standard_Integer StepRepr_RepresentationOrRepresentationReference::CaseNum(
  const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationReference)))
    return 2;
  return 0;
}

//=================================================================================================

Handle(StepRepr_Representation) StepRepr_RepresentationOrRepresentationReference::Representation()
  const
{
  return Handle(StepRepr_Representation)::DownCast(Value());
}

//=================================================================================================

Handle(StepRepr_RepresentationReference) StepRepr_RepresentationOrRepresentationReference::
  RepresentationReference() const
{
  return Handle(StepRepr_RepresentationReference)::DownCast(Value());
}
