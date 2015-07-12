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


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepAP214_AutoDesignPresentedItemSelect.hxx>
#include <StepBasic_DocumentRelationship.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <StepRepr_ShapeAspect.hxx>

StepAP214_AutoDesignPresentedItemSelect::StepAP214_AutoDesignPresentedItemSelect ()    {  }

Standard_Integer  StepAP214_AutoDesignPresentedItemSelect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionRelationship))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_RepresentationRelationship))) return 4;
  if (ent->IsKind(STANDARD_TYPE(StepRepr_ShapeAspect))) return 5;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DocumentRelationship))) return 6;
  return 0;
}

Handle(StepBasic_ProductDefinition)  StepAP214_AutoDesignPresentedItemSelect::ProductDefinition () const
{
  return GetCasted(StepBasic_ProductDefinition,Value());
}

Handle(StepBasic_ProductDefinitionRelationship)  StepAP214_AutoDesignPresentedItemSelect::ProductDefinitionRelationship () const
{
  return GetCasted(StepBasic_ProductDefinitionRelationship,Value());
}

Handle(StepRepr_ProductDefinitionShape)  StepAP214_AutoDesignPresentedItemSelect::ProductDefinitionShape () const
{
  return GetCasted(StepRepr_ProductDefinitionShape,Value());
}

Handle(StepRepr_RepresentationRelationship)  StepAP214_AutoDesignPresentedItemSelect::RepresentationRelationship () const
{
  return GetCasted(StepRepr_RepresentationRelationship,Value());
}

Handle(StepRepr_ShapeAspect)  StepAP214_AutoDesignPresentedItemSelect::ShapeAspect () const
{
  return GetCasted(StepRepr_ShapeAspect,Value());
}

Handle(StepBasic_DocumentRelationship)  StepAP214_AutoDesignPresentedItemSelect::DocumentRelationship () const
{
  return GetCasted(StepBasic_DocumentRelationship,Value());
}
