// Created on: 2016-03-31
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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
#include <StepBasic_ProductDefinitionOrReference.hxx>
#include <StepBasic_ProductDefinitionReference.hxx>
#include <StepBasic_ProductDefinitionReferenceWithLocalRepresentation.hxx>

//=================================================================================================

StepBasic_ProductDefinitionOrReference::StepBasic_ProductDefinitionOrReference() {}

//=================================================================================================

Standard_Integer StepBasic_ProductDefinitionOrReference::CaseNum(
  const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionReference)))
    return 2;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionReferenceWithLocalRepresentation)))
    return 3;
  return 0;
}

//=================================================================================================

Handle(StepBasic_ProductDefinition) StepBasic_ProductDefinitionOrReference::ProductDefinition()
  const
{
  return GetCasted(StepBasic_ProductDefinition, Value());
}

//=================================================================================================

Handle(StepBasic_ProductDefinitionReference) StepBasic_ProductDefinitionOrReference::
  ProductDefinitionReference() const
{
  return GetCasted(StepBasic_ProductDefinitionReference, Value());
}

//=================================================================================================

Handle(StepBasic_ProductDefinitionReferenceWithLocalRepresentation)
  StepBasic_ProductDefinitionOrReference::ProductDefinitionReferenceWithLocalRepresentation() const
{
  return GetCasted(StepBasic_ProductDefinitionReferenceWithLocalRepresentation, Value());
}
