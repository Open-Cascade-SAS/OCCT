// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepAP214_AutoDesignGroupedItem_HeaderFile
#define _StepAP214_AutoDesignGroupedItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepShape_AdvancedBrepShapeRepresentation;
class StepShape_CsgShapeRepresentation;
class StepShape_FacetedBrepShapeRepresentation;
class StepShape_GeometricallyBoundedSurfaceShapeRepresentation;
class StepShape_GeometricallyBoundedWireframeShapeRepresentation;
class StepShape_ManifoldSurfaceShapeRepresentation;
class StepRepr_Representation;
class StepRepr_RepresentationItem;
class StepRepr_ShapeAspect;
class StepShape_ShapeRepresentation;
class StepVisual_TemplateInstance;

class StepAP214_AutoDesignGroupedItem : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a AutoDesignGroupedItem SelectType
  Standard_EXPORT StepAP214_AutoDesignGroupedItem();

  //! Recognizes a AutoDesignGroupedItem Kind Entity that is :
  //! 1 -> AdvancedBrepShapeRepresentation
  //! 2 -> CsgShapeRepresentation
  //! 3 -> FacetedBrepShapeRepresentation
  //! 4 -> GeometricallyBoundedSurfaceShapeRepresentation
  //! 5 -> GeometricallyBoundedWireframeShapeRepresentation
  //! 6 -> ManifoldSurfaceShapeRepresentation
  //! 7 -> Representation
  //! 8 -> RepresentationItem
  //! 9 -> ShapeAspect
  //! 10 -> ShapeRepresentation
  //! 11 -> TemplateInstance
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a AdvancedBrepShapeRepresentation (Null if another type)
  Standard_EXPORT Handle(StepShape_AdvancedBrepShapeRepresentation)
    AdvancedBrepShapeRepresentation() const;

  //! returns Value as a CsgShapeRepresentation (Null if another type)
  Standard_EXPORT Handle(StepShape_CsgShapeRepresentation) CsgShapeRepresentation() const;

  //! returns Value as a FacetedBrepShapeRepresentation (Null if another type)
  Standard_EXPORT Handle(StepShape_FacetedBrepShapeRepresentation) FacetedBrepShapeRepresentation()
    const;

  //! returns Value as a GeometricallyBoundedSurfaceShapeRepresentation (Null if another type)
  Standard_EXPORT Handle(StepShape_GeometricallyBoundedSurfaceShapeRepresentation)
    GeometricallyBoundedSurfaceShapeRepresentation() const;

  //! returns Value as a GeometricallyBoundedWireframeShapeRepresentation (Null if another type)
  Standard_EXPORT Handle(StepShape_GeometricallyBoundedWireframeShapeRepresentation)
    GeometricallyBoundedWireframeShapeRepresentation() const;

  //! returns Value as a ManifoldSurfaceShapeRepresentation (Null if another type)
  Standard_EXPORT Handle(StepShape_ManifoldSurfaceShapeRepresentation)
    ManifoldSurfaceShapeRepresentation() const;

  //! returns Value as a Representation (Null if another type)
  Standard_EXPORT Handle(StepRepr_Representation) Representation() const;

  //! returns Value as a RepresentationItem (Null if another type)
  Standard_EXPORT Handle(StepRepr_RepresentationItem) RepresentationItem() const;

  //! returns Value as a ShapeAspect (Null if another type)
  Standard_EXPORT Handle(StepRepr_ShapeAspect) ShapeAspect() const;

  //! returns Value as a ShapeRepresentation (Null if another type)
  Standard_EXPORT Handle(StepShape_ShapeRepresentation) ShapeRepresentation() const;

  //! returns Value as a TemplateInstance (Null if another type)
  Standard_EXPORT Handle(StepVisual_TemplateInstance) TemplateInstance() const;

protected:
private:
};

#endif // _StepAP214_AutoDesignGroupedItem_HeaderFile
