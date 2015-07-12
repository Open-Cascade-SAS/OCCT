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

#include <BRepApprox_MyGradientbisOfTheComputeLineOfApprox.hxx>

#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <BRepApprox_TheMultiLineOfApprox.hxx>
#include <BRepApprox_TheMultiLineToolOfApprox.hxx>
#include <BRepApprox_ParLeastSquareOfMyGradientbisOfTheComputeLineOfApprox.hxx>
#include <BRepApprox_ResConstraintOfMyGradientbisOfTheComputeLineOfApprox.hxx>
#include <BRepApprox_ParFunctionOfMyGradientbisOfTheComputeLineOfApprox.hxx>
#include <BRepApprox_Gradient_BFGSOfMyGradientbisOfTheComputeLineOfApprox.hxx>
#include <AppParCurves_MultiCurve.hxx>
 

#define MultiLine BRepApprox_TheMultiLineOfApprox
#define MultiLine_hxx <BRepApprox_TheMultiLineOfApprox.hxx>
#define ToolLine BRepApprox_TheMultiLineToolOfApprox
#define ToolLine_hxx <BRepApprox_TheMultiLineToolOfApprox.hxx>
#define AppParCurves_ParLeastSquare BRepApprox_ParLeastSquareOfMyGradientbisOfTheComputeLineOfApprox
#define AppParCurves_ParLeastSquare_hxx <BRepApprox_ParLeastSquareOfMyGradientbisOfTheComputeLineOfApprox.hxx>
#define AppParCurves_ResConstraint BRepApprox_ResConstraintOfMyGradientbisOfTheComputeLineOfApprox
#define AppParCurves_ResConstraint_hxx <BRepApprox_ResConstraintOfMyGradientbisOfTheComputeLineOfApprox.hxx>
#define AppParCurves_ParFunction BRepApprox_ParFunctionOfMyGradientbisOfTheComputeLineOfApprox
#define AppParCurves_ParFunction_hxx <BRepApprox_ParFunctionOfMyGradientbisOfTheComputeLineOfApprox.hxx>
#define AppParCurves_Gradient_BFGS BRepApprox_Gradient_BFGSOfMyGradientbisOfTheComputeLineOfApprox
#define AppParCurves_Gradient_BFGS_hxx <BRepApprox_Gradient_BFGSOfMyGradientbisOfTheComputeLineOfApprox.hxx>
#define AppParCurves_Gradient BRepApprox_MyGradientbisOfTheComputeLineOfApprox
#define AppParCurves_Gradient_hxx <BRepApprox_MyGradientbisOfTheComputeLineOfApprox.hxx>
#include <AppParCurves_Gradient.gxx>

