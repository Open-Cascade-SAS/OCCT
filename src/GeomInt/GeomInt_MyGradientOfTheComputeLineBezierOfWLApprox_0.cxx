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

#include <GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox.hxx>

#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#include <GeomInt_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <GeomInt_ParFunctionOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <GeomInt_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <AppParCurves_MultiCurve.hxx>
 

#define MultiLine GeomInt_TheMultiLineOfWLApprox
#define MultiLine_hxx <GeomInt_TheMultiLineOfWLApprox.hxx>
#define ToolLine GeomInt_TheMultiLineToolOfWLApprox
#define ToolLine_hxx <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#define AppParCurves_ParLeastSquare GeomInt_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfWLApprox
#define AppParCurves_ParLeastSquare_hxx <GeomInt_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define AppParCurves_ResConstraint GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox
#define AppParCurves_ResConstraint_hxx <GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define AppParCurves_ParFunction GeomInt_ParFunctionOfMyGradientOfTheComputeLineBezierOfWLApprox
#define AppParCurves_ParFunction_hxx <GeomInt_ParFunctionOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define AppParCurves_Gradient_BFGS GeomInt_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfWLApprox
#define AppParCurves_Gradient_BFGS_hxx <GeomInt_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define AppParCurves_Gradient GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox
#define AppParCurves_Gradient_hxx <GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <AppParCurves_Gradient.gxx>

