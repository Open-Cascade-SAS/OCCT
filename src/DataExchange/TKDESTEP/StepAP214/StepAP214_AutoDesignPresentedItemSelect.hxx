// Created on: 1997-03-26
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepAP214_AutoDesignPresentedItemSelect_HeaderFile
#define _StepAP214_AutoDesignPresentedItemSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_ProductDefinitionRelationship;
class StepBasic_ProductDefinition;
class StepRepr_ProductDefinitionShape;
class StepRepr_RepresentationRelationship;
class StepRepr_ShapeAspect;
class StepBasic_DocumentRelationship;

class StepAP214_AutoDesignPresentedItemSelect : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a AutoDesignPresentedItemSelect SelectType
  Standard_EXPORT StepAP214_AutoDesignPresentedItemSelect();

  //! Recognizes a AutoDesignPresentedItemSelect Kind Entity that is :
  //! 1 -> ProductDefinition,
  //! 2 -> ProductDefinitionRelationship,
  //! 3 -> ProductDefinitionShape
  //! 4 -> RepresentationRelationship
  //! 5 -> ShapeAspect
  //! 6 -> DocumentRelationship,
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a ProductDefinitionRelationship (Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinitionRelationship) ProductDefinitionRelationship()
    const;

  //! returns Value as a ProductDefinition (Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductDefinition) ProductDefinition() const;

  //! returns Value as a ProductDefinitionShape (Null if another type)
  Standard_EXPORT Handle(StepRepr_ProductDefinitionShape) ProductDefinitionShape() const;

  //! returns Value as a RepresentationRelationship (Null if another type)
  Standard_EXPORT Handle(StepRepr_RepresentationRelationship) RepresentationRelationship() const;

  //! returns Value as a ShapeAspect (Null if another type)
  Standard_EXPORT Handle(StepRepr_ShapeAspect) ShapeAspect() const;

  //! returns Value as a DocumentRelationship (Null if another type)
  Standard_EXPORT Handle(StepBasic_DocumentRelationship) DocumentRelationship() const;

protected:
private:
};

#endif // _StepAP214_AutoDesignPresentedItemSelect_HeaderFile
