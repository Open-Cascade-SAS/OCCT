// Created on: 1993-04-07
// Created by: Laurent BUCHARD
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

#include <IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter.hxx>

#include <Adaptor3d_Curve.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntCurveSurface_TheHCurveTool.hxx>

#define TheQuadric IntSurf_Quadric
#define TheQuadric_hxx <IntSurf_Quadric.hxx>
#define TheCurve Handle(Adaptor3d_Curve)
#define TheCurve_hxx <Adaptor3d_Curve.hxx>
#define TheCurveTool IntCurveSurface_TheHCurveTool
#define TheCurveTool_hxx <IntCurveSurface_TheHCurveTool.hxx>
#define IntCurveSurface_QuadricCurveFunc IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter
#define IntCurveSurface_QuadricCurveFunc_hxx                                                       \
  <IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter.hxx>
#include "../IntCurveSurface/IntCurveSurface_QuadricCurveFunc.gxx"
