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

#ifndef _StepRepr_ContinuosShapeAspect_HeaderFile
#define _StepRepr_ContinuosShapeAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_CompositeShapeAspect.hxx>

class StepRepr_ContinuosShapeAspect;
DEFINE_STANDARD_HANDLE(StepRepr_ContinuosShapeAspect, StepRepr_CompositeShapeAspect)

//! Added for Dimensional Tolerances
class StepRepr_ContinuosShapeAspect : public StepRepr_CompositeShapeAspect
{

public:
  Standard_EXPORT StepRepr_ContinuosShapeAspect();

  DEFINE_STANDARD_RTTIEXT(StepRepr_ContinuosShapeAspect, StepRepr_CompositeShapeAspect)
};
#endif // _StepRepr_ContinuosShapeAspect_HeaderFile
