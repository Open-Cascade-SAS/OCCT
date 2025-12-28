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

//=================================================================================================

StepGeom_SurfaceBoundary::StepGeom_SurfaceBoundary() = default;

//=================================================================================================

int StepGeom_SurfaceBoundary::CaseNum(const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_BoundaryCurve)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_DegeneratePcurve)))
    return 2;
  return 0;
}

//=================================================================================================

occ::handle<StepGeom_BoundaryCurve> StepGeom_SurfaceBoundary::BoundaryCurve() const
{
  return occ::down_cast<StepGeom_BoundaryCurve>(Value());
}

//=================================================================================================

occ::handle<StepGeom_DegeneratePcurve> StepGeom_SurfaceBoundary::DegeneratePcurve() const
{
  return occ::down_cast<StepGeom_DegeneratePcurve>(Value());
}
