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


#include <Standard_Type.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationLayerUsage.hxx>
#include <StepVisual_PresentationRepresentation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_PresentationLayerUsage,Standard_Transient)

StepVisual_PresentationLayerUsage::StepVisual_PresentationLayerUsage  ()    {  }

void  StepVisual_PresentationLayerUsage::Init
  (const Handle(StepVisual_PresentationLayerAssignment)& aAssignment,
   const Handle(StepVisual_PresentationRepresentation)& aPresentation)
{  theAssignment = aAssignment;  thePresentation = aPresentation;  }

void  StepVisual_PresentationLayerUsage::SetAssignment
  (const Handle(StepVisual_PresentationLayerAssignment)& aAssignment)
{  theAssignment = aAssignment;  }

Handle(StepVisual_PresentationLayerAssignment)  StepVisual_PresentationLayerUsage::Assignment () const
{  return theAssignment;  }

void  StepVisual_PresentationLayerUsage::SetPresentation
  (const Handle(StepVisual_PresentationRepresentation)& aPresentation)
{  thePresentation = aPresentation;  }

Handle(StepVisual_PresentationRepresentation)  StepVisual_PresentationLayerUsage::Presentation () const
{  return thePresentation;  }
