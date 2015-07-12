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

#include <GeomInt_ParLeastSquareOfMyGradientbisOfTheComputeLineOfWLApprox.hxx>

#include <StdFail_NotDone.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <math_Matrix.hxx>
 

#define MultiLine GeomInt_TheMultiLineOfWLApprox
#define MultiLine_hxx <GeomInt_TheMultiLineOfWLApprox.hxx>
#define ToolLine GeomInt_TheMultiLineToolOfWLApprox
#define ToolLine_hxx <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#define AppParCurves_LeastSquare GeomInt_ParLeastSquareOfMyGradientbisOfTheComputeLineOfWLApprox
#define AppParCurves_LeastSquare_hxx <GeomInt_ParLeastSquareOfMyGradientbisOfTheComputeLineOfWLApprox.hxx>
#include <AppParCurves_LeastSquare.gxx>

