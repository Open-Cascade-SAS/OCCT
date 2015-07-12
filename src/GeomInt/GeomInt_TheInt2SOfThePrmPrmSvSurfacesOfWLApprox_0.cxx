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

#include <GeomInt_TheInt2SOfThePrmPrmSvSurfacesOfWLApprox.hxx>

#include <StdFail_NotDone.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_UndefinedDerivative.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <GeomInt_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfWLApprox.hxx>
#include <math_FunctionSetRoot.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
 

#define ThePSurface Handle(Adaptor3d_HSurface)
#define ThePSurface_hxx <Adaptor3d_HSurface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define IntImp_TheFunction GeomInt_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfWLApprox
#define IntImp_TheFunction_hxx <GeomInt_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfWLApprox.hxx>
#define IntImp_Int2S GeomInt_TheInt2SOfThePrmPrmSvSurfacesOfWLApprox
#define IntImp_Int2S_hxx <GeomInt_TheInt2SOfThePrmPrmSvSurfacesOfWLApprox.hxx>
#include <IntImp_Int2S.gxx>

