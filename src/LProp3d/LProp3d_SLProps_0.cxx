// Created on: 2002-08-02
// Created by: Alexander KARTOMIN  (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <LProp3d_SLProps.hxx>

#include <Adaptor3d_HSurface.hxx>
#include <LProp_BadContinuity.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <LProp_NotDefined.hxx>
#include <LProp3d_SurfaceTool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
 

#define Surface Handle(Adaptor3d_HSurface)
#define Surface_hxx <Adaptor3d_HSurface.hxx>
#define Tool LProp3d_SurfaceTool
#define Tool_hxx <LProp3d_SurfaceTool.hxx>
#define LProp_SLProps LProp3d_SLProps
#define LProp_SLProps_hxx <LProp3d_SLProps.hxx>
#include <LProp_SLProps.gxx>

