// Created on: 1998-08-04
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _StepAP214_AutoDesignReferencingItem_HeaderFile
#define _StepAP214_AutoDesignReferencingItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_Approval;
class StepBasic_DocumentRelationship;
class StepRepr_ExternallyDefinedRepresentation;
class StepRepr_MappedItem;
class StepRepr_MaterialDesignation;
class StepVisual_PresentationArea;
class StepVisual_PresentationView;
class StepBasic_ProductCategory;
class StepBasic_ProductDefinition;
class StepBasic_ProductDefinitionRelationship;
class StepRepr_PropertyDefinition;
class StepRepr_Representation;
class StepRepr_RepresentationRelationship;
class StepRepr_ShapeAspect;

class StepAP214_AutoDesignReferencingItem : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a AutoDesignReferencingItem SelectType
  Standard_EXPORT StepAP214_AutoDesignReferencingItem();

  //! Recognizes a AutoDesignReferencingItem Kind Entity that is :
  //! 1     Approval from StepBasic,
  //! 2     DocumentRelationship from StepBasic,
  //! 3     ExternallyDefinedRepresentation from StepRepr,
  //! 4     MappedItem from StepRepr,
  //! 5     MaterialDesignation from StepRepr,
  //! 6     PresentationArea from StepVisual,
  //! 7     PresentationView from StepVisual,
  //! 8     ProductCategory from StepBasic,
  //! 9     ProductDefinition from StepBasic,
  //! 10     ProductDefinitionRelationship from StepBasic,
  //! 11     PropertyDefinition from StepBasic,
  //! 12     Representation from StepRepr,
  //! 13     RepresentationRelationship from StepRepr,
  //! 14     ShapeAspect from StepRepr
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  Standard_EXPORT Handle(StepBasic_Approval) Approval() const;

  Standard_EXPORT Handle(StepBasic_DocumentRelationship) DocumentRelationship() const;

  Standard_EXPORT Handle(StepRepr_ExternallyDefinedRepresentation) ExternallyDefinedRepresentation()
    const;

  Standard_EXPORT Handle(StepRepr_MappedItem) MappedItem() const;

  Standard_EXPORT Handle(StepRepr_MaterialDesignation) MaterialDesignation() const;

  Standard_EXPORT Handle(StepVisual_PresentationArea) PresentationArea() const;

  Standard_EXPORT Handle(StepVisual_PresentationView) PresentationView() const;

  Standard_EXPORT Handle(StepBasic_ProductCategory) ProductCategory() const;

  Standard_EXPORT Handle(StepBasic_ProductDefinition) ProductDefinition() const;

  Standard_EXPORT Handle(StepBasic_ProductDefinitionRelationship) ProductDefinitionRelationship()
    const;

  Standard_EXPORT Handle(StepRepr_PropertyDefinition) PropertyDefinition() const;

  Standard_EXPORT Handle(StepRepr_Representation) Representation() const;

  Standard_EXPORT Handle(StepRepr_RepresentationRelationship) RepresentationRelationship() const;

  Standard_EXPORT Handle(StepRepr_ShapeAspect) ShapeAspect() const;

protected:
private:
};

#endif // _StepAP214_AutoDesignReferencingItem_HeaderFile
