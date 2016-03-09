// Created on: 1999-03-10
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

#ifndef _StepAP214_SecurityClassificationItem_HeaderFile
#define _StepAP214_SecurityClassificationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepAP214_ApprovalItem.hxx>

class StepBasic_Action;
class StepRepr_AssemblyComponentUsage;
class StepRepr_ConfigurationDesign;
class StepRepr_ConfigurationEffectivity;
class StepVisual_DraughtingModel;
class StepBasic_GeneralProperty;
class StepRepr_MakeFromUsageOption;
class StepRepr_ProductConcept;
class StepRepr_ProductDefinitionUsage;
class StepBasic_VersionedActionRequest;

class StepAP214_SecurityClassificationItem  : public StepAP214_ApprovalItem
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a SecurityClassificationItem SelectType
  Standard_EXPORT StepAP214_SecurityClassificationItem();

  //! Recognizes a SecurityClassificationItem Kind Entity that is :
  //! 1 -> Action
  //! 2 -> AssemblyComponentUsage
  //! 3 -> AssemblyComponentUsageSubstitute
  //! 4 -> ConfigurationDesign
  //! 5 -> ConfigurationEffectivity
  //! 6 -> Document
  //! 7 -> DocumentFile
  //! 8 -> DraughtingModel
  //! 9 -> GeneralProperty
  //! 10 -> MakeFromUsageOption
  //! 11 -> MaterialDesignation
  //! 12 -> MechanicalDesignGeometricPresentationRepresentation
  //! 13 -> PresentationArea
  //! 14 -> Product
  //! 15 -> ProductConcept
  //! 16 -> ProductDefinition
  //! 17 -> ProductDefinitionFormation
  //! 18 -> ProductDefinitionRelationship
  //! 19 -> ProductDefinitionUsage
  //! 20 -> PropertyDefinition
  //! 21 -> ShapeRepresentation
  //! 22 -> VersionedActionRequest
  //! 0 else
  Standard_EXPORT virtual Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;

  //! returns Value as a Action (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_Action) Action() const;

  //! returns Value as a AssemblyComponentUsage (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_AssemblyComponentUsage) AssemblyComponentUsage() const;

  //! returns Value as a ConfigurationDesign (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_ConfigurationDesign) ConfigurationDesign() const;

  //! returns Value as a ConfigurationEffectivity (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_ConfigurationEffectivity) ConfigurationEffectivity() const;

  //! returns Value as a DraughtingModel (Null if another type)
  Standard_EXPORT virtual Handle(StepVisual_DraughtingModel) DraughtingModel() const;

  //! returns Value as a GeneralProperty (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_GeneralProperty) GeneralProperty() const;

  //! returns Value as a MakeFromUsageOption (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_MakeFromUsageOption) MakeFromUsageOption() const;

  //! returns Value as a ProductConcept (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_ProductConcept) ProductConcept() const;

  //! returns Value as a ProductDefinitionUsage (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_ProductDefinitionUsage) ProductDefinitionUsage() const;

  //! returns Value as a VersionedActionRequest (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_VersionedActionRequest) VersionedActionRequest() const;

protected:





private:





};







#endif // _StepAP214_SecurityClassificationItem_HeaderFile
