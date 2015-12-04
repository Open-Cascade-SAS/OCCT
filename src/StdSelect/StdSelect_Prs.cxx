// Created on: 1995-03-17
// Created by: Robert COUBLANC
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


#include <Graphic3d_StructureManager.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_Prs.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StdSelect_Prs,Prs3d_Presentation)

StdSelect_Prs::
StdSelect_Prs(const Handle(Graphic3d_StructureManager)& aStructureManager):
Prs3d_Presentation(aStructureManager),
myManager(aStructureManager){}

