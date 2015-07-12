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

#include <IntPatch_CurvIntSurf.hxx>

#include <StdFail_NotDone.hxx>
#include <Standard_DomainError.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <IntPatch_HCurve2dTool.hxx>
#include <IntPatch_CSFunction.hxx>
#include <math_FunctionSetRoot.hxx>
#include <gp_Pnt.hxx>
 

#define ThePSurface Handle(Adaptor3d_HSurface)
#define ThePSurface_hxx <Adaptor3d_HSurface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define TheCurve Handle(Adaptor2d_HCurve2d)
#define TheCurve_hxx <Adaptor2d_HCurve2d.hxx>
#define TheCurveTool IntPatch_HCurve2dTool
#define TheCurveTool_hxx <IntPatch_HCurve2dTool.hxx>
#define TheFunction IntPatch_CSFunction
#define TheFunction_hxx <IntPatch_CSFunction.hxx>
#define IntImp_IntCS IntPatch_CurvIntSurf
#define IntImp_IntCS_hxx <IntPatch_CurvIntSurf.hxx>
#include <IntImp_IntCS.gxx>

