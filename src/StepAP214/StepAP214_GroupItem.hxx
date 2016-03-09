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

#ifndef _StepAP214_GroupItem_HeaderFile
#define _StepAP214_GroupItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepGeom_GeometricRepresentationItem;
class StepBasic_GroupRelationship;
class StepRepr_MappedItem;
class StepBasic_ProductDefinition;
class StepBasic_ProductDefinitionFormation;
class StepRepr_PropertyDefinitionRepresentation;
class StepRepr_Representation;
class StepRepr_RepresentationItem;
class StepRepr_RepresentationRelationshipWithTransformation;
class StepRepr_ShapeAspect;
class StepRepr_ShapeAspectRelationship;
class StepRepr_ShapeRepresentationRelationship;
class StepVisual_StyledItem;
class StepShape_TopologicalRepresentationItem;


class StepAP214_GroupItem  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a GroupItem SelectType
  Standard_EXPORT StepAP214_GroupItem();
  
  //! Recognizes a GroupItem Kind Entity that is :
  //! 1 ->  GeometricRepresentationItem
  //! 2 ->  GroupRelationship
  //! 3 ->  MappedItem
  //! 4 ->  ProductDefinition
  //! 5 ->  ProductDefinitionFormation
  //! 6 ->  PropertyDefinitionRepresentation
  //! 7 ->  Representation
  //! 8 ->  RepresentationItem
  //! 9 ->  RepresentationRelationshipWithTransformation
  //! 10 -> ShapeAspect
  //! 11 -> ShapeAspectRelationship
  //! 12 -> ShapeRepresentationRelationship
  //! 13 -> StyledItem
  //! 14 -> TopologicalRepresentationItem
  //! 0 else
  Standard_EXPORT virtual Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! returns Value as a  GeometricRepresentationItem (Null if another type)
  Standard_EXPORT virtual Handle(StepGeom_GeometricRepresentationItem) GeometricRepresentationItem() const;

  //! returns Value as a  GroupRelationship (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_GroupRelationship) GroupRelationship() const;

  //! returns Value as a  MappedItem (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_MappedItem) MappedItem() const;

  //! returns Value as a  ProductDefinition (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_ProductDefinition) ProductDefinition() const;

  //! returns Value as a  ProductDefinitionFormation (Null if another type)
  Standard_EXPORT virtual Handle(StepBasic_ProductDefinitionFormation) ProductDefinitionFormation() const;

  //! returns Value as a  PropertyDefinitionRepresentation (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_PropertyDefinitionRepresentation) PropertyDefinitionRepresentation() const;

  //! returns Value as a  Representation (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_Representation) Representation() const;

  //! returns Value as a  RepresentationItem (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_RepresentationItem) RepresentationItem() const;

  //! returns Value as a  RepresentationRelationshipWithTransformation (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_RepresentationRelationshipWithTransformation) RepresentationRelationshipWithTransformation() const;

  //! returns Value as a  ShapeAspect (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_ShapeAspect) ShapeAspect() const;

  //! returns Value as a  ShapeAspectRelationship (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_ShapeAspectRelationship) ShapeAspectRelationship() const;

  //! returns Value as a  ShapeRepresentationRelationship (Null if another type)
  Standard_EXPORT virtual Handle(StepRepr_ShapeRepresentationRelationship) ShapeRepresentationRelationship() const;

  //! returns Value as a  StyledItem (Null if another type)
  Standard_EXPORT virtual Handle(StepVisual_StyledItem) StyledItem() const;

  //! returns Value as a  TopologicalRepresentationItem (Null if another type)
  Standard_EXPORT virtual Handle(StepShape_TopologicalRepresentationItem) TopologicalRepresentationItem() const;
protected:





private:





};







#endif // _StepAP214_GroupItem_HeaderFile
