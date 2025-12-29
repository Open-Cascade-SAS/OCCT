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

#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductDefinition, Standard_Transient)

StepBasic_ProductDefinition::StepBasic_ProductDefinition() = default;

void StepBasic_ProductDefinition::Init(
  const occ::handle<TCollection_HAsciiString>&             aId,
  const occ::handle<TCollection_HAsciiString>&             aDescription,
  const occ::handle<StepBasic_ProductDefinitionFormation>& aFormation,
  const occ::handle<StepBasic_ProductDefinitionContext>&   aFrameOfReference)
{
  // --- classe own fields ---
  id               = aId;
  description      = aDescription;
  formation        = aFormation;
  frameOfReference = aFrameOfReference;
}

void StepBasic_ProductDefinition::SetId(const occ::handle<TCollection_HAsciiString>& aId)
{
  id = aId;
}

occ::handle<TCollection_HAsciiString> StepBasic_ProductDefinition::Id() const
{
  return id;
}

void StepBasic_ProductDefinition::SetDescription(
  const occ::handle<TCollection_HAsciiString>& aDescription)
{
  description = aDescription;
}

occ::handle<TCollection_HAsciiString> StepBasic_ProductDefinition::Description() const
{
  return description;
}

void StepBasic_ProductDefinition::SetFormation(
  const occ::handle<StepBasic_ProductDefinitionFormation>& aFormation)
{
  formation = aFormation;
}

occ::handle<StepBasic_ProductDefinitionFormation> StepBasic_ProductDefinition::Formation() const
{
  return formation;
}

void StepBasic_ProductDefinition::SetFrameOfReference(
  const occ::handle<StepBasic_ProductDefinitionContext>& aFrameOfReference)
{
  frameOfReference = aFrameOfReference;
}

occ::handle<StepBasic_ProductDefinitionContext> StepBasic_ProductDefinition::FrameOfReference()
  const
{
  return frameOfReference;
}
