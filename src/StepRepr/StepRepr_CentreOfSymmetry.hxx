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

#ifndef _StepRepr_CentreOfSymmetry_HeaderFile
#define _StepRepr_CentreOfSymmetry_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_DerivedShapeAspect.hxx>

class StepRepr_CentreOfSymmetry;
DEFINE_STANDARD_HANDLE(StepRepr_CentreOfSymmetry, StepRepr_DerivedShapeAspect)
//! Added for Dimensional Tolerances
class StepRepr_CentreOfSymmetry : public StepRepr_DerivedShapeAspect
{

public:
  
  Standard_EXPORT StepRepr_CentreOfSymmetry();

  DEFINE_STANDARD_RTTIEXT(StepRepr_CentreOfSymmetry,StepRepr_DerivedShapeAspect)

};
#endif // _StepRepr_CentreOfSymmetry_HeaderFile
