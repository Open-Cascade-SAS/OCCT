// Created on: 2015-07-10
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

#include <StepVisual_DraughtingCalloutElement.hxx>
#include <Interface_Macros.hxx>
#include <StepVisual_AnnotationCurveOccurrence.hxx>
#include <StepVisual_AnnotationFillAreaOccurrence.hxx>
#include <StepVisual_AnnotationTextOccurrence.hxx>
#include <StepVisual_TessellatedAnnotationOccurrence.hxx>

//=======================================================================
//function : StepVisual_DraughtingCalloutElement
//purpose  : 
//=======================================================================

StepVisual_DraughtingCalloutElement::StepVisual_DraughtingCalloutElement () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_DraughtingCalloutElement::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_AnnotationCurveOccurrence))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_AnnotationTextOccurrence))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_TessellatedAnnotationOccurrence))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_AnnotationFillAreaOccurrence))) return 4;
  return 0;
}

Handle(StepVisual_AnnotationCurveOccurrence) StepVisual_DraughtingCalloutElement::AnnotationCurveOccurrence() const
{  return GetCasted(StepVisual_AnnotationCurveOccurrence,Value());  }

Handle(StepVisual_TessellatedAnnotationOccurrence) StepVisual_DraughtingCalloutElement::TessellatedAnnotationOccurrence()  const
{  return GetCasted(StepVisual_TessellatedAnnotationOccurrence,Value()); } 

Handle(StepVisual_AnnotationTextOccurrence) StepVisual_DraughtingCalloutElement::AnnotationTextOccurrence()  const
{  return GetCasted(StepVisual_AnnotationTextOccurrence, Value()); }

Handle(StepVisual_AnnotationFillAreaOccurrence) StepVisual_DraughtingCalloutElement::AnnotationFillAreaOccurrence()  const
{ return GetCasted(StepVisual_AnnotationFillAreaOccurrence, Value()); }
