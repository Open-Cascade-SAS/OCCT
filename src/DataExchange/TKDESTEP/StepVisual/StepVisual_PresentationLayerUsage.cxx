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

#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationLayerUsage.hxx>
#include <StepVisual_PresentationRepresentation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_PresentationLayerUsage, Standard_Transient)

StepVisual_PresentationLayerUsage::StepVisual_PresentationLayerUsage() {}

void StepVisual_PresentationLayerUsage::Init(
  const occ::handle<StepVisual_PresentationLayerAssignment>& aAssignment,
  const occ::handle<StepVisual_PresentationRepresentation>&  aPresentation)
{
  theAssignment   = aAssignment;
  thePresentation = aPresentation;
}

void StepVisual_PresentationLayerUsage::SetAssignment(
  const occ::handle<StepVisual_PresentationLayerAssignment>& aAssignment)
{
  theAssignment = aAssignment;
}

occ::handle<StepVisual_PresentationLayerAssignment> StepVisual_PresentationLayerUsage::Assignment() const
{
  return theAssignment;
}

void StepVisual_PresentationLayerUsage::SetPresentation(
  const occ::handle<StepVisual_PresentationRepresentation>& aPresentation)
{
  thePresentation = aPresentation;
}

occ::handle<StepVisual_PresentationRepresentation> StepVisual_PresentationLayerUsage::Presentation()
  const
{
  return thePresentation;
}
