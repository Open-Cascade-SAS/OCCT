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

#include <StepAP242_IdAttributeSelect.hxx>
#include <Interface_Macros.hxx>
#include <StepBasic_Action.hxx>
#include <StepBasic_Address.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_Group.hxx>
#include <StepBasic_ProductCategory.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepShape_DimensionalSize.hxx>

//=======================================================================
//function : StepAP242_IdAttributeSelect
//purpose  : 
//=======================================================================

StepAP242_IdAttributeSelect::StepAP242_IdAttributeSelect () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepAP242_IdAttributeSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Action))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Address))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ApplicationContext))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Group))) return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductCategory))) return 8;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition))) return 9;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_Representation))) return 10;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 11;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))) return 12;
  return 0;
}

Handle(StepBasic_Action) StepAP242_IdAttributeSelect::Action() const
{  return GetCasted(StepBasic_Action,Value());  }

Handle(StepBasic_Address) StepAP242_IdAttributeSelect::Address() const
{  return GetCasted(StepBasic_Address,Value());  }

Handle(StepBasic_ApplicationContext) StepAP242_IdAttributeSelect::ApplicationContext() const
{  return GetCasted(StepBasic_ApplicationContext,Value());  }

Handle(StepShape_DimensionalSize) StepAP242_IdAttributeSelect::DimensionalSize() const
{  return GetCasted(StepShape_DimensionalSize,Value());  }

Handle(StepDimTol_GeometricTolerance) StepAP242_IdAttributeSelect::GeometricTolerance() const
{  return GetCasted(StepDimTol_GeometricTolerance,Value());  }

Handle(StepBasic_Group) StepAP242_IdAttributeSelect::Group() const
{  return GetCasted(StepBasic_Group,Value());  }

Handle(StepBasic_ProductCategory) StepAP242_IdAttributeSelect::ProductCategory() const
{  return GetCasted(StepBasic_ProductCategory,Value());  }

Handle(StepRepr_PropertyDefinition) StepAP242_IdAttributeSelect::PropertyDefinition() const
{  return GetCasted(StepRepr_PropertyDefinition,Value());  }

Handle(StepRepr_Representation) StepAP242_IdAttributeSelect::Representation() const
{  return GetCasted(StepRepr_Representation,Value());  }

Handle(StepRepr_ShapeAspect) StepAP242_IdAttributeSelect::ShapeAspect() const
{  return GetCasted(StepRepr_ShapeAspect,Value());  }

Handle(StepRepr_ShapeAspectRelationship) StepAP242_IdAttributeSelect::ShapeAspectRelationship() const
{  return GetCasted(StepRepr_ShapeAspectRelationship,Value());  }

