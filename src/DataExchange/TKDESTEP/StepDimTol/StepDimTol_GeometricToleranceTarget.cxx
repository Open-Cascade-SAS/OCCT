// Created on: 2015-07-20
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StepDimTol_GeometricToleranceTarget.hxx>
#include <MoniTool_Macros.hxx>
#include <StepShape_DimensionalLocation.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_ShapeAspect.hxx>

//=================================================================================================

StepDimTol_GeometricToleranceTarget::StepDimTol_GeometricToleranceTarget() {}

//=================================================================================================

int StepDimTol_GeometricToleranceTarget::CaseNum(const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalSize)))
    return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape)))
    return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect)))
    return 4;
  return 0;
}

occ::handle<StepShape_DimensionalLocation> StepDimTol_GeometricToleranceTarget::
  DimensionalLocation() const
{
  return GetCasted(StepShape_DimensionalLocation, Value());
}

occ::handle<StepShape_DimensionalSize> StepDimTol_GeometricToleranceTarget::DimensionalSize() const
{
  return GetCasted(StepShape_DimensionalSize, Value());
}

occ::handle<StepRepr_ProductDefinitionShape> StepDimTol_GeometricToleranceTarget::
  ProductDefinitionShape() const
{
  return GetCasted(StepRepr_ProductDefinitionShape, Value());
}

occ::handle<StepRepr_ShapeAspect> StepDimTol_GeometricToleranceTarget::ShapeAspect() const
{
  return GetCasted(StepRepr_ShapeAspect, Value());
}
