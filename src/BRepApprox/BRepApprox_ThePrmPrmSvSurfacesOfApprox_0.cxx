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

#include <BRepApprox_ThePrmPrmSvSurfacesOfApprox.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRepApprox_SurfaceTool.hxx>
#include <BRepApprox_ApproxLine.hxx>
#include <BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox.hxx>
#include <BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
 

#define ThePSurface BRepAdaptor_Surface
#define ThePSurface_hxx <BRepAdaptor_Surface.hxx>
#define ThePSurfaceTool BRepApprox_SurfaceTool
#define ThePSurfaceTool_hxx <BRepApprox_SurfaceTool.hxx>
#define Handle_TheLine Handle(BRepApprox_ApproxLine)
#define TheLine BRepApprox_ApproxLine
#define TheLine_hxx <BRepApprox_ApproxLine.hxx>
#define ApproxInt_TheInt2S BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox
#define ApproxInt_TheInt2S_hxx <BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox.hxx>
#define ApproxInt_TheFunctionOfTheInt2S BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox
#define ApproxInt_TheFunctionOfTheInt2S_hxx <BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox.hxx>
#define ApproxInt_TheFunctionOfTheInt2S BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox
#define ApproxInt_TheFunctionOfTheInt2S_hxx <BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox.hxx>
#define ApproxInt_PrmPrmSvSurfaces BRepApprox_ThePrmPrmSvSurfacesOfApprox
#define ApproxInt_PrmPrmSvSurfaces_hxx <BRepApprox_ThePrmPrmSvSurfacesOfApprox.hxx>
#include <ApproxInt_PrmPrmSvSurfaces.gxx>

