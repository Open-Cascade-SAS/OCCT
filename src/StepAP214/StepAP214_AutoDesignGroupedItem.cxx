// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <StepAP214_AutoDesignGroupedItem.ixx>
#include <Interface_Macros.hxx>

StepAP214_AutoDesignGroupedItem::StepAP214_AutoDesignGroupedItem () {  }

Standard_Integer StepAP214_AutoDesignGroupedItem::CaseNum(const Handle(Standard_Transient)& ent) const
{
	if (ent.IsNull()) return 0;
	if (ent->IsKind(STANDARD_TYPE(StepShape_AdvancedBrepShapeRepresentation))) return 1;
	if (ent->IsKind(STANDARD_TYPE(StepShape_CsgShapeRepresentation))) return 2;
	if (ent->IsKind(STANDARD_TYPE(StepShape_FacetedBrepShapeRepresentation))) return 3;
	if (ent->IsKind(STANDARD_TYPE(StepShape_GeometricallyBoundedSurfaceShapeRepresentation))) return 4;
	if (ent->IsKind(STANDARD_TYPE(StepShape_GeometricallyBoundedWireframeShapeRepresentation))) return 5;
	if (ent->IsKind(STANDARD_TYPE(StepShape_ManifoldSurfaceShapeRepresentation))) return 6;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 7;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationItem))) return 8;
	if (ent->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) return 9;
	if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 10;
	if (ent->IsKind(STANDARD_TYPE(StepVisual_TemplateInstance))) return 11;
	return 0;
}

Handle(StepShape_AdvancedBrepShapeRepresentation) StepAP214_AutoDesignGroupedItem::AdvancedBrepShapeRepresentation () const
{
	return GetCasted(StepShape_AdvancedBrepShapeRepresentation,Value());
}

Handle(StepShape_CsgShapeRepresentation) StepAP214_AutoDesignGroupedItem::CsgShapeRepresentation () const
{
	return GetCasted(StepShape_CsgShapeRepresentation,Value());
}

Handle(StepShape_FacetedBrepShapeRepresentation) StepAP214_AutoDesignGroupedItem::FacetedBrepShapeRepresentation () const
{
	return GetCasted(StepShape_FacetedBrepShapeRepresentation,Value());
}

Handle(StepShape_GeometricallyBoundedSurfaceShapeRepresentation) StepAP214_AutoDesignGroupedItem::GeometricallyBoundedSurfaceShapeRepresentation () const
{
	return GetCasted(StepShape_GeometricallyBoundedSurfaceShapeRepresentation,Value());
}

Handle(StepShape_GeometricallyBoundedWireframeShapeRepresentation) StepAP214_AutoDesignGroupedItem::GeometricallyBoundedWireframeShapeRepresentation () const
{
	return GetCasted(StepShape_GeometricallyBoundedWireframeShapeRepresentation,Value());
}

Handle(StepShape_ManifoldSurfaceShapeRepresentation) StepAP214_AutoDesignGroupedItem::ManifoldSurfaceShapeRepresentation () const
{
	return GetCasted(StepShape_ManifoldSurfaceShapeRepresentation,Value());
}

Handle(StepRepr_Representation) StepAP214_AutoDesignGroupedItem::Representation () const
{
	return GetCasted(StepRepr_Representation,Value());
}

Handle(StepRepr_RepresentationItem) StepAP214_AutoDesignGroupedItem::RepresentationItem () const
{
	return GetCasted(StepRepr_RepresentationItem,Value());
}

Handle(StepRepr_ShapeAspect) StepAP214_AutoDesignGroupedItem::ShapeAspect () const
{
	return GetCasted(StepRepr_ShapeAspect,Value());
}

Handle(StepShape_ShapeRepresentation) StepAP214_AutoDesignGroupedItem::ShapeRepresentation () const
{
	return GetCasted(StepShape_ShapeRepresentation,Value());
}

Handle(StepVisual_TemplateInstance) StepAP214_AutoDesignGroupedItem::TemplateInstance () const
{
	return GetCasted(StepVisual_TemplateInstance,Value());
}
