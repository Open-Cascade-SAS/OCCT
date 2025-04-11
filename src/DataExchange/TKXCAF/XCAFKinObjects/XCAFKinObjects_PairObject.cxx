// Created on: 2020-03-16
// Created by: Irina KRYLOVA
// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <XCAFKinObjects_PairObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFKinObjects_PairObject, Standard_Transient)

//=================================================================================================

XCAFKinObjects_PairObject::XCAFKinObjects_PairObject() {}

//=================================================================================================

XCAFKinObjects_PairObject::XCAFKinObjects_PairObject(
  const Handle(XCAFKinObjects_PairObject)& theObj)
{
  myName  = theObj->myName;
  myType  = theObj->myType;
  myTrsf1 = theObj->myTrsf1;
  myTrsf2 = theObj->myTrsf2;
}
