// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#include <IntPatch_TheIWLineOfTheIWalking.hxx>

#include <Standard_Type.hxx>

#include <IntSurf_LineOn2S.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_DomainError.hxx>
#include <IntSurf_PathPoint.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <gp_Vec.hxx>

 








#define TheStartPoint IntSurf_PathPoint
#define TheStartPoint_hxx <IntSurf_PathPoint.hxx>
#define IntWalk_IWLine IntPatch_TheIWLineOfTheIWalking
#define IntWalk_IWLine_hxx <IntPatch_TheIWLineOfTheIWalking.hxx>
#define Handle_IntWalk_IWLine Handle(IntPatch_TheIWLineOfTheIWalking)
#include <IntWalk_IWLine.gxx>

