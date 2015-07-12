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

#include <GeomInt_WLApprox.hxx>

#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_QuadricTool.hxx>
#include <IntPatch_WLine.hxx>
#include <GeomInt_ThePrmPrmSvSurfacesOfWLApprox.hxx>
#include <GeomInt_TheInt2SOfThePrmPrmSvSurfacesOfWLApprox.hxx>
#include <GeomInt_TheImpPrmSvSurfacesOfWLApprox.hxx>
#include <GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox.hxx>
#include <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#include <GeomInt_TheComputeLineOfWLApprox.hxx>
#include <GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#include <GeomInt_MyGradientbisOfTheComputeLineOfWLApprox.hxx>
#include <GeomInt_TheComputeLineBezierOfWLApprox.hxx>
#include <GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
 

#define ThePSurface Handle(Adaptor3d_HSurface)
#define ThePSurface_hxx <Adaptor3d_HSurface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define TheISurface IntSurf_Quadric
#define TheISurface_hxx <IntSurf_Quadric.hxx>
#define TheISurfaceTool IntSurf_QuadricTool
#define TheISurfaceTool_hxx <IntSurf_QuadricTool.hxx>
#define Handle_TheWLine Handle(IntPatch_WLine)
#define TheWLine IntPatch_WLine
#define TheWLine_hxx <IntPatch_WLine.hxx>
#define ApproxInt_ThePrmPrmSvSurfaces GeomInt_ThePrmPrmSvSurfacesOfWLApprox
#define ApproxInt_ThePrmPrmSvSurfaces_hxx <GeomInt_ThePrmPrmSvSurfacesOfWLApprox.hxx>
#define ApproxInt_TheInt2SOfThePrmPrmSvSurfaces GeomInt_TheInt2SOfThePrmPrmSvSurfacesOfWLApprox
#define ApproxInt_TheInt2SOfThePrmPrmSvSurfaces_hxx <GeomInt_TheInt2SOfThePrmPrmSvSurfacesOfWLApprox.hxx>
#define ApproxInt_TheInt2SOfThePrmPrmSvSurfaces GeomInt_TheInt2SOfThePrmPrmSvSurfacesOfWLApprox
#define ApproxInt_TheInt2SOfThePrmPrmSvSurfaces_hxx <GeomInt_TheInt2SOfThePrmPrmSvSurfacesOfWLApprox.hxx>
#define ApproxInt_TheImpPrmSvSurfaces GeomInt_TheImpPrmSvSurfacesOfWLApprox
#define ApproxInt_TheImpPrmSvSurfaces_hxx <GeomInt_TheImpPrmSvSurfacesOfWLApprox.hxx>
#define ApproxInt_TheZerImpFuncOfTheImpPrmSvSurfaces GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox
#define ApproxInt_TheZerImpFuncOfTheImpPrmSvSurfaces_hxx <GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox.hxx>
#define ApproxInt_TheZerImpFuncOfTheImpPrmSvSurfaces GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox
#define ApproxInt_TheZerImpFuncOfTheImpPrmSvSurfaces_hxx <GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox.hxx>
#define ApproxInt_TheMultiLine GeomInt_TheMultiLineOfWLApprox
#define ApproxInt_TheMultiLine_hxx <GeomInt_TheMultiLineOfWLApprox.hxx>
#define ApproxInt_TheMultiLineTool GeomInt_TheMultiLineToolOfWLApprox
#define ApproxInt_TheMultiLineTool_hxx <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#define ApproxInt_TheComputeLine GeomInt_TheComputeLineOfWLApprox
#define ApproxInt_TheComputeLine_hxx <GeomInt_TheComputeLineOfWLApprox.hxx>
#define ApproxInt_MyBSplGradientOfTheComputeLine GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox
#define ApproxInt_MyBSplGradientOfTheComputeLine_hxx <GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#define ApproxInt_MyGradientbisOfTheComputeLine GeomInt_MyGradientbisOfTheComputeLineOfWLApprox
#define ApproxInt_MyGradientbisOfTheComputeLine_hxx <GeomInt_MyGradientbisOfTheComputeLineOfWLApprox.hxx>
#define ApproxInt_MyBSplGradientOfTheComputeLine GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox
#define ApproxInt_MyBSplGradientOfTheComputeLine_hxx <GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#define ApproxInt_MyGradientbisOfTheComputeLine GeomInt_MyGradientbisOfTheComputeLineOfWLApprox
#define ApproxInt_MyGradientbisOfTheComputeLine_hxx <GeomInt_MyGradientbisOfTheComputeLineOfWLApprox.hxx>
#define ApproxInt_TheComputeLineBezier GeomInt_TheComputeLineBezierOfWLApprox
#define ApproxInt_TheComputeLineBezier_hxx <GeomInt_TheComputeLineBezierOfWLApprox.hxx>
#define ApproxInt_MyGradientOfTheComputeLineBezier GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox
#define ApproxInt_MyGradientOfTheComputeLineBezier_hxx <GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define ApproxInt_MyGradientOfTheComputeLineBezier GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox
#define ApproxInt_MyGradientOfTheComputeLineBezier_hxx <GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define ApproxInt_Approx GeomInt_WLApprox
#define ApproxInt_Approx_hxx <GeomInt_WLApprox.hxx>
#include <ApproxInt_Approx.gxx>

