// Created on: 2015-10-29
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

#include <Standard_Type.hxx>
#include <StepVisual_AnnotationPlane.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_AnnotationPlane,StepVisual_AnnotationOccurrence)

//=======================================================================
//function : StepVisual_AnnotationPlane
//purpose  : 
//=======================================================================
StepVisual_AnnotationPlane::StepVisual_AnnotationPlane ()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void StepVisual_AnnotationPlane::Init (const Handle(TCollection_HAsciiString)& theName,
                                       const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& theStyles,
                                       const Handle(Standard_Transient)& theItem,
                                       const Handle(StepVisual_HArray1OfAnnotationPlaneElement)& theElements)
{
  StepVisual_AnnotationOccurrence::Init(theName, theStyles, theItem);
  myElements = theElements;
}
