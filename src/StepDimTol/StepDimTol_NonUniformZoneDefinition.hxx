// Created on: 2015-07-13
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

#ifndef _StepDimTol_NonUniformZoneDefinition_HeaderFile
#define _StepDimTol_NonUniformZoneDefinition_HeaderFile

#include <Standard.hxx>

#include <StepDimTol_ToleranceZoneDefinition.hxx>

class StepDimTol_NonUniformZoneDefinition;
DEFINE_STANDARD_HANDLE(StepDimTol_NonUniformZoneDefinition, StepDimTol_ToleranceZoneDefinition)
//! Representation of STEP entity NonUniformZoneDefinition
class StepDimTol_NonUniformZoneDefinition : public StepDimTol_ToleranceZoneDefinition
{

public:
  
  //! Empty constructor
  Standard_EXPORT StepDimTol_NonUniformZoneDefinition();

  DEFINE_STANDARD_RTTIEXT(StepDimTol_NonUniformZoneDefinition,StepDimTol_ToleranceZoneDefinition)
};
#endif // _StepDimTol_NonUniformZoneDefinition_HeaderFile
