// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepRepr_RepresentedDefinition_HeaderFile
#define _StepRepr_RepresentedDefinition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepBasic_GeneralProperty;
class StepRepr_PropertyDefinition;
class StepRepr_PropertyDefinitionRelationship;
class StepRepr_ShapeAspect;
class StepRepr_ShapeAspectRelationship;


//! Representation of STEP SELECT type RepresentedDefinition
class StepRepr_RepresentedDefinition  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT StepRepr_RepresentedDefinition();
  
  //! Recognizes a kind of RepresentedDefinition select type
  //! 1 -> GeneralProperty from StepBasic
  //! 2 -> PropertyDefinition from StepRepr
  //! 3 -> PropertyDefinitionRelationship from StepRepr
  //! 4 -> ShapeAspect from StepRepr
  //! 5 -> ShapeAspectRelationship from StepRepr
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! Returns Value as GeneralProperty (or Null if another type)
  Standard_EXPORT Handle(StepBasic_GeneralProperty) GeneralProperty() const;
  
  //! Returns Value as PropertyDefinition (or Null if another type)
  Standard_EXPORT Handle(StepRepr_PropertyDefinition) PropertyDefinition() const;
  
  //! Returns Value as PropertyDefinitionRelationship (or Null if another type)
  Standard_EXPORT Handle(StepRepr_PropertyDefinitionRelationship) PropertyDefinitionRelationship() const;
  
  //! Returns Value as ShapeAspect (or Null if another type)
  Standard_EXPORT Handle(StepRepr_ShapeAspect) ShapeAspect() const;
  
  //! Returns Value as ShapeAspectRelationship (or Null if another type)
  Standard_EXPORT Handle(StepRepr_ShapeAspectRelationship) ShapeAspectRelationship() const;




protected:





private:





};







#endif // _StepRepr_RepresentedDefinition_HeaderFile
