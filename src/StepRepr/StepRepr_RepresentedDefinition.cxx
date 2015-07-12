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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Transient.hxx>
#include <StepBasic_GeneralProperty.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_PropertyDefinitionRelationship.hxx>
#include <StepRepr_RepresentedDefinition.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>

//=======================================================================
//function : StepRepr_RepresentedDefinition
//purpose  : 
//=======================================================================
StepRepr_RepresentedDefinition::StepRepr_RepresentedDefinition ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepRepr_RepresentedDefinition::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_GeneralProperty))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinition))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_PropertyDefinitionRelationship))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship))) return 5;
  return 0;
}

//=======================================================================
//function : GeneralProperty
//purpose  : 
//=======================================================================

Handle(StepBasic_GeneralProperty) StepRepr_RepresentedDefinition::GeneralProperty () const
{
  return Handle(StepBasic_GeneralProperty)::DownCast(Value());
}

//=======================================================================
//function : PropertyDefinition
//purpose  : 
//=======================================================================

Handle(StepRepr_PropertyDefinition) StepRepr_RepresentedDefinition::PropertyDefinition () const
{
  return Handle(StepRepr_PropertyDefinition)::DownCast(Value());
}

//=======================================================================
//function : PropertyDefinitionRelationship
//purpose  : 
//=======================================================================

Handle(StepRepr_PropertyDefinitionRelationship) StepRepr_RepresentedDefinition::PropertyDefinitionRelationship () const
{
  return Handle(StepRepr_PropertyDefinitionRelationship)::DownCast(Value());
}

//=======================================================================
//function : ShapeAspect
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepRepr_RepresentedDefinition::ShapeAspect () const
{
  return Handle(StepRepr_ShapeAspect)::DownCast(Value());
}

//=======================================================================
//function : ShapeAspectRelationship
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspectRelationship) StepRepr_RepresentedDefinition::ShapeAspectRelationship () const
{
  return Handle(StepRepr_ShapeAspectRelationship)::DownCast(Value());
}
