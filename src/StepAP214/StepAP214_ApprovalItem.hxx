// Created on: 1999-03-09
// Created by: data exchange team
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

#ifndef _StepAP214_ApprovalItem_HeaderFile
#define _StepAP214_ApprovalItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepRepr_AssemblyComponentUsageSubstitute;
class StepBasic_DocumentFile;
class StepRepr_MaterialDesignation;
class StepVisual_MechanicalDesignGeometricPresentationRepresentation;
class StepVisual_PresentationArea;
class StepBasic_Product;
class StepBasic_ProductDefinition;
class StepBasic_ProductDefinitionFormation;
class StepBasic_ProductDefinitionRelationship;
class StepRepr_PropertyDefinition;
class StepShape_ShapeRepresentation;
class StepBasic_SecurityClassification;
class StepRepr_ConfigurationItem;
class StepBasic_Date;
class StepBasic_Document;
class StepBasic_Effectivity;
class StepBasic_Group;
class StepBasic_GroupRelationship;
class StepBasic_ProductDefinitionFormationRelationship;
class StepRepr_Representation;
class StepRepr_ShapeAspectRelationship;

class StepAP214_ApprovalItem  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a ApprovalItem SelectType
  Standard_EXPORT StepAP214_ApprovalItem();
  
  //! Recognizes a ApprovalItem Kind Entity that is :
  //! 1 -> AssemblyComponentUsageSubstitute
  //! 2 -> DocumentFile
  //! 3 -> MaterialDesignation
  //! 4 -> MechanicalDesignGeometricPresentationRepresentation
  //! 5 -> PresentationArea
  //! 6 -> Product
  //! 7 -> ProductDefinition
  //! 8 -> ProductDefinitionFormation
  //! 9 -> ProductDefinitionRelationship
  //! 10 -> PropertyDefinition
  //! 11 -> ShapeRepresentation
  //! 12 -> SecurityClassification
  //! 13 -> ConfigurationItem
  //! 14 -> Date
  //! 15 -> Document
  //! 16 -> Effectivity
  //! 17 -> Group
  //! 18 -> GroupRelationship
  //! 19 -> ProductDefinitionFormationRelationship
  //! 20 -> Representation
  //! 21 -> ShapeAspectRelationship
  //! 0 else
  Standard_EXPORT virtual Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! returns Value as a AssemblyComponentUsageSubstitute (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_AssemblyComponentUsageSubstitute) AssemblyComponentUsageSubstitute() const;
  
  //! returns Value as a DocumentFile (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_DocumentFile) DocumentFile() const;
  
  //! returns Value as a MaterialDesignation (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_MaterialDesignation) MaterialDesignation() const;
  
  //! returns Value as a MechanicalDesignGeometricPresentationRepresentation (Null if another type)
  Standard_EXPORT virtual Handle(StepVisual_MechanicalDesignGeometricPresentationRepresentation) MechanicalDesignGeometricPresentationRepresentation() const;
  
  //! returns Value as a PresentationArea (Null if another type)
  Standard_EXPORT virtual Handle(StepVisual_PresentationArea) PresentationArea() const;
  
  //! returns Value as a Product (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_Product) Product() const;
  
  //! returns Value as a ProductDefinition (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_ProductDefinition) ProductDefinition() const;
  
  //! returns Value as a ProductDefinitionFormation (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_ProductDefinitionFormation) ProductDefinitionFormation() const;
  
  //! returns Value as aProductDefinitionRelationship (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_ProductDefinitionRelationship) ProductDefinitionRelationship() const;
  
  //! returns Value as a PropertyDefinition (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_PropertyDefinition) PropertyDefinition() const;
  
  //! returns Value as a ShapeRepresentation  (Null if another type)
  Standard_EXPORT virtual Handle(StepShape_ShapeRepresentation) ShapeRepresentation() const;
  
  //! returns Value as a SecurityClassification (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_SecurityClassification) SecurityClassification() const;

  //! returns Value as a ConfigurationItem (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_ConfigurationItem) ConfigurationItem() const;

  //! returns Value as a Date (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_Date) Date() const;

  //! returns Value as a Document (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_Document) Document() const;

  //! returns Value as a Effectivity (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_Effectivity) Effectivity() const;

  //! returns Value as a Group (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_Group) Group() const;

  //! returns Value as a GroupRelationship (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_GroupRelationship) GroupRelationship() const;

  //! returns Value as a ProductDefinitionFormationRelationship (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_ProductDefinitionFormationRelationship) ProductDefinitionFormationRelationship() const;

  //! returns Value as a Representation (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_Representation) Representation() const;

  //! returns Value as a ShapeAspectRelationship (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_ShapeAspectRelationship) ShapeAspectRelationship() const;

protected:





private:





};







#endif // _StepAP214_ApprovalItem_HeaderFile
