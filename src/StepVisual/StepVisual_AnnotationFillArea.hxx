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

#ifndef _StepVisual_AnnotationFillArea_HeaderFile
#define _StepVisual_AnnotationFillArea_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;

class StepVisual_AnnotationFillArea;
DEFINE_STANDARD_HANDLE(StepVisual_AnnotationFillArea, StepShape_GeometricCurveSet)


class StepVisual_AnnotationFillArea : public StepShape_GeometricCurveSet
{

public:  
  //! Returns a AnnotationFillArea
  Standard_EXPORT StepVisual_AnnotationFillArea();

  DEFINE_STANDARD_RTTIEXT(StepVisual_AnnotationFillArea, StepShape_GeometricCurveSet)
};
#endif // _StepVisual_AnnotationFillArea_HeaderFile
