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

#include <GeomInt_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfWLApprox.hxx>

#include <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#include <GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#include <GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#include <GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
 

#define MultiLine GeomInt_TheMultiLineOfWLApprox
#define MultiLine_hxx <GeomInt_TheMultiLineOfWLApprox.hxx>
#define ToolLine GeomInt_TheMultiLineToolOfWLApprox
#define ToolLine_hxx <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#define AppParCurves_BSpParLeastSquare GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox
#define AppParCurves_BSpParLeastSquare_hxx <GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#define AppParCurves_BSpParFunction GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox
#define AppParCurves_BSpParFunction_hxx <GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#define AppParCurves_BSpGradient_BFGS GeomInt_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfWLApprox
#define AppParCurves_BSpGradient_BFGS_hxx <GeomInt_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#define AppParCurves_BSpGradient GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox
#define AppParCurves_BSpGradient_hxx <GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#include <AppParCurves_BSpGradient_BFGS.gxx>

