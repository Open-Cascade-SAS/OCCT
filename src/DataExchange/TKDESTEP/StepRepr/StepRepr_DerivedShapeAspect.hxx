// Created on: 2001-04-24
// Created by: Cheistian CAILLET
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _StepRepr_DerivedShapeAspect_HeaderFile
#define _StepRepr_DerivedShapeAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_ShapeAspect.hxx>

class StepRepr_DerivedShapeAspect;
DEFINE_STANDARD_HANDLE(StepRepr_DerivedShapeAspect, StepRepr_ShapeAspect)

//! Added for Dimensional Tolerances
class StepRepr_DerivedShapeAspect : public StepRepr_ShapeAspect
{

public:
  Standard_EXPORT StepRepr_DerivedShapeAspect();

  DEFINE_STANDARD_RTTIEXT(StepRepr_DerivedShapeAspect, StepRepr_ShapeAspect)

protected:
private:
};

#endif // _StepRepr_DerivedShapeAspect_HeaderFile
