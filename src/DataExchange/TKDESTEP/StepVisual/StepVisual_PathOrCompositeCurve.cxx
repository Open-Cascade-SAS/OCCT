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

#include <StepVisual_PathOrCompositeCurve.hxx>
#include <StepGeom_CompositeCurve.hxx>
#include <StepShape_Path.hxx>

//=================================================================================================

StepVisual_PathOrCompositeCurve::StepVisual_PathOrCompositeCurve() = default;

//=================================================================================================

int StepVisual_PathOrCompositeCurve::CaseNum(const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_CompositeCurve)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_Path)))
    return 2;
  return 0;
}

//=================================================================================================

occ::handle<StepGeom_CompositeCurve> StepVisual_PathOrCompositeCurve::CompositeCurve() const
{
  return occ::down_cast<StepGeom_CompositeCurve>(Value());
}

//=================================================================================================

occ::handle<StepShape_Path> StepVisual_PathOrCompositeCurve::Path() const
{
  return occ::down_cast<StepShape_Path>(Value());
}
