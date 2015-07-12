// Created on: 1991-12-02
// Created by: Laurent PAINNOT
// Copyright (c) 1991-1999 Matra Datavision
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

#include <AppDef_BSpGradient_BFGSOfMyBSplGradientOfBSplineCompute.hxx>

#include <AppDef_MultiLine.hxx>
#include <AppDef_MyLineTool.hxx>
#include <AppDef_MyBSplGradientOfBSplineCompute.hxx>
#include <AppDef_BSpParLeastSquareOfMyBSplGradientOfBSplineCompute.hxx>
#include <AppDef_BSpParFunctionOfMyBSplGradientOfBSplineCompute.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
 

#define MultiLine AppDef_MultiLine
#define MultiLine_hxx <AppDef_MultiLine.hxx>
#define ToolLine AppDef_MyLineTool
#define ToolLine_hxx <AppDef_MyLineTool.hxx>
#define AppParCurves_BSpParLeastSquare AppDef_BSpParLeastSquareOfMyBSplGradientOfBSplineCompute
#define AppParCurves_BSpParLeastSquare_hxx <AppDef_BSpParLeastSquareOfMyBSplGradientOfBSplineCompute.hxx>
#define AppParCurves_BSpParFunction AppDef_BSpParFunctionOfMyBSplGradientOfBSplineCompute
#define AppParCurves_BSpParFunction_hxx <AppDef_BSpParFunctionOfMyBSplGradientOfBSplineCompute.hxx>
#define AppParCurves_BSpGradient_BFGS AppDef_BSpGradient_BFGSOfMyBSplGradientOfBSplineCompute
#define AppParCurves_BSpGradient_BFGS_hxx <AppDef_BSpGradient_BFGSOfMyBSplGradientOfBSplineCompute.hxx>
#define AppParCurves_BSpGradient AppDef_MyBSplGradientOfBSplineCompute
#define AppParCurves_BSpGradient_hxx <AppDef_MyBSplGradientOfBSplineCompute.hxx>
#include <AppParCurves_BSpGradient_BFGS.gxx>

