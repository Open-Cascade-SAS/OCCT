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

#include <IntPatch_TheSurfFunction.hxx>

#include <StdFail_UndefinedDerivative.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_QuadricTool.hxx>
#include <math_Matrix.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir2d.hxx>
 

#define ThePSurface Handle(Adaptor3d_HSurface)
#define ThePSurface_hxx <Adaptor3d_HSurface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define TheISurface IntSurf_Quadric
#define TheISurface_hxx <IntSurf_Quadric.hxx>
#define TheISurfaceTool IntSurf_QuadricTool
#define TheISurfaceTool_hxx <IntSurf_QuadricTool.hxx>
#define IntImp_ZerImpFunc IntPatch_TheSurfFunction
#define IntImp_ZerImpFunc_hxx <IntPatch_TheSurfFunction.hxx>
#include <IntImp_ZerImpFunc.gxx>

