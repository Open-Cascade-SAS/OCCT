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

#include <BRepApprox_Approx.hxx>

#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepApprox_SurfaceTool.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_QuadricTool.hxx>
#include <BRepApprox_ApproxLine.hxx>
#include <BRepApprox_ThePrmPrmSvSurfacesOfApprox.hxx>
#include <BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox.hxx>
#include <BRepApprox_TheImpPrmSvSurfacesOfApprox.hxx>
#include <BRepApprox_TheZerImpFuncOfTheImpPrmSvSurfacesOfApprox.hxx>
#include <BRepApprox_TheMultiLineOfApprox.hxx>
#include <BRepApprox_TheMultiLineToolOfApprox.hxx>
#include <BRepApprox_TheComputeLineOfApprox.hxx>
#include <BRepApprox_MyBSplGradientOfTheComputeLineOfApprox.hxx>
#include <BRepApprox_MyGradientbisOfTheComputeLineOfApprox.hxx>
#include <BRepApprox_TheComputeLineBezierOfApprox.hxx>
#include <BRepApprox_MyGradientOfTheComputeLineBezierOfApprox.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
 

#define ThePSurface BRepAdaptor_Surface
#define ThePSurface_hxx <BRepAdaptor_Surface.hxx>
#define ThePSurfaceTool BRepApprox_SurfaceTool
#define ThePSurfaceTool_hxx <BRepApprox_SurfaceTool.hxx>
#define TheISurface IntSurf_Quadric
#define TheISurface_hxx <IntSurf_Quadric.hxx>
#define TheISurfaceTool IntSurf_QuadricTool
#define TheISurfaceTool_hxx <IntSurf_QuadricTool.hxx>
#define Handle_TheWLine Handle(BRepApprox_ApproxLine)
#define TheWLine BRepApprox_ApproxLine
#define TheWLine_hxx <BRepApprox_ApproxLine.hxx>
#define ApproxInt_ThePrmPrmSvSurfaces BRepApprox_ThePrmPrmSvSurfacesOfApprox
#define ApproxInt_ThePrmPrmSvSurfaces_hxx <BRepApprox_ThePrmPrmSvSurfacesOfApprox.hxx>
#define ApproxInt_TheInt2SOfThePrmPrmSvSurfaces BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox
#define ApproxInt_TheInt2SOfThePrmPrmSvSurfaces_hxx <BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox.hxx>
#define ApproxInt_TheInt2SOfThePrmPrmSvSurfaces BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox
#define ApproxInt_TheInt2SOfThePrmPrmSvSurfaces_hxx <BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox.hxx>
#define ApproxInt_TheImpPrmSvSurfaces BRepApprox_TheImpPrmSvSurfacesOfApprox
#define ApproxInt_TheImpPrmSvSurfaces_hxx <BRepApprox_TheImpPrmSvSurfacesOfApprox.hxx>
#define ApproxInt_TheZerImpFuncOfTheImpPrmSvSurfaces BRepApprox_TheZerImpFuncOfTheImpPrmSvSurfacesOfApprox
#define ApproxInt_TheZerImpFuncOfTheImpPrmSvSurfaces_hxx <BRepApprox_TheZerImpFuncOfTheImpPrmSvSurfacesOfApprox.hxx>
#define ApproxInt_TheZerImpFuncOfTheImpPrmSvSurfaces BRepApprox_TheZerImpFuncOfTheImpPrmSvSurfacesOfApprox
#define ApproxInt_TheZerImpFuncOfTheImpPrmSvSurfaces_hxx <BRepApprox_TheZerImpFuncOfTheImpPrmSvSurfacesOfApprox.hxx>
#define ApproxInt_TheMultiLine BRepApprox_TheMultiLineOfApprox
#define ApproxInt_TheMultiLine_hxx <BRepApprox_TheMultiLineOfApprox.hxx>
#define ApproxInt_TheMultiLineTool BRepApprox_TheMultiLineToolOfApprox
#define ApproxInt_TheMultiLineTool_hxx <BRepApprox_TheMultiLineToolOfApprox.hxx>
#define ApproxInt_TheComputeLine BRepApprox_TheComputeLineOfApprox
#define ApproxInt_TheComputeLine_hxx <BRepApprox_TheComputeLineOfApprox.hxx>
#define ApproxInt_MyBSplGradientOfTheComputeLine BRepApprox_MyBSplGradientOfTheComputeLineOfApprox
#define ApproxInt_MyBSplGradientOfTheComputeLine_hxx <BRepApprox_MyBSplGradientOfTheComputeLineOfApprox.hxx>
#define ApproxInt_MyGradientbisOfTheComputeLine BRepApprox_MyGradientbisOfTheComputeLineOfApprox
#define ApproxInt_MyGradientbisOfTheComputeLine_hxx <BRepApprox_MyGradientbisOfTheComputeLineOfApprox.hxx>
#define ApproxInt_MyBSplGradientOfTheComputeLine BRepApprox_MyBSplGradientOfTheComputeLineOfApprox
#define ApproxInt_MyBSplGradientOfTheComputeLine_hxx <BRepApprox_MyBSplGradientOfTheComputeLineOfApprox.hxx>
#define ApproxInt_MyGradientbisOfTheComputeLine BRepApprox_MyGradientbisOfTheComputeLineOfApprox
#define ApproxInt_MyGradientbisOfTheComputeLine_hxx <BRepApprox_MyGradientbisOfTheComputeLineOfApprox.hxx>
#define ApproxInt_TheComputeLineBezier BRepApprox_TheComputeLineBezierOfApprox
#define ApproxInt_TheComputeLineBezier_hxx <BRepApprox_TheComputeLineBezierOfApprox.hxx>
#define ApproxInt_MyGradientOfTheComputeLineBezier BRepApprox_MyGradientOfTheComputeLineBezierOfApprox
#define ApproxInt_MyGradientOfTheComputeLineBezier_hxx <BRepApprox_MyGradientOfTheComputeLineBezierOfApprox.hxx>
#define ApproxInt_MyGradientOfTheComputeLineBezier BRepApprox_MyGradientOfTheComputeLineBezierOfApprox
#define ApproxInt_MyGradientOfTheComputeLineBezier_hxx <BRepApprox_MyGradientOfTheComputeLineBezierOfApprox.hxx>
#define ApproxInt_Approx BRepApprox_Approx
#define ApproxInt_Approx_hxx <BRepApprox_Approx.hxx>
#include <ApproxInt_Approx.gxx>

