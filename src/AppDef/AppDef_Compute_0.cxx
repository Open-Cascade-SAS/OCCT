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

#include <AppDef_Compute.hxx>

#include <AppDef_MultiLine.hxx>
#include <AppDef_MyLineTool.hxx>
#include <AppDef_MyGradientOfCompute.hxx>
#include <AppDef_ParLeastSquareOfMyGradientOfCompute.hxx>
#include <AppDef_ResConstraintOfMyGradientOfCompute.hxx>
#include <AppDef_ParFunctionOfMyGradientOfCompute.hxx>
#include <AppDef_Gradient_BFGSOfMyGradientOfCompute.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
 

#define MultiLine AppDef_MultiLine
#define MultiLine_hxx <AppDef_MultiLine.hxx>
#define LineTool AppDef_MyLineTool
#define LineTool_hxx <AppDef_MyLineTool.hxx>
#define Approx_MyGradient AppDef_MyGradientOfCompute
#define Approx_MyGradient_hxx <AppDef_MyGradientOfCompute.hxx>
#define Approx_ParLeastSquareOfMyGradient AppDef_ParLeastSquareOfMyGradientOfCompute
#define Approx_ParLeastSquareOfMyGradient_hxx <AppDef_ParLeastSquareOfMyGradientOfCompute.hxx>
#define Approx_ResConstraintOfMyGradient AppDef_ResConstraintOfMyGradientOfCompute
#define Approx_ResConstraintOfMyGradient_hxx <AppDef_ResConstraintOfMyGradientOfCompute.hxx>
#define Approx_ParFunctionOfMyGradient AppDef_ParFunctionOfMyGradientOfCompute
#define Approx_ParFunctionOfMyGradient_hxx <AppDef_ParFunctionOfMyGradientOfCompute.hxx>
#define Approx_Gradient_BFGSOfMyGradient AppDef_Gradient_BFGSOfMyGradientOfCompute
#define Approx_Gradient_BFGSOfMyGradient_hxx <AppDef_Gradient_BFGSOfMyGradientOfCompute.hxx>
#define Approx_ParLeastSquareOfMyGradient AppDef_ParLeastSquareOfMyGradientOfCompute
#define Approx_ParLeastSquareOfMyGradient_hxx <AppDef_ParLeastSquareOfMyGradientOfCompute.hxx>
#define Approx_ResConstraintOfMyGradient AppDef_ResConstraintOfMyGradientOfCompute
#define Approx_ResConstraintOfMyGradient_hxx <AppDef_ResConstraintOfMyGradientOfCompute.hxx>
#define Approx_ParFunctionOfMyGradient AppDef_ParFunctionOfMyGradientOfCompute
#define Approx_ParFunctionOfMyGradient_hxx <AppDef_ParFunctionOfMyGradientOfCompute.hxx>
#define Approx_Gradient_BFGSOfMyGradient AppDef_Gradient_BFGSOfMyGradientOfCompute
#define Approx_Gradient_BFGSOfMyGradient_hxx <AppDef_Gradient_BFGSOfMyGradientOfCompute.hxx>
#define Approx_ComputeLine AppDef_Compute
#define Approx_ComputeLine_hxx <AppDef_Compute.hxx>
#include <Approx_ComputeLine.gxx>

