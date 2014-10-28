// Created on: 1997-04-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_box_HeaderFile
#define _TopOpeBRepTool_box_HeaderFile

#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_HBoxTool.hxx>

#ifdef OCCT_DEBUG
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceBOX();
#endif

Standard_EXPORT void FBOX_Prepare();
Standard_EXPORT Handle(TopOpeBRepTool_HBoxTool) FBOX_GetHBoxTool();
Standard_EXPORT const Bnd_Box& FBOX_Box(const TopoDS_Shape& S);

#endif
