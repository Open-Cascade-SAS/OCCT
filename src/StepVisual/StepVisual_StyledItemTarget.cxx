// Created on: 2016-03-18
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <StepVisual_StyledItemTarget.hxx>
#include <Interface_Macros.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_Representation.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>

//=======================================================================
//function : StepVisual_StyledItemTarget
//purpose  : 
//=======================================================================

StepVisual_StyledItemTarget::StepVisual_StyledItemTarget () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_StyledItemTarget::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_MappedItem))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepShape_TopologicalRepresentationItem))) return 4;
  return 0;
}

Handle(StepGeom_GeometricRepresentationItem) StepVisual_StyledItemTarget::GeometricRepresentationItem() const
{
  return GetCasted(StepGeom_GeometricRepresentationItem,Value());
}

Handle(StepRepr_MappedItem) StepVisual_StyledItemTarget::MappedItem() const
{
  return GetCasted(StepRepr_MappedItem,Value());
}

Handle(StepRepr_Representation) StepVisual_StyledItemTarget::Representation() const
{
  return GetCasted(StepRepr_Representation,Value());
}

Handle(StepShape_TopologicalRepresentationItem) StepVisual_StyledItemTarget::TopologicalRepresentationItem() const
{
  return GetCasted(StepShape_TopologicalRepresentationItem,Value());
}
