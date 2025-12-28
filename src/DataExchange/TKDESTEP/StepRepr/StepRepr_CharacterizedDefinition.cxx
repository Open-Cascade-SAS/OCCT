// Created on: 2000-05-11
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Transient.hxx>
#include <StepBasic_CharacterizedObject.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepRepr_CharacterizedDefinition.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>

//=================================================================================================

StepRepr_CharacterizedDefinition::StepRepr_CharacterizedDefinition() {}

//=================================================================================================

int StepRepr_CharacterizedDefinition::CaseNum(const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_CharacterizedObject)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition)))
    return 2;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship)))
    return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape)))
    return 4;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect)))
    return 5;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspectRelationship)))
    return 6;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentFile)))
    return 7;
  return 0;
}

//=================================================================================================

occ::handle<StepBasic_CharacterizedObject> StepRepr_CharacterizedDefinition::CharacterizedObject()
  const
{
  return occ::down_cast<StepBasic_CharacterizedObject>(Value());
}

//=================================================================================================

occ::handle<StepBasic_ProductDefinition> StepRepr_CharacterizedDefinition::ProductDefinition() const
{
  return occ::down_cast<StepBasic_ProductDefinition>(Value());
}

//=================================================================================================

occ::handle<StepBasic_ProductDefinitionRelationship> StepRepr_CharacterizedDefinition::
  ProductDefinitionRelationship() const
{
  return occ::down_cast<StepBasic_ProductDefinitionRelationship>(Value());
}

//=================================================================================================

occ::handle<StepRepr_ProductDefinitionShape> StepRepr_CharacterizedDefinition::
  ProductDefinitionShape() const
{
  return occ::down_cast<StepRepr_ProductDefinitionShape>(Value());
}

//=================================================================================================

occ::handle<StepRepr_ShapeAspect> StepRepr_CharacterizedDefinition::ShapeAspect() const
{
  return occ::down_cast<StepRepr_ShapeAspect>(Value());
}

//=================================================================================================

occ::handle<StepRepr_ShapeAspectRelationship> StepRepr_CharacterizedDefinition::
  ShapeAspectRelationship() const
{
  return occ::down_cast<StepRepr_ShapeAspectRelationship>(Value());
}

//=================================================================================================

occ::handle<StepBasic_DocumentFile> StepRepr_CharacterizedDefinition::DocumentFile() const
{
  return occ::down_cast<StepBasic_DocumentFile>(Value());
}
