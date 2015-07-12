// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#include <IntCurveSurface_TheCSFunctionOfHInter.hxx>

#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <IntCurveSurface_TheHCurveTool.hxx>
#include <math_Matrix.hxx>
#include <gp_Pnt.hxx>
 

#define ThePSurface Handle(Adaptor3d_HSurface)
#define ThePSurface_hxx <Adaptor3d_HSurface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define TheCurve Handle(Adaptor3d_HCurve)
#define TheCurve_hxx <Adaptor3d_HCurve.hxx>
#define TheCurveTool IntCurveSurface_TheHCurveTool
#define TheCurveTool_hxx <IntCurveSurface_TheHCurveTool.hxx>
#define IntImp_ZerCSParFunc IntCurveSurface_TheCSFunctionOfHInter
#define IntImp_ZerCSParFunc_hxx <IntCurveSurface_TheCSFunctionOfHInter.hxx>
#include <IntImp_ZerCSParFunc.gxx>

