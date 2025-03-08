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

#ifndef _StepAP214_AutoDesignGeneralOrgItem_HeaderFile
#define _StepAP214_AutoDesignGeneralOrgItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_Product;
class StepBasic_ProductDefinition;
class StepBasic_ProductDefinitionFormation;
class StepBasic_ProductDefinitionRelationship;
class StepBasic_ProductDefinitionWithAssociatedDocuments;
class StepRepr_Representation;
class StepRepr_ExternallyDefinedRepresentation;
class StepAP214_AutoDesignDocumentReference;

class StepAP214_AutoDesignGeneralOrgItem : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a AutoDesignGeneralOrgItem SelectType
  Standard_EXPORT StepAP214_AutoDesignGeneralOrgItem();

  //! Recognizes a AutoDesignGeneralOrgItem Kind Entity that is :
  //! 1     Product from StepBasic,
  //! 2     ProductDefinition from StepBasic,
  //! 3     ProductDefinitionFormation from StepBasic,
  //! 4     ProductDefinitionRelationship from StepBasic,
  //! 5     ProductDefinitionWithAssociatedDocuments from StepBasic,
  //! 6     Representation from StepRepr
  //! 7     ExternallyDefinedRepresentation from StepRepr,
  //! 8     AutoDesignDocumentReference from StepAP214,
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a Product (Null if another type)
  Standard_EXPORT Handle(StepBasic_Product) Product() const;

  //! returns Value as a ProductDefinition (Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinition) ProductDefinition() const;

  //! returns Value as a ProductDefinitionFormation (Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinitionFormation) ProductDefinitionFormation() const;

  //! returns Value as a ProductDefinitionRelationship (Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinitionRelationship) ProductDefinitionRelationship()
    const;

  //! returns Value as a ProductDefinitionWithAssociatedDocuments (Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinitionWithAssociatedDocuments)
    ProductDefinitionWithAssociatedDocuments() const;

  //! returns Value as a Representation (Null if another type)
  Standard_EXPORT Handle(StepRepr_Representation) Representation() const;

  //! returns Value as a Representation (Null if another type)
  Standard_EXPORT Handle(StepRepr_ExternallyDefinedRepresentation) ExternallyDefinedRepresentation()
    const;

  Standard_EXPORT Handle(StepAP214_AutoDesignDocumentReference) AutoDesignDocumentReference() const;

protected:
private:
};

#endif // _StepAP214_AutoDesignGeneralOrgItem_HeaderFile
