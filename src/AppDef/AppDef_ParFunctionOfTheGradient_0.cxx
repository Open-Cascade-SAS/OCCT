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

#include <AppDef_ParFunctionOfTheGradient.hxx>

#include <AppDef_MultiLine.hxx>
#include <AppDef_MyLineTool.hxx>
#include <AppDef_ParLeastSquareOfTheGradient.hxx>
#include <AppDef_ResConstraintOfTheGradient.hxx>
#include <AppParCurves_MultiCurve.hxx>
 

#define MultiLine AppDef_MultiLine
#define MultiLine_hxx <AppDef_MultiLine.hxx>
#define ToolLine AppDef_MyLineTool
#define ToolLine_hxx <AppDef_MyLineTool.hxx>
#define Squares AppDef_ParLeastSquareOfTheGradient
#define Squares_hxx <AppDef_ParLeastSquareOfTheGradient.hxx>
#define ResolCons AppDef_ResConstraintOfTheGradient
#define ResolCons_hxx <AppDef_ResConstraintOfTheGradient.hxx>
#define AppParCurves_Function AppDef_ParFunctionOfTheGradient
#define AppParCurves_Function_hxx <AppDef_ParFunctionOfTheGradient.hxx>
#include <AppParCurves_Function.gxx>

