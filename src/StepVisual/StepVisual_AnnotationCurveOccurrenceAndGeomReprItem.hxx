// Created on: 2017-02-06
// Created by: Irina KRYLOVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _StepVisual_AnnotationCurveOccurrenceAndGeomReprItem_HeaderFile
#define _StepVisual_AnnotationCurveOccurrenceAndGeomReprItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_AnnotationCurveOccurrence.hxx>

class StepVisual_AnnotationCurveOccurrenceAndGeomReprItem;
DEFINE_STANDARD_HANDLE(StepVisual_AnnotationCurveOccurrenceAndGeomReprItem, StepVisual_AnnotationCurveOccurrence)
//! Added for Dimensional Tolerances
//! Complex STEP entity AnnotationCurveOccurrence & AnnotationOccurrence & GeometricRepresentationItem & RepresentationItem & StyledItem
class StepVisual_AnnotationCurveOccurrenceAndGeomReprItem : public StepVisual_AnnotationCurveOccurrence
{

public:
  
  Standard_EXPORT StepVisual_AnnotationCurveOccurrenceAndGeomReprItem();

  DEFINE_STANDARD_RTTIEXT(StepVisual_AnnotationCurveOccurrenceAndGeomReprItem, StepVisual_AnnotationCurveOccurrence)

};
#endif // _StepVisual_AnnotationCurveOccurrenceAndGeomReprItem_HeaderFile
