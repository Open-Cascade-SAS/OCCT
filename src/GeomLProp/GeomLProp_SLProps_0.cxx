// Created on: 1992-03-26
// Created by: Herve LEGRAND
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

#include <GeomLProp_SLProps.hxx>

#include <Geom_Surface.hxx>
#include <LProp_BadContinuity.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <LProp_NotDefined.hxx>
#include <GeomLProp_SurfaceTool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
 

#define Surface Handle(Geom_Surface)
#define Surface_hxx <Geom_Surface.hxx>
#define Tool GeomLProp_SurfaceTool
#define Tool_hxx <GeomLProp_SurfaceTool.hxx>
#define LProp_SLProps GeomLProp_SLProps
#define LProp_SLProps_hxx <GeomLProp_SLProps.hxx>
#include <LProp_SLProps.gxx>

