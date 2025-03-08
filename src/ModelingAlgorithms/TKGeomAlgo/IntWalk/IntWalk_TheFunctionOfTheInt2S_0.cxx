// Created on: 1991-03-28
// Created by: Jacques GOUSSARD
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

#include <IntWalk_TheFunctionOfTheInt2S.hxx>

#include <StdFail_UndefinedDerivative.hxx>
#include <Standard_ConstructionError.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <math_Matrix.hxx>

#define ThePSurface Handle(Adaptor3d_Surface)
#define ThePSurface_hxx <Adaptor3d_Surface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define IntImp_ZerParFunc IntWalk_TheFunctionOfTheInt2S
#define IntImp_ZerParFunc_hxx <IntWalk_TheFunctionOfTheInt2S.hxx>
#include "../IntImp/IntImp_ZerParFunc.gxx"
