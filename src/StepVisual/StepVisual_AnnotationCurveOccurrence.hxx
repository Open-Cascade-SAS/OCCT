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

#ifndef _StepVisual_AnnotationCurveOccurrence_HeaderFile
#define _StepVisual_AnnotationCurveOccurrence_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_AnnotationOccurrence.hxx>

class StepVisual_AnnotationCurveOccurrence;
DEFINE_STANDARD_HANDLE(StepVisual_AnnotationCurveOccurrence, StepVisual_AnnotationOccurrence)

class StepVisual_AnnotationCurveOccurrence : public StepVisual_AnnotationOccurrence
{
public:

  //! Returns a AnnotationCurveOccurrence
  Standard_EXPORT StepVisual_AnnotationCurveOccurrence();

  DEFINE_STANDARD_RTTIEXT(StepVisual_AnnotationCurveOccurrence,StepVisual_AnnotationOccurrence)
};
#endif // _StepVisual_AnnotationCurveOccurrence_HeaderFile
