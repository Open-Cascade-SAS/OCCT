// Created on: 2002-12-15
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_FeaModelDefinition_HeaderFile
#define _StepFEA_FeaModelDefinition_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_ShapeAspect.hxx>

class StepFEA_FeaModelDefinition;
DEFINE_STANDARD_HANDLE(StepFEA_FeaModelDefinition, StepRepr_ShapeAspect)

//! Representation of STEP entity FeaModelDefinition
class StepFEA_FeaModelDefinition : public StepRepr_ShapeAspect
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_FeaModelDefinition();

  DEFINE_STANDARD_RTTIEXT(StepFEA_FeaModelDefinition, StepRepr_ShapeAspect)

protected:
private:
};

#endif // _StepFEA_FeaModelDefinition_HeaderFile
