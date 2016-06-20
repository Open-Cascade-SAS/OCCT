// Copyright (c) 1995-1999 Matra Datavision
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

#include <Graphic3d_CStructure.hxx>

#include "Graphic3d_Structure.pxx"
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <Graphic3d_GraphicDriver.hxx>


IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_CStructure,Standard_Transient)

//=============================================================================
//function : Graphic3d_CStructure
//purpose  :
//=============================================================================
Graphic3d_CStructure::Graphic3d_CStructure (const Handle(Graphic3d_StructureManager)& theManager)
: myZLayer         (Graphic3d_ZLayerId_Default),
  Priority         (Structure_MAX_PRIORITY / 2),
  PreviousPriority (Structure_MAX_PRIORITY / 2),
  ContainsFacet    (0),
  IsInfinite       (0),
  stick            (0),
  highlight        (0),
  visible          (1),
  HLRValidation    (0),
  IsForHighlight   (Standard_False),
  IsMutable        (Standard_False),
  Is2dText         (Standard_False),
  myGraphicDriver  (theManager->GraphicDriver())
{
  Id = myGraphicDriver->NewIdentification();
}
