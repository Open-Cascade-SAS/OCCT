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

#include <StepDimTol_RunoutZoneDefinition.hxx>

#include <StepRepr_HArray1OfShapeAspect.hxx>
#include <StepDimTol_RunoutZoneOrientation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_RunoutZoneDefinition,StepDimTol_ToleranceZoneDefinition)

//=======================================================================
//function : StepDimTol_RunoutZoneDefinition
//purpose  : 
//=======================================================================

StepDimTol_RunoutZoneDefinition::StepDimTol_RunoutZoneDefinition ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_RunoutZoneDefinition:: Init (const Handle(StepDimTol_ToleranceZone)& theZone,
                                             const Handle(StepRepr_HArray1OfShapeAspect)& theBoundaries,
                                             const Handle(StepDimTol_RunoutZoneOrientation)& theOrientation)
{
  StepDimTol_ToleranceZoneDefinition::Init(theZone, theBoundaries);
  myOrientation = theOrientation;
}
