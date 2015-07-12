// Created on: 1993-02-05
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Contap_TheSearchInside.hxx>

#include <StdFail_NotDone.hxx>
#include <Standard_OutOfRange.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <Contap_HContTool.hxx>
#include <Contap_SurfFunction.hxx>
#include <IntSurf_InteriorPoint.hxx>
 

#define ThePSurface Handle(Adaptor3d_HSurface)
#define ThePSurface_hxx <Adaptor3d_HSurface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define Handle_TheTopolTool Handle(Adaptor3d_TopolTool)
#define TheTopolTool Adaptor3d_TopolTool
#define TheTopolTool_hxx <Adaptor3d_TopolTool.hxx>
#define TheSITool Contap_HContTool
#define TheSITool_hxx <Contap_HContTool.hxx>
#define TheFunction Contap_SurfFunction
#define TheFunction_hxx <Contap_SurfFunction.hxx>
#define IntStart_SearchInside Contap_TheSearchInside
#define IntStart_SearchInside_hxx <Contap_TheSearchInside.hxx>
#include <IntStart_SearchInside.gxx>

