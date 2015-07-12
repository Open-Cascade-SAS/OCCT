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

#include <AppDef_TheGradient.hxx>

#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppDef_MyLineTool.hxx>
#include <AppDef_ParLeastSquareOfTheGradient.hxx>
#include <AppDef_ResConstraintOfTheGradient.hxx>
#include <AppDef_ParFunctionOfTheGradient.hxx>
#include <AppDef_Gradient_BFGSOfTheGradient.hxx>
#include <AppParCurves_MultiCurve.hxx>
 

#define MultiLine AppDef_MultiLine
#define MultiLine_hxx <AppDef_MultiLine.hxx>
#define ToolLine AppDef_MyLineTool
#define ToolLine_hxx <AppDef_MyLineTool.hxx>
#define AppParCurves_ParLeastSquare AppDef_ParLeastSquareOfTheGradient
#define AppParCurves_ParLeastSquare_hxx <AppDef_ParLeastSquareOfTheGradient.hxx>
#define AppParCurves_ResConstraint AppDef_ResConstraintOfTheGradient
#define AppParCurves_ResConstraint_hxx <AppDef_ResConstraintOfTheGradient.hxx>
#define AppParCurves_ParFunction AppDef_ParFunctionOfTheGradient
#define AppParCurves_ParFunction_hxx <AppDef_ParFunctionOfTheGradient.hxx>
#define AppParCurves_Gradient_BFGS AppDef_Gradient_BFGSOfTheGradient
#define AppParCurves_Gradient_BFGS_hxx <AppDef_Gradient_BFGSOfTheGradient.hxx>
#define AppParCurves_Gradient AppDef_TheGradient
#define AppParCurves_Gradient_hxx <AppDef_TheGradient.hxx>
#include <AppParCurves_Gradient.gxx>

