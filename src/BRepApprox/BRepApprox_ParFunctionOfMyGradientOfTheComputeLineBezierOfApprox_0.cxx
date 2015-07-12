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

#include <BRepApprox_ParFunctionOfMyGradientOfTheComputeLineBezierOfApprox.hxx>

#include <BRepApprox_TheMultiLineOfApprox.hxx>
#include <BRepApprox_TheMultiLineToolOfApprox.hxx>
#include <BRepApprox_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#include <BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#include <AppParCurves_MultiCurve.hxx>
 

#define MultiLine BRepApprox_TheMultiLineOfApprox
#define MultiLine_hxx <BRepApprox_TheMultiLineOfApprox.hxx>
#define ToolLine BRepApprox_TheMultiLineToolOfApprox
#define ToolLine_hxx <BRepApprox_TheMultiLineToolOfApprox.hxx>
#define Squares BRepApprox_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfApprox
#define Squares_hxx <BRepApprox_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#define ResolCons BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox
#define ResolCons_hxx <BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#define AppParCurves_Function BRepApprox_ParFunctionOfMyGradientOfTheComputeLineBezierOfApprox
#define AppParCurves_Function_hxx <BRepApprox_ParFunctionOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#include <AppParCurves_Function.gxx>

