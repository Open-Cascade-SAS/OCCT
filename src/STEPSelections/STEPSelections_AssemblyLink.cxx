// Created on: 1999-03-24
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <STEPSelections_AssemblyComponent.hxx>
#include <STEPSelections_AssemblyLink.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPSelections_AssemblyLink,Standard_Transient)

STEPSelections_AssemblyLink::STEPSelections_AssemblyLink()
{
}

STEPSelections_AssemblyLink::STEPSelections_AssemblyLink(const Handle(StepRepr_NextAssemblyUsageOccurrence)& nauo,
						       const Handle(Standard_Transient)& item,
						       const Handle(STEPSelections_AssemblyComponent)& part)
{
  myNAUO = nauo;
  myItem = item;
  myComponent = part;
}
