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

#include <StepVisual_AnnotationFillAreaOccurrence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_AnnotationFillAreaOccurrence, StepVisual_AnnotationOccurrence)

//=================================================================================================

StepVisual_AnnotationFillAreaOccurrence::StepVisual_AnnotationFillAreaOccurrence() = default;

//=================================================================================================

void StepVisual_AnnotationFillAreaOccurrence::Init(
  const occ::handle<TCollection_HAsciiString>& theName,
  const occ::handle<NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>>&
                                                           theStyles,
  const occ::handle<Standard_Transient>&                   theItem,
  const occ::handle<StepGeom_GeometricRepresentationItem>& theFillStyleTarget)
{
  StepVisual_AnnotationOccurrence::Init(theName, theStyles, theItem);
  myFillStyleTarget = theFillStyleTarget;
}
