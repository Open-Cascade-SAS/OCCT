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

#include <BRepApprox_TheComputeLineBezierOfApprox.hxx>

#include <BRepApprox_TheMultiLineOfApprox.hxx>
#include <BRepApprox_TheMultiLineToolOfApprox.hxx>
#include <BRepApprox_MyGradientOfTheComputeLineBezierOfApprox.hxx>
#include <BRepApprox_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#include <BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#include <BRepApprox_ParFunctionOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#include <BRepApprox_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>

#define MultiLine BRepApprox_TheMultiLineOfApprox
#define MultiLine_hxx <BRepApprox_TheMultiLineOfApprox.hxx>
#define LineTool BRepApprox_TheMultiLineToolOfApprox
#define LineTool_hxx <BRepApprox_TheMultiLineToolOfApprox.hxx>
#define Approx_MyGradient BRepApprox_MyGradientOfTheComputeLineBezierOfApprox
#define Approx_MyGradient_hxx <BRepApprox_MyGradientOfTheComputeLineBezierOfApprox.hxx>
#define Approx_ParLeastSquareOfMyGradient                                                          \
  BRepApprox_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfApprox
#define Approx_ParLeastSquareOfMyGradient_hxx                                                      \
  <BRepApprox_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#define Approx_ResConstraintOfMyGradient                                                           \
  BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox
#define Approx_ResConstraintOfMyGradient_hxx                                                       \
  <BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#define Approx_ParFunctionOfMyGradient                                                             \
  BRepApprox_ParFunctionOfMyGradientOfTheComputeLineBezierOfApprox
#define Approx_ParFunctionOfMyGradient_hxx                                                         \
  <BRepApprox_ParFunctionOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#define Approx_Gradient_BFGSOfMyGradient                                                           \
  BRepApprox_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfApprox
#define Approx_Gradient_BFGSOfMyGradient_hxx                                                       \
  <BRepApprox_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#define Approx_ParLeastSquareOfMyGradient                                                          \
  BRepApprox_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfApprox
#define Approx_ParLeastSquareOfMyGradient_hxx                                                      \
  <BRepApprox_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#define Approx_ResConstraintOfMyGradient                                                           \
  BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox
#define Approx_ResConstraintOfMyGradient_hxx                                                       \
  <BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#define Approx_ParFunctionOfMyGradient                                                             \
  BRepApprox_ParFunctionOfMyGradientOfTheComputeLineBezierOfApprox
#define Approx_ParFunctionOfMyGradient_hxx                                                         \
  <BRepApprox_ParFunctionOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#define Approx_Gradient_BFGSOfMyGradient                                                           \
  BRepApprox_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfApprox
#define Approx_Gradient_BFGSOfMyGradient_hxx                                                       \
  <BRepApprox_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfApprox.hxx>
#define Approx_ComputeLine BRepApprox_TheComputeLineBezierOfApprox
#define Approx_ComputeLine_hxx <BRepApprox_TheComputeLineBezierOfApprox.hxx>
#include "../Approx/Approx_ComputeLine.gxx"
