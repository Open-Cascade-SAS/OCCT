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

#include <StepDimTol_ToleranceZoneForm.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_ToleranceZoneForm,Standard_Transient)

//=======================================================================
//function : StepDimTol_ToleranceZoneForm
//purpose  : 
//=======================================================================

StepDimTol_ToleranceZoneForm::StepDimTol_ToleranceZoneForm ()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_ToleranceZoneForm::Init(
  const Handle(TCollection_HAsciiString)& theName)
{
  // --- classe own fields ---
  myName = theName;
}
