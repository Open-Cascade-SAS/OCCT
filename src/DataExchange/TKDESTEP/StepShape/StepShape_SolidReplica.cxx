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

#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepShape_SolidReplica.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_SolidReplica, StepShape_SolidModel)

StepShape_SolidReplica::StepShape_SolidReplica() = default;

void StepShape_SolidReplica::Init(
  const occ::handle<TCollection_HAsciiString>&                   aName,
  const occ::handle<StepShape_SolidModel>&                       aParentSolid,
  const occ::handle<StepGeom_CartesianTransformationOperator3d>& aTransformation)
{
  // --- classe own fields ---
  parentSolid    = aParentSolid;
  transformation = aTransformation;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepShape_SolidReplica::SetParentSolid(const occ::handle<StepShape_SolidModel>& aParentSolid)
{
  parentSolid = aParentSolid;
}

occ::handle<StepShape_SolidModel> StepShape_SolidReplica::ParentSolid() const
{
  return parentSolid;
}

void StepShape_SolidReplica::SetTransformation(
  const occ::handle<StepGeom_CartesianTransformationOperator3d>& aTransformation)
{
  transformation = aTransformation;
}

occ::handle<StepGeom_CartesianTransformationOperator3d> StepShape_SolidReplica::Transformation()
  const
{
  return transformation;
}
