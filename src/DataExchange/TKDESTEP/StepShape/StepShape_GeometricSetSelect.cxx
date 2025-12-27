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

#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepGeom_Point.hxx>
#include <StepGeom_Surface.hxx>
#include <StepShape_GeometricSetSelect.hxx>

StepShape_GeometricSetSelect::StepShape_GeometricSetSelect() {}

int StepShape_GeometricSetSelect::CaseNum(const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_Point)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_Curve)))
    return 2;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_Surface)))
    return 3;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem)))
    return 4;
  return 0;
}

occ::handle<StepGeom_Point> StepShape_GeometricSetSelect::Point() const
{
  return GetCasted(StepGeom_Point, Value());
}

occ::handle<StepGeom_Curve> StepShape_GeometricSetSelect::Curve() const
{
  return GetCasted(StepGeom_Curve, Value());
}

occ::handle<StepGeom_Surface> StepShape_GeometricSetSelect::Surface() const
{
  return GetCasted(StepGeom_Surface, Value());
}
