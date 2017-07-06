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

#include <StepAP242_IdAttribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP242_IdAttribute,Standard_Transient)

//=======================================================================
//function : StepAP242_IdAttribute
//purpose  : 
//=======================================================================

StepAP242_IdAttribute::StepAP242_IdAttribute ()  {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP242_IdAttribute::Init(
  const Handle(TCollection_HAsciiString)& theAttributeValue,
  const StepAP242_IdAttributeSelect& theIdentifiedItem)
{
  // --- classe own fields ---
  attributeValue = theAttributeValue;
  identifiedItem = theIdentifiedItem;
}
