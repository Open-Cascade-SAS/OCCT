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

#include <StepDimTol_ToleranceZone.hxx>

#include <StepDimTol_HArray1OfToleranceZoneTarget.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_ToleranceZone,StepRepr_ShapeAspect)

//=======================================================================
//function : StepDimTol_ToleranceZone
//purpose  : 
//=======================================================================

StepDimTol_ToleranceZone::StepDimTol_ToleranceZone ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_ToleranceZone::Init (const Handle(TCollection_HAsciiString)& theName,
                                     const Handle(TCollection_HAsciiString)& theDescription,
                                     const Handle(StepRepr_ProductDefinitionShape)& theOfShape,
                                     const StepData_Logical theProductDefinitional,
                                     const Handle(StepDimTol_HArray1OfToleranceZoneTarget)& theDefiningTolerance,
                                     const Handle(StepDimTol_ToleranceZoneForm)& theForm)
{
  StepRepr_ShapeAspect::Init(theName, theDescription, theOfShape, theProductDefinitional);
  myDefiningTolerance = theDefiningTolerance;
  myForm = theForm;
}
    