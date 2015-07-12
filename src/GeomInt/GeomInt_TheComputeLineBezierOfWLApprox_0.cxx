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

#include <GeomInt_TheComputeLineBezierOfWLApprox.hxx>

#include <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#include <GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <GeomInt_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <GeomInt_ParFunctionOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <GeomInt_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
 

#define MultiLine GeomInt_TheMultiLineOfWLApprox
#define MultiLine_hxx <GeomInt_TheMultiLineOfWLApprox.hxx>
#define LineTool GeomInt_TheMultiLineToolOfWLApprox
#define LineTool_hxx <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#define Approx_MyGradient GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox
#define Approx_MyGradient_hxx <GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define Approx_ParLeastSquareOfMyGradient GeomInt_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfWLApprox
#define Approx_ParLeastSquareOfMyGradient_hxx <GeomInt_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define Approx_ResConstraintOfMyGradient GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox
#define Approx_ResConstraintOfMyGradient_hxx <GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define Approx_ParFunctionOfMyGradient GeomInt_ParFunctionOfMyGradientOfTheComputeLineBezierOfWLApprox
#define Approx_ParFunctionOfMyGradient_hxx <GeomInt_ParFunctionOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define Approx_Gradient_BFGSOfMyGradient GeomInt_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfWLApprox
#define Approx_Gradient_BFGSOfMyGradient_hxx <GeomInt_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define Approx_ParLeastSquareOfMyGradient GeomInt_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfWLApprox
#define Approx_ParLeastSquareOfMyGradient_hxx <GeomInt_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define Approx_ResConstraintOfMyGradient GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox
#define Approx_ResConstraintOfMyGradient_hxx <GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define Approx_ParFunctionOfMyGradient GeomInt_ParFunctionOfMyGradientOfTheComputeLineBezierOfWLApprox
#define Approx_ParFunctionOfMyGradient_hxx <GeomInt_ParFunctionOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define Approx_Gradient_BFGSOfMyGradient GeomInt_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfWLApprox
#define Approx_Gradient_BFGSOfMyGradient_hxx <GeomInt_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfWLApprox.hxx>
#define Approx_ComputeLine GeomInt_TheComputeLineBezierOfWLApprox
#define Approx_ComputeLine_hxx <GeomInt_TheComputeLineBezierOfWLApprox.hxx>
#include <Approx_ComputeLine.gxx>

