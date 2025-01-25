// Created on: 2015-07-10
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepAP242_IdAttributeSelect_HeaderFile
#define _StepAP242_IdAttributeSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_Action;
class StepBasic_Address;
class StepBasic_ApplicationContext;
class StepShape_DimensionalSize;
class StepDimTol_GeometricTolerance;
class StepBasic_Group;
class StepBasic_ProductCategory;
class StepRepr_PropertyDefinition;
class StepRepr_Representation;
class StepRepr_ShapeAspect;
class StepRepr_ShapeAspectRelationship;

class StepAP242_IdAttributeSelect : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a IdAttributeSelect select type
  Standard_EXPORT StepAP242_IdAttributeSelect();

  //! Recognizes a IdAttributeSelect Kind Entity that is :
  //! 1 -> Action
  //! 2 -> Address
  //! 3 -> ApplicationContext
  //! 4 -> DimensionalSize
  //! 5 -> GeometricTolerance
  //! 6 -> Group
  //! 7 -> Reserved for OrganizatonalProject (not implemented in OCCT)
  //! 8 -> ProductCategory
  //! 9 -> PropertyDefinition
  //! 10 -> Representation
  //! 11 -> ShapeAspect
  //! 12 -> ShapeAspectRelationship
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a Action (Null if another type)
  Standard_EXPORT Handle(StepBasic_Action) Action() const;

  //! returns Value as a Address (Null if another type)
  Standard_EXPORT Handle(StepBasic_Address) Address() const;

  //! returns Value as a ApplicationContext (Null if another type)
  Standard_EXPORT Handle(StepBasic_ApplicationContext) ApplicationContext() const;

  //! returns Value as a DimensionalSize (Null if another type)
  Standard_EXPORT Handle(StepShape_DimensionalSize) DimensionalSize() const;

  //! returns Value as a GeometricTolerance (Null if another type)
  Standard_EXPORT Handle(StepDimTol_GeometricTolerance) GeometricTolerance() const;

  //! returns Value as a Group (Null if another type)
  Standard_EXPORT Handle(StepBasic_Group) Group() const;

  //! returns Value as a ProductCategory (Null if another type)
  Standard_EXPORT Handle(StepBasic_ProductCategory) ProductCategory() const;

  //! returns Value as a PropertyDefinition (Null if another type)
  Standard_EXPORT Handle(StepRepr_PropertyDefinition) PropertyDefinition() const;

  //! returns Value as a Representation (Null if another type)
  Standard_EXPORT Handle(StepRepr_Representation) Representation() const;

  //! returns Value as a ShapeAspect (Null if another type)
  Standard_EXPORT Handle(StepRepr_ShapeAspect) ShapeAspect() const;

  //! returns Value as a ShapeAspectRelationship (Null if another type)
  Standard_EXPORT Handle(StepRepr_ShapeAspectRelationship) ShapeAspectRelationship() const;
};
#endif // _StepAP242_IdAttributeSelect_HeaderFile
