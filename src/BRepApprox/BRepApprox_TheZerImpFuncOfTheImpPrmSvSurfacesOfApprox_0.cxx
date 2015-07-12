// Created on: 1995-06-06
// Created by: Jean Yves LEBEY
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

#include <BRepApprox_TheZerImpFuncOfTheImpPrmSvSurfacesOfApprox.hxx>

#include <StdFail_UndefinedDerivative.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepApprox_SurfaceTool.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_QuadricTool.hxx>
#include <math_Matrix.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir2d.hxx>
 

#define ThePSurface BRepAdaptor_Surface
#define ThePSurface_hxx <BRepAdaptor_Surface.hxx>
#define ThePSurfaceTool BRepApprox_SurfaceTool
#define ThePSurfaceTool_hxx <BRepApprox_SurfaceTool.hxx>
#define TheISurface IntSurf_Quadric
#define TheISurface_hxx <IntSurf_Quadric.hxx>
#define TheISurfaceTool IntSurf_QuadricTool
#define TheISurfaceTool_hxx <IntSurf_QuadricTool.hxx>
#define IntImp_ZerImpFunc BRepApprox_TheZerImpFuncOfTheImpPrmSvSurfacesOfApprox
#define IntImp_ZerImpFunc_hxx <BRepApprox_TheZerImpFuncOfTheImpPrmSvSurfacesOfApprox.hxx>
#include <IntImp_ZerImpFunc.gxx>

