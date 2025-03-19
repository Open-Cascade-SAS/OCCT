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

#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductDefinitionContext, StepBasic_ApplicationContextElement)

StepBasic_ProductDefinitionContext::StepBasic_ProductDefinitionContext() {}

void StepBasic_ProductDefinitionContext::Init(
  const Handle(TCollection_HAsciiString)&     aName,
  const Handle(StepBasic_ApplicationContext)& aFrameOfReference,
  const Handle(TCollection_HAsciiString)&     aLifeCycleStage)
{
  // --- classe own fields ---
  lifeCycleStage = aLifeCycleStage;
  // --- classe inherited fields ---
  StepBasic_ApplicationContextElement::Init(aName, aFrameOfReference);
}

void StepBasic_ProductDefinitionContext::SetLifeCycleStage(
  const Handle(TCollection_HAsciiString)& aLifeCycleStage)
{
  lifeCycleStage = aLifeCycleStage;
}

Handle(TCollection_HAsciiString) StepBasic_ProductDefinitionContext::LifeCycleStage() const
{
  return lifeCycleStage;
}
