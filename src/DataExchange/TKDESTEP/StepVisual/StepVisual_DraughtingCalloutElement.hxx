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

#ifndef _StepVisual_DraughtingCalloutElement_HeaderFile
#define _StepVisual_DraughtingCalloutElement_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepVisual_AnnotationCurveOccurrence;
class StepVisual_AnnotationFillAreaOccurrence;
class StepVisual_AnnotationTextOccurrence;
class StepVisual_TessellatedAnnotationOccurrence;

class StepVisual_DraughtingCalloutElement : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a DraughtingCalloutElement select type
  Standard_EXPORT StepVisual_DraughtingCalloutElement();

  //! Recognizes a IdAttributeSelect Kind Entity that is :
  //! 1 -> AnnotationCurveOccurrence
  //! 2 -> AnnotationTextOccurrence
  //! 3 -> TessellatedAnnotationOccurrence
  //! 4 -> AnnotationFillAreaOccurrence
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a AnnotationCurveOccurrence (Null if another type)
  Standard_EXPORT Handle(StepVisual_AnnotationCurveOccurrence) AnnotationCurveOccurrence() const;

  //! returns Value as a AnnotationTextOccurrence
  Standard_EXPORT Handle(StepVisual_AnnotationTextOccurrence) AnnotationTextOccurrence() const;

  //! returns Value as a TessellatedAnnotationOccurrence
  Standard_EXPORT Handle(StepVisual_TessellatedAnnotationOccurrence)
    TessellatedAnnotationOccurrence() const;

  //! returns Value as a AnnotationFillAreaOccurrence
  Standard_EXPORT Handle(StepVisual_AnnotationFillAreaOccurrence) AnnotationFillAreaOccurrence()
    const;
};
#endif // StepVisual_DraughtingCalloutElement
