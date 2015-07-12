// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
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

#include <GeomInt_TheImpPrmSvSurfacesOfWLApprox.hxx>

#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_QuadricTool.hxx>
#include <IntPatch_WLine.hxx>
#include <GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
 

#define ThePSurface Handle(Adaptor3d_HSurface)
#define ThePSurface_hxx <Adaptor3d_HSurface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define TheISurface IntSurf_Quadric
#define TheISurface_hxx <IntSurf_Quadric.hxx>
#define TheISurfaceTool IntSurf_QuadricTool
#define TheISurfaceTool_hxx <IntSurf_QuadricTool.hxx>
#define Handle_TheLine Handle(IntPatch_WLine)
#define TheLine IntPatch_WLine
#define TheLine_hxx <IntPatch_WLine.hxx>
#define ApproxInt_TheZerImpFunc GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox
#define ApproxInt_TheZerImpFunc_hxx <GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox.hxx>
#define ApproxInt_ImpPrmSvSurfaces GeomInt_TheImpPrmSvSurfacesOfWLApprox
#define ApproxInt_ImpPrmSvSurfaces_hxx <GeomInt_TheImpPrmSvSurfacesOfWLApprox.hxx>
#include <ApproxInt_ImpPrmSvSurfaces.gxx>

