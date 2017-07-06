// Created on: 2016-12-28
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

#include <Standard_Type.hxx>
#include <StepVisual_AnnotationFillAreaOccurrence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_AnnotationFillAreaOccurrence, StepVisual_AnnotationOccurrence)

//=======================================================================
//function : StepVisual_AnnotationFillAreaOccurrence
//purpose  : 
//=======================================================================
StepVisual_AnnotationFillAreaOccurrence::StepVisual_AnnotationFillAreaOccurrence ()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void StepVisual_AnnotationFillAreaOccurrence::Init(const Handle(TCollection_HAsciiString)& theName,
  const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& theStyles,
  const Handle(Standard_Transient)& theItem,
  const Handle(StepGeom_GeometricRepresentationItem)& theFillStyleTarget)
{
  StepVisual_AnnotationOccurrence::Init(theName, theStyles, theItem);
  myFillStyleTarget = theFillStyleTarget;
}
