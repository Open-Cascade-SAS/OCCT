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
#include <StepAP214_AutoDesignGroupedItem.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepShape_CsgShapeRepresentation.hxx>
#include <StepShape_FacetedBrepShapeRepresentation.hxx>
#include <StepShape_GeometricallyBoundedSurfaceShapeRepresentation.hxx>
#include <StepShape_GeometricallyBoundedWireframeShapeRepresentation.hxx>
#include <StepShape_ManifoldSurfaceShapeRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepVisual_TemplateInstance.hxx>

StepAP214_AutoDesignGroupedItem::StepAP214_AutoDesignGroupedItem() = default;

int StepAP214_AutoDesignGroupedItem::CaseNum(const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepShape_AdvancedBrepShapeRepresentation)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_CsgShapeRepresentation)))
    return 2;
  if (ent->IsKind(STANDARD_TYPE(StepShape_FacetedBrepShapeRepresentation)))
    return 3;
  if (ent->IsKind(STANDARD_TYPE(StepShape_GeometricallyBoundedSurfaceShapeRepresentation)))
    return 4;
  if (ent->IsKind(STANDARD_TYPE(StepShape_GeometricallyBoundedWireframeShapeRepresentation)))
    return 5;
  if (ent->IsKind(STANDARD_TYPE(StepShape_ManifoldSurfaceShapeRepresentation)))
    return 6;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation)))
    return 7;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationItem)))
    return 8;
  if (ent->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation)))
    return 9;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect)))
    return 10;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_TemplateInstance)))
    return 11;
  return 0;
}

occ::handle<StepShape_AdvancedBrepShapeRepresentation> StepAP214_AutoDesignGroupedItem::
  AdvancedBrepShapeRepresentation() const
{
  return GetCasted(StepShape_AdvancedBrepShapeRepresentation, Value());
}

occ::handle<StepShape_CsgShapeRepresentation> StepAP214_AutoDesignGroupedItem::
  CsgShapeRepresentation() const
{
  return GetCasted(StepShape_CsgShapeRepresentation, Value());
}

occ::handle<StepShape_FacetedBrepShapeRepresentation> StepAP214_AutoDesignGroupedItem::
  FacetedBrepShapeRepresentation() const
{
  return GetCasted(StepShape_FacetedBrepShapeRepresentation, Value());
}

occ::handle<StepShape_GeometricallyBoundedSurfaceShapeRepresentation>
  StepAP214_AutoDesignGroupedItem::GeometricallyBoundedSurfaceShapeRepresentation() const
{
  return GetCasted(StepShape_GeometricallyBoundedSurfaceShapeRepresentation, Value());
}

occ::handle<StepShape_GeometricallyBoundedWireframeShapeRepresentation>
  StepAP214_AutoDesignGroupedItem::GeometricallyBoundedWireframeShapeRepresentation() const
{
  return GetCasted(StepShape_GeometricallyBoundedWireframeShapeRepresentation, Value());
}

occ::handle<StepShape_ManifoldSurfaceShapeRepresentation> StepAP214_AutoDesignGroupedItem::
  ManifoldSurfaceShapeRepresentation() const
{
  return GetCasted(StepShape_ManifoldSurfaceShapeRepresentation, Value());
}

occ::handle<StepRepr_Representation> StepAP214_AutoDesignGroupedItem::Representation() const
{
  return GetCasted(StepRepr_Representation, Value());
}

occ::handle<StepRepr_RepresentationItem> StepAP214_AutoDesignGroupedItem::RepresentationItem() const
{
  return GetCasted(StepRepr_RepresentationItem, Value());
}

occ::handle<StepRepr_ShapeAspect> StepAP214_AutoDesignGroupedItem::ShapeAspect() const
{
  return GetCasted(StepRepr_ShapeAspect, Value());
}

occ::handle<StepShape_ShapeRepresentation> StepAP214_AutoDesignGroupedItem::ShapeRepresentation()
  const
{
  return GetCasted(StepShape_ShapeRepresentation, Value());
}

occ::handle<StepVisual_TemplateInstance> StepAP214_AutoDesignGroupedItem::TemplateInstance() const
{
  return GetCasted(StepVisual_TemplateInstance, Value());
}
